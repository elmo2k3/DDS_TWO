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
#include "ad9910.h"
#include "portbits.h"
#include "uart.h"
#include "settings.h"
#include "adc.h"
#include "menu.h"

#define UART_BAUDRATE 115200


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


    settings_init();
    io_init();
    ks0108Init();
    drehgeber_init();
    ad9910_init();
    uart_init(UART_BAUD_SELECT_DOUBLE_SPEED(UART_BAUDRATE, F_CPU));
    menu_init();

    adcCalibOffset();

    sei();
    uart_puts("DDS\r\n");
    while (1) {

        if (uart_getc() == 'a') {
            cli();
            printBootloader();
            wdt_enable(WDTO_15MS);
            while (1);
        }
        menu_task();
    }
}

ISR(TIMER0_COMP_vect)           // 1ms
{
    cli();
    static uint16_t prescaler = 3000;   // 5000 = 1s
    drehgeber_work();
    if (--prescaler == 0) {
        menu_tick();
        prescaler = 1000;
    } else if (!(prescaler % 10)) {     // 10ms
        buttons_every_10_ms();
    }
    sei();
}
