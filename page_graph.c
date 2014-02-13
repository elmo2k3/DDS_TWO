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
#include <math.h>
#include <stdio.h>
#include "page_misc.h"
#include "ks0108/ks0108.h"
#include "page_graph.h"
#include "ad9910.h"
#include "adc.h"
#include "settings.h"

void drawCoordinateSystem(void)
{
    for (char counter = 0; counter < 26; counter++) {
//        if(counter == 12)
//            ks0108DrawLine(7+counter*5,58,7+counter*5,63, BLACK);
//        else
        ks0108DrawLine(counter * 5 + 1, 60, counter * 5 + 1, 63, BLACK);
    }
    ks0108DrawHoriLine(1, 60, 125, BLACK);
    //ks0108DrawHoriLine(13,20,3,BLACK);
    //ks0108DrawHoriLine(13,40,3,BLACK);
    //ks0108DrawHoriLine(124,40,3,BLACK);
}

void drawMinMaxTemps(void)
{
    char *buf;
    ks0108GotoXY(100, 16);
    buf = intToString(1, 0);
    ks0108Puts(buf);
    ks0108Puts(".");
    buf = intToString(1, 0);
    ks0108Puts(buf);
    ks0108Puts("C");
    ks0108GotoXY(100, 26);
    buf = intToString(1, 0);
    ks0108Puts(buf);
    ks0108Puts(".");
    buf = intToString(1, 0);
    ks0108Puts(buf);
    ks0108Puts("C");
}

void clearGraph(void)
{
    ks0108FillRect(0, 20, 127, 43, WHITE);
}

void drawYLegend(int temp_max, int temp_min)
{
    char *buf;
    ks0108GotoXY(1, 47);
    buf = intToString(temp_min, 0);
    ks0108Puts(buf);
    ks0108GotoXY(1, 19);
    buf = intToString(temp_max, 0);
    ks0108Puts(buf);
}

void draw_graph(struct menuitem *self)
{
    int y1, y2;
    int temp_max, temp_min;
    uint16_t power_val;
    uint16_t power_val_last;
    uint32_t frequency,f_step;

    //clearGraph();
    drawCoordinateSystem();
    //drawMinMaxTemps();
    temp_max = 10;
    temp_min = 0;

    f_step = (settings.graph_settings.upper_frequency -
                settings.graph_settings.lower_frequency) / 126ul;

    for (uint8_t i = 0; i < 126; i++) {
        frequency = settings.graph_settings.lower_frequency + f_step*i;
        dds_set_single_tone_frequency(10, frequency);
        if (self->num == 2) {
            power_val = getPDValue(PD_REFLECT);
        } else if (self->num == 3) {
            power_val = getPDValue(PD_TRANSMISSION);
        }
        ks0108FillRect(i + 1, 15, 1, 44, WHITE);
        if (i == 0) {
            ks0108SetDot(i + 1, 60 - power_val / 2, BLACK);
        } else {
            ks0108DrawLine(i, 60 - power_val_last / 2, i + 1,
                           60 - power_val / 2, BLACK);
        }
        power_val_last = power_val;
    }

//    for(uint8_t counter=0;counter<graphData.numberOfPoints;counter++){
//        y1 = graphData.temperature_history[counter-1];
//        y2 = graphData.temperature_history[counter];
//        if(counter!=0)
//            ks0108DrawLine(7+counter,60-y1,8+counter,60-y2,BLACK);
//    }
    //drawYLegend(temp_max,temp_min);
}

void page_graph(struct menuitem *self)
{
    clear_page_main();
    draw_graph(self);
}
