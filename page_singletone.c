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
#include <stdio.h>
#include "ks0108/ks0108.h"
#include "page_singletone.h"
#include "page_misc.h"
#include "page_graph.h"
#include "ad9910.h"
#include "adc.h"

static uint8_t focus_not_here = 1;
static uint8_t state;

uint32_t frequency = 50000000;
int8_t gain = 8;
static uint8_t toggle = 0;

static enum cursor_state {
    STATE_IDLE,
    STATE_FREQUENCY_MHZ,
    STATE_FREQUENCY_KHZ,
    STATE_GAIN
};

uint32_t getTcData(void)
{

    uint8_t i;
    uint32_t data = 0;

    PORTF &= ~(1 << PF3);
    for (i = 0; i < 4; i++) {
        SPDR = 0x00;
        while (!(SPSR & (1 << SPIF)));  // wait until transmission finished
        data |= (uint32_t) SPDR << ((3 - i) * 8);
    }

    PORTF |= (1 << PF3);

    return data;
}

void printForwardPower(uint8_t update)
{
    char str[15];
    static int16_t last_val = -1;
    int16_t current_val;

    if (update) {
        last_val = -1;
    }
    current_val = getPDValue(PD_FORWARD);

    if (current_val != last_val) {
        ks0108FillRect(60, 16, 20, 10, WHITE);
        sprintf(str, "%d", current_val);
        ks0108SelectFont(1, BLACK);
        ks0108GotoXY(0, 17);
        ks0108Puts_P(PSTR("Forward"));
        ks0108GotoXY(60, 17);
        ks0108Puts(str);
    }
    last_val = current_val;
}

void printReflectPower(uint8_t update)
{
    char str[15];
    static int16_t last_val = -1;
    int16_t current_val;

    if (update) {
        last_val = -1;
    }
    current_val = getPDValue(PD_REFLECT);

    if (current_val != last_val) {
        ks0108FillRect(60, 16 + 10, 20, 10, WHITE);
        sprintf(str, "%d", current_val);
        ks0108SelectFont(1, BLACK);
        ks0108GotoXY(0, 17 + 10);
        ks0108Puts_P(PSTR("Reverse"));
        ks0108GotoXY(60, 17 + 10);
        ks0108Puts(str);
    }
    last_val = current_val;
}

void printTransmitPower(uint8_t update)
{
    char str[15];
    static int16_t last_val = -1;
    int16_t current_val;
    if (update) {
        last_val = -1;
    }

    current_val = getPDValue(PD_TRANSMISSION);

    if (current_val != last_val) {
        ks0108FillRect(60, 16 + 20, 20, 10, WHITE);
        sprintf(str, "%d", current_val);
        ks0108SelectFont(1, BLACK);
        ks0108GotoXY(0, 17 + 20);
        ks0108Puts_P(PSTR("Received"));
        ks0108GotoXY(60, 17 + 20);
        ks0108Puts(str);
    }
    last_val = current_val;
}

void printGain(uint8_t toggle)
{
    char str[15];
    static uint8_t gain_last = 16;

    if (gain != gain_last) {
        sprintf(str, "v = %02d", gain);
        ks0108SelectFont(3, BLACK);
        ks0108FillRect(93, 28, 34, 20, WHITE);
        ks0108GotoXY(85, 28);
        ks0108Puts(str);
        gain_last = gain;
    } else if (toggle) {
        ks0108FillRect(106, 28, 21, 20, WHITE);
        gain_last = 16;
    }
}

void printFrequencyMhz(uint8_t toggle)
{
    char str[15];
    uint32_t mhz;
    static uint32_t frequency_last = 500000001;

    if (frequency != frequency_last) {
        mhz = frequency / 1000000ul;
        sprintf(str, "f = %03ld", mhz);
        ks0108SelectFont(3, BLACK);
        ks0108FillRect(12, 48, 33, 20, WHITE);
        ks0108GotoXY(12, 48);
        ks0108Puts(str);
        frequency_last = frequency;
    } else if (toggle) {
        ks0108FillRect(25, 48, 33, 20, WHITE);
        frequency_last = 500000001;
    }
}

