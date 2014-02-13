/*
 * Copyright (C) 2014 Bjoern Biesenbach <bjoern at bjoern-b.de>
 *               2014 homerj00			<homerj00 at web.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#include "DDS_TWO.h"
#include "buttons.h"
#include "drehgeber.h"
#include "ks0108/ks0108.h"
#include "page_misc.h"
#include "page_singletone.h"
#include "page_graph.h"
#include "page_settings.h"
#include "ad9910.h"
#include "portbits.h"
#include "uart.h"
#include "settings.h"

#define UART_BAUDRATE 115200

#define NUM_PAGES 4
static struct menuitem menu[] = {
    {NULL, 0, update_singletone, singletone_drehgeber, singletone_button_pressed, page_singletone},
    {NULL, 1, NULL, settings_drehgeber, settings_button_pressed, page_settings},
    {NULL, 2, draw_graph, NULL, NULL, page_graph},
    {NULL, 3, draw_graph, NULL, NULL, page_graph}
};

uint8_t refreshFlags;

#define SEC_HALF 0
void io_init(void)
{
    // PULLUPS
    PORTA = (1 << KEY0) | (1 << KEY1) | (1 << KEY2) | (1 << KEY3) | (1 <<KEY4);
    PORTA |= (1 << DREHGEBER_A) | (1 << DREHGEBER_B);
    // LED BL
    DDRF = (1 << PF3);
    PORTF = (1 << PF3);
    // LCD RESET
    DDRC |= (1 << PC2);
}

int main(void)
{
    int8_t drehgeber_delta = 0;
    int8_t menu_position = 0;
    int8_t old_menu_position = 0;
    uint8_t focus_here = 1;
    void (*reset) (void) = 0x0;
    void (*bootloader) (void) = 0x1f800;
    uint8_t on_off = 0;

    menu[0].name = PSTR("Single Tone");
    menu[1].name = PSTR("Sweep settings");
    menu[2].name = PSTR("Reflection");
    menu[3].name = PSTR("Transmission");
    menu[4].name = PSTR("DDS TWO 2");
    menu[5].name = PSTR("DDS TWO 2");

    io_init();
    ks0108Init();
    drehgeber_init();
    ad9910_init();
    uart_init(UART_BAUD_SELECT_DOUBLE_SPEED(UART_BAUDRATE, F_CPU));

    draw_page_header(&menu[menu_position]);
    menu[menu_position].draw_func(&menu[menu_position]);
    adcCalibOffset();

    sei();
    uart_puts("DDS\r\n");
    while (1) {

        //draw_page_header(&menu[0]);
        //TODO:: Please write your application code
        if (uart_getc() == 'a') {
            cli();
            printBootloader();
            wdt_enable(WDTO_15MS);
            while (1);
        }
        if ((drehgeber_delta = drehgeber_read())) {     // Drehgeber in action
            if (focus_here) {
                menu_position += drehgeber_delta;
                if (menu_position > (NUM_PAGES - 1))
                    menu_position = NUM_PAGES - 1;
                if (menu_position < 0)
                    menu_position = 0;
                if (menu_position != old_menu_position) {
                    if (menu[menu_position].draw_func) {
                        menu[menu_position].draw_func(&menu
                                                      [menu_position]);
                    }
                    draw_page_header(&menu[menu_position]);
                    old_menu_position = menu_position;
                }
            } else {
                if (menu[menu_position].drehgeber_func) {
                    menu[menu_position].drehgeber_func(&menu
                                                       [menu_position],
                                                       drehgeber_delta);
                }
            }
        }
        if (get_key_press(1 << KEY0)) { //button left
            if (menu[menu_position].taster_func) {
                focus_here =
                    menu[menu_position].taster_func(&menu[menu_position],
                                                    0);
            }
            settings.output_active ^= 1;
            dds_power(settings.output_active);
        }
        if (get_key_press(1 << KEY1)) { //button left +1
            if (menu[menu_position].taster_func) {
                focus_here =
                    menu[menu_position].taster_func(&menu[menu_position],
                                                    1);
            }
            adcCalibOffset();
        }
        if (get_key_press(1 << KEY2)) { //button right -1
            if (menu[menu_position].taster_func) {
                focus_here =
                    menu[menu_position].taster_func(&menu[menu_position],
                                                    2);
            }
        }
        if (get_key_press(1 << KEY3)) { //button right
            if (menu[menu_position].taster_func) {
                focus_here =
                    menu[menu_position].taster_func(&menu[menu_position],
                                                    3);
            }
        }
        if (get_key_press(1 << KEY4)) { //button encoder
            if (menu[menu_position].taster_func) {
                focus_here =
                    menu[menu_position].taster_func(&menu[menu_position],
                                                    4);
            }
        }
        if (refreshFlags & (1 << SEC_HALF)) {
            if (menu[menu_position].refresh_func) {
                menu[menu_position].refresh_func(&menu[menu_position],
                                                 SEC_HALF);
            }
            refreshFlags &= ~(1 << SEC_HALF);
        }
    }
}

ISR(TIMER0_COMP_vect)           // 1ms
{
    cli();
    uint8_t toggle = 0;
    static uint16_t prescaler = 3000;   // 5000 = 1s
    drehgeber_work();
    if (--prescaler == 0) {
        refreshFlags |= (1 << SEC_HALF);
        prescaler = 1000;
    } else if (!(prescaler % 10)) {     // 10ms
        buttons_every_10_ms();
    }
    sei();
}
