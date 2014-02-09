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
#include "adc.h"

static uint16_t offset_reflect;
static uint16_t offset_forward;
static uint16_t offset_transmit;

void
adcCalibOffset ()
{
  uint16_t min1, min2, min3;
  uint16_t val;
  uint8_t i;

  offset_reflect = 0;
  offset_forward = 0;
  offset_transmit = 0;

  min1 = 1025;
  min2 = 1025;
  min3 = 1025;

  for (i = 0; i < 10; i++)
    {
      if ((val = getPDValue (PD_REFLECT)) < min1)
	min1 = val;
      if ((val = getPDValue (PD_FORWARD)) < min2)
	min2 = val;
      if ((val = getPDValue (PD_TRANSMISSION)) < min3)
	min3 = val;
    }
  offset_reflect = min1;
  offset_forward = min2;
  offset_transmit = min3;
}

uint16_t
getPDValue (uint8_t mux)
{
  uint16_t avg = 0;

  ADCSRA = (1 << ADEN) | (1 << ADPS2);	// prescaler 16
  ADMUX = (1 << REFS0) | mux;	// vcc reference and ADC0 Channel

  ADCSRA |= 1 << ADSC;
  while (ADCSRA & (1 << ADSC));	// throw away first measurement

  ADCSRA |= 1 << ADSC;
  while (ADCSRA & (1 << ADSC));

  avg = ADC;
  ADCSRA &= ~(1 << ADEN);	//ADC disable for powersaving

  if (mux == PD_REFLECT)
    avg -= offset_reflect;
  else if (mux == PD_FORWARD)
    avg -= offset_forward;
  else if (mux == PD_TRANSMISSION)
    avg -= offset_transmit;

  return avg;
}
