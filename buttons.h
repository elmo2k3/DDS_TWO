#ifndef __BUTTONS_H__
#define __BUTTONS_H__

#include <stdint.h>

//code taken from http://www.mikrocontroller.net/articles/Entprellung

#define KEY0    PA3
#define KEY1    PA2
#define KEY2    PA1
#define KEY3    PA0
#define KEY4    PA5

#define REPEAT_MASK 0
#define REPEAT_START 50         // 500ms
#define REPEAT_NEXT 20          // 200ms

void buttons_every_10_ms(void);
uint8_t get_key_press(uint8_t key_mask);
uint8_t get_key_rpt(uint8_t key_mask);

#endif
