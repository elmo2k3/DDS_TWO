#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <stdio.h>
#include "page_misc.h"
#include "page_singletone.h"
#include "page_graph.h"
#include "page_settings.h"
#include "buttons.h"
#include "drehgeber.h"
#include "settings.h"
#include "ad9910.h"


#define SEC_HALF 0

uint8_t NUM_PAGES;
volatile uint8_t refreshFlags;

int8_t drehgeber_delta;
int8_t menu_position;
int8_t old_menu_position;
uint8_t focus_here;

//#define NUM_PAGES 4
static struct menuitem menu[] = {
//    Page Name | Number | Periodic | Drehgeber | Button | Init page
    {NULL, 0, update_singletone, singletone_drehgeber, singletone_button_pressed, page_singletone},
    {NULL, 1, NULL, settings_drehgeber, settings_button_pressed, page_settings},
    {NULL, 2, draw_graph, NULL, NULL, page_graph},
    {NULL, 3, draw_graph, NULL, NULL, page_graph}
};

void menu_init()
{
    //NUM_PAGES = sizeof(struct menuitem) * sizeof(menu);
    NUM_PAGES = 4;
    menu[0].name = PSTR("Single Tone");
    menu[1].name = PSTR("Sweep settings");
    menu[2].name = PSTR("Reflection");
    menu[3].name = PSTR("Transmission");
    
    draw_page_header(&menu[menu_position]);
    menu[menu_position].draw_func(&menu[menu_position]);
    focus_here = 1;

}

void menu_tick()
{
    refreshFlags |= (1 << SEC_HALF);
}

void menu_task()
{
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
    }
    if (get_key_press(1 << KEY2)) { //button right -1
        if (menu[menu_position].taster_func) {
            focus_here =
                menu[menu_position].taster_func(&menu[menu_position],
                                                2);
        }
    }
    if (get_key_press(1 << KEY3)) { //button right
        wdt_enable(WDTO_15MS);
        while (1);
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
