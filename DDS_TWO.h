#ifndef __DDS_TWO_H__
#define __DDS_TWO_H__

#include <stdint.h>
#include <avr/pgmspace.h>

struct menuitem
{
    PGM_P name;
    uint8_t num;
    void (*refresh_func)(struct menuitem *self, uint8_t event);
    void (*drehgeber_func)(struct menuitem *self, int8_t steps);
    uint8_t (*taster_func)(struct menuitem *self, uint8_t taster);
    void (*draw_func)(struct menuitem *self);
};

#endif