void printFrequencyKhz(uint8_t toggle)
{
    char str[15];
    uint32_t mhz, khz;
    static uint32_t frequency_last = 500000001;

    if (frequency != frequency_last) {
        mhz = frequency / 1000000ul;
        khz = (frequency - mhz * 1000000ul) / 1000ul;
        sprintf(str, ".%03ldMHz", khz);
        ks0108SelectFont(3, BLACK);
        ks0108FillRect(60, 48, 66, 20, WHITE);
        ks0108GotoXY(60, 48);
        ks0108Puts(str);
        frequency_last = frequency;
    } else if (toggle) {
        ks0108FillRect(60, 48, 66, 20, WHITE);
        frequency_last = 500000001;
    }
}

void page_singletone(struct menuitem *self)
{

    clear_page_main();
    printGain(1);
    printGain(0);
    printFrequencyMhz(1);
    printFrequencyMhz(0);
    printFrequencyKhz(1);
    printFrequencyKhz(0);
    printForwardPower(1);
    printReflectPower(1);
    printTransmitPower(1);
    dds_set_single_tone_frequency(10, frequency);
}


void printBootloader()
{
    clear_page_main();
    ks0108SelectFont(3, BLACK);
    ks0108GotoXY(14, 46);
    ks0108Puts_P(PSTR("Bootloader.."));
}

void update_singletone(struct menuitem *self, uint8_t event)
{

    //clear_page_singletone();
    //
    uint32_t tcData;

    //tcData = getTcData();

    //frequency = (tcData>>4)&0xFFF;
    //frequency = (tcData>>18)&0x3FFF;
    //frequency = frequency *1000000/4;

    //printFrequencyMhz(0);
    //printFrequencyKhz(0);

    printForwardPower(0);
    printReflectPower(0);
    printTransmitPower(0);
    switch (state) {
    case STATE_FREQUENCY_MHZ:
        printFrequencyMhz(toggle == 0);
        break;
    case STATE_FREQUENCY_KHZ:
        printFrequencyMhz(0);
        printFrequencyKhz(toggle == 0);
        break;
    case STATE_GAIN:
        printFrequencyKhz(0);
        printGain(toggle == 0);
        break;
    default:
        printGain(0);
    }
    if (++toggle == 10)
        toggle = 0;
}

uint8_t singletone_button_pressed(struct menuitem *self, uint8_t button)
{

    toggle = 0;
    if (button == 4) {
        focus_not_here = 0;
        if (state == STATE_IDLE) {
            state = STATE_FREQUENCY_MHZ;
        } else if (state == STATE_FREQUENCY_MHZ) {
            state = STATE_FREQUENCY_KHZ;
        } else if (state == STATE_FREQUENCY_KHZ) {
            state = STATE_GAIN;
        } else if (state == STATE_GAIN) {
            state = STATE_IDLE;
            focus_not_here = 1;
        }
    } else if (button == 2) {
    } else if (button == 4) {
    }

    return focus_not_here;
}

void singletone_drehgeber(struct menuitem *self, int8_t steps)
{

    switch (state) {
    case STATE_GAIN:
        gain += steps;
        if (gain < 0)
            gain = 0;
        if (gain > 15)
            gain = 15;
        pga_set_gain(gain);
        printGain(0);
        break;
    case STATE_FREQUENCY_MHZ:
        frequency += (uint32_t) steps *1000ul * 1000ul;
        if (frequency > 1000000000)
            frequency = 0;
        if (frequency > 500000000)
            frequency = 500000000;
        dds_set_single_tone_frequency(10, frequency);
        printFrequencyMhz(0);
        break;
    case STATE_FREQUENCY_KHZ:
        frequency += (uint32_t) steps *1000ul;
        if (frequency > 1000000000)
            frequency = 0;
        if (frequency > 500000000)
            frequency = 500000000;
        dds_set_single_tone_frequency(10, frequency);
        printFrequencyKhz(0);
        break;
    }
}
