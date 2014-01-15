/*
 * Copyright (C) 2011-2012 Bjoern Biesenbach <bjoern@bjoern-b.de>
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


uint16_t getPDValue(uint8_t mux)
{
	uint16_t avg = 0;

	ADCSRA = (1<<ADEN)|(1<<ADPS2); // prescaler 16
	ADMUX = (1<<REFS0) | mux; // vcc reference and ADC0 Channel

	ADCSRA |= 1<<ADSC;
	while(ADCSRA & (1<<ADSC)); // throw away first measurement

	ADCSRA |= 1<<ADSC;
	while(ADCSRA & (1<<ADSC));

	avg = ADC;
	ADCSRA &= ~(1<<ADEN); //ADC disable for powersaving
	return avg;
}
