#ifndef __PAGE_SETTINGS_H__
#define __PAGE_SETTINGS_H__

#include <stdint.h>
#include "DDS_TWO.h"

void page_settings(struct menuitem *self);
void update_settings(struct menuitem *self, uint8_t event);
uint8_t settings_button_pressed(struct menuitem *self, uint8_t button);
void settings_drehgeber(struct menuitem *self, int8_t steps);
void save_settings(void);
void load_settings(void);

#endif

