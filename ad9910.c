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
#include "settings.h"
#include "ad9910.h"

#define DDS_REGISTER_CFR1 0x00
#define DDS_REGISTER_CFR1_LENGTH 4
#define DDS_REGISTER_CFR2 0x01
#define DDS_REGISTER_CFR2_LENGTH 4
#define DDS_REGISTER_CFR3 0x02
#define DDS_REGISTER_CFR3_LENGTH 4
#define DDS_REGISTER_FTW 0x07
#define DDS_REGISTER_FTW_LENGTH 4
#define DDS_REGISTER_SINGLE_TONE0 0x0E
#define DDS_REGISTER_SINGLE_TONE_LENGTH 8

#define PLL_ENABLE (1<<8)
#define PFD_RESET (1<<10)
#define REFCLK_IN_DIV_RESET_B (1<<14)
#define REFCLK_IN_DIV_BYPASS (1<<15)
#define PLL_N(X) ((uint32_t)X<<1)
#define PLL_ICP(X) ((uint32_t)X<<19)
#define PLL_VCO_SEL(X) ((uint32_t)X<<24)
#define PLL_DRV0(X) ((uint32_t)X<<28)
#define DAC_POWER_DOWN 6
#define DDS_POWER_DOWN 7

#define DDS_CS_SET() 	DDS_PORT0 |= (1<<DDS_CS);
#define DDS_CS_CLR() 	DDS_PORT0 &= ~(1<<DDS_CS);
#define DDS_DO_RESET()		DDS_PORT0 |= (1<<DDS_MASTER_RESET); DDS_PORT0 &= ~(1<<DDS_MASTER_RESET);
#define DDS_DO_IOUPDATE()	DDS_PORT0 |= (1<<DDS_IO_UPDATE); DDS_PORT0 &= ~(1<<DDS_IO_UPDATE);

//#define FREQUENCY_FACTOR 2235 // divide by 10000
#define FREQUENCY_FACTOR 4.474705

static uint8_t *uint64_to_uint8_array(uint64_t value)
{
    uint8_t i;
    static uint8_t values[8];

    for (i = 0; i < 8; i++) {
        values[8 - i - 1] = (value >> ((8 - i - 1) * 8)) & 0xFF;
    }
    return values;
}

static uint8_t *uint32_to_uint8_array(uint32_t value)
{
    uint8_t i;
    static uint8_t values[4];

    for (i = 0; i < 4; i++) {
        values[4 - i - 1] = (value >> ((4 - i - 1) * 8)) & 0xFF;
    }
    return values;
}

static void dds_cmd(uint8_t address, uint8_t read_access, uint8_t * values,
                    uint8_t size)
{
    uint8_t i;
    uint32_t temp;

    DDS_CS_CLR();               // Start transfer

    SPDR = (address & 0x1F) | (read_access << 7);
    while (!(SPSR & (1 << SPIF)));      // wait until transmission finished

    for (i = 0; i < size; i++) {
        // Start with upper byte (MSB first)
        temp = values[size - i - 1];
        SPDR = temp & 0xFF;
        while (!(SPSR & (1 << SPIF)));  // wait until transmission finished
    }

    DDS_CS_SET();               // End of transfer
    _delay_ms(1);
}

static void dds_cmd_32(uint8_t address, uint8_t read_access,
                       uint32_t value, uint8_t size)
{
    uint8_t *values;

    if (size > 4)
        return;

    values = uint32_to_uint8_array(value);
    dds_cmd(address, read_access, values, size);
}

static void dds_cmd_64(uint8_t address, uint8_t read_access,
                       uint64_t value, uint8_t size)
{
    uint8_t *values;

    if (size > 8)
        return;

    values = uint64_to_uint8_array(value);
    dds_cmd(address, read_access, values, size);
}

void dds_power(uint8_t on)
{
    if(!on){ // off
        PORTA &= ~(1 << OUTPUT_LED);
        dds_cmd_32(DDS_REGISTER_CFR1, 0, (1 << DDS_POWER_DOWN),
                   DDS_REGISTER_CFR1_LENGTH);
    }else{ // on
        PORTA |= (1 << OUTPUT_LED);
        dds_cmd_32(DDS_REGISTER_CFR1, 0, (0 << DDS_POWER_DOWN),
                   DDS_REGISTER_CFR1_LENGTH);
    }
    DDS_DO_IOUPDATE();
}

void ad9910_init()
{
    DDRB |=
        (1 << DDS_CS) | (1 << DDS_SCK) | (1 << DDS_MOSI) | (1 <<
                                                            DDS_MASTER_RESET)
        | (1 << DDS_IO_UPDATE) | (1 << DDS_IO_RESET);
    //DDRB |=  (1<<DDS_SCK) | (1<<DDS_MOSI) | (1<<DDS_MASTER_RESET) | (1<<DDS_IO_UPDATE) | (1<<DDS_IO_RESET);
    PORTB |= (1 << DDS_CS) | (1 << DDS_IO_RESET) | (1 << DDS_MASTER_RESET);
    _delay_us(1);
    PORTB &= ~((1 << DDS_IO_RESET) | (1 << DDS_MASTER_RESET));

    DDRE |=
        (1 << DDS_OSK) | (1 << DDS_DRCTL) | (1 << DDS_PROFILE0) | (1 <<
                                                                   DDS_PROFILE1)
        | (1 << DDS_PROFILE2);
    PORTE |= (1 << DDS_OSK);
    PORTE &= ~(1 << DDS_DRCTL);

    DDRG = (1 << PGA0) | (1 << PGA1) | (1 << PGA2) | (1 << PGA3);
    PORTG = (0 << PGA0) | (1 << PGA1) | (0 << PGA2) | (1 << PGA3);
    // LED
    DDRA = (1 << OUTPUT_LED);

    //SPI
    SPCR |=
        (1 << SPE) | (1 << MSTR) | (1 << SPR1) | (1 << SPR0) | (0 << CPOL);
//      SPSR |= (1<<SPI2X);

    DDS_DO_RESET();

    dds_cmd_32(DDS_REGISTER_CFR3, 0,
               PLL_N(40) | PLL_ENABLE | REFCLK_IN_DIV_RESET_B | PLL_DRV0(1)
               | PLL_VCO_SEL(5) | PLL_ICP(3), DDS_REGISTER_CFR3_LENGTH);
    DDS_DO_IOUPDATE();
    dds_power(0);
}

void dds_set_single_tone_frequency(uint16_t amplitude, uint32_t frequency)
{
    uint64_t val;
    uint16_t phase = 0;

    if(!settings.output_active){
        return;
    }

    val =
        ((uint64_t) amplitude & 0x3FFF) << 48 | (uint64_t) phase << 32 |
        (uint32_t) (((double) frequency * FREQUENCY_FACTOR));
    dds_cmd_64(DDS_REGISTER_SINGLE_TONE0, 0, val,
               DDS_REGISTER_SINGLE_TONE_LENGTH);
    DDS_DO_IOUPDATE();
}

void pga_set_gain(uint8_t gain)
{
    gain = (gain & 0x03) | ((gain & 0x8) >> 1) | ((gain & 0x04) << 1);
    PORTG = gain & (1 << PGA0 | 1 << PGA1 | 1 << PGA2 | 1 << PGA3);
}
