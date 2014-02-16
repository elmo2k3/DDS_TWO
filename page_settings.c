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

#include <string.h>
#include <avr/eeprom.h>
#include <math.h>
#include <stdio.h>

#include "DDS_TWO.h"
#include "page_misc.h"
#include "ks0108/ks0108.h"
#include "page_settings.h"
#include "settings.h"

static uint8_t state;
static enum cursor_state {
    STATE_IDLE,
    STATE_FREQUENCY_START,
    STATE_FREQUENCY_STOP
};

void draw_frequency_start(void)
{
    char f_val[10];

    snprintf(f_val,10,"%ld",settings.graph_settings.lower_frequency/1000000ul); 
    draw_settings_line(PSTR("F_Start [MHz]"),f_val,0);
}

void draw_frequency_stop(void)
{
    char f_val[10];

    snprintf(f_val,10,"%ld",settings.graph_settings.upper_frequency/1000000ul); 
    draw_settings_line(PSTR("F_Stop [MHz]"),f_val,1);
}

void page_settings(struct menuitem *self)
{
    clear_page_main();
    ks0108SelectFont(1, BLACK);
    draw_frequency_start();
    draw_frequency_stop();
}

void update_settings(struct menuitem *self, uint8_t event)
{
}

uint8_t settings_button_pressed(struct menuitem *self, uint8_t button)
{
    if(button != 4){
        return state!=0;
    }
    if (state == STATE_IDLE) {
        state = STATE_FREQUENCY_START;
        draw_arrow(90, 20);
    } else if (state == STATE_FREQUENCY_START) {
        state = STATE_FREQUENCY_STOP;
        ks0108FillRect(90, 17, 5, 10, WHITE);
        draw_arrow(90, 31);
    } else if (state == STATE_FREQUENCY_STOP) {
        state = STATE_IDLE;
        ks0108FillRect(90, 17, 5, 10, WHITE);
        ks0108FillRect(90,28,5,10,WHITE);
        return 1;
    }
    return 0;
}

void settings_drehgeber(struct menuitem *self, int8_t steps)
{
    switch (state) {
    case STATE_FREQUENCY_START:
        settings.graph_settings.lower_frequency += (uint32_t) steps *1000ul * 1000ul;
        if (settings.graph_settings.lower_frequency> 1000000000)
            settings.graph_settings.lower_frequency = 0;
        if (settings.graph_settings.lower_frequency > 500000000)
            settings.graph_settings.lower_frequency = 500000000;
        if (settings.graph_settings.lower_frequency > settings.graph_settings.upper_frequency)
            settings.graph_settings.lower_frequency = settings.graph_settings.upper_frequency;
        draw_frequency_start();
        break;
    case STATE_FREQUENCY_STOP:
        settings.graph_settings.upper_frequency += (uint32_t) steps *1000ul * 1000ul;
        if (settings.graph_settings.upper_frequency > 1000000000)
            settings.graph_settings.upper_frequency = 0;
        if (settings.graph_settings.upper_frequency > 500000000)
            settings.graph_settings.upper_frequency = 500000000;
        if (settings.graph_settings.upper_frequency < settings.graph_settings.lower_frequency)
            settings.graph_settings.upper_frequency = settings.graph_settings.lower_frequency;
        draw_frequency_stop();
        break;
    }
}
