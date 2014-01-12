#ifndef __PAGE_MISC_H___
#define __PAGE_MISC_H__

#include <avr/pgmspace.h>
#include <inttypes.h>


void clear_page_header(void);
void clear_page_main(void);
void draw_page_header(struct menuitem *self);
void draw_arrow(uint8_t x, uint8_t y);
void draw_settings_line(PGM_P name, char *value, uint8_t position);
void draw_settings_line_variable_space(PGM_P name, char *value, uint8_t position, uint8_t space);
void draw_centered_string(char *value, uint8_t position);

char *intToString(int8_t value, uint8_t one_leading_zero);

#endif

