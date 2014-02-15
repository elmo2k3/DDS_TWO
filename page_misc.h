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

#ifndef __PAGE_MISC_H___
#define __PAGE_MISC_H__

#include <avr/pgmspace.h>
#include <inttypes.h>
#include "DDS_TWO.h"


void clear_page_header(void);
void clear_page_main(void);
void draw_page_header(struct menuitem *self);
void draw_arrow(uint8_t x, uint8_t y);
void draw_settings_line(PGM_P name, char *value, uint8_t position);
void draw_settings_line_variable_space(PGM_P name, char *value,
                                       uint8_t position, uint8_t space);
void draw_centered_string(char *value, uint8_t position);

char *intToString(int8_t value, uint8_t one_leading_zero);

#endif
