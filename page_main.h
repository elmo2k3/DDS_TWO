#ifndef __PAGE_MAIN_H__
#define __PAGE_MAIN_H__

#include "DDS_TWO.h"

void update_main(struct menuitem *self, uint8_t event);
void page_main(struct menuitem *self);
uint8_t main_button_pressed(struct menuitem *self, uint8_t button);
void main_drehgeber(struct menuitem *self, int8_t steps);
void printBootloader(void);

#endif

