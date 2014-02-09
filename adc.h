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

#ifndef __ADC_H__
#define __ADC_H__

#define PD_REFLECT 0
#define PD_FORWARD 1
#define PD_TRANSMISSION 2

uint16_t getPDValue (uint8_t mux);
void adcCalibOffset (void);

#endif
