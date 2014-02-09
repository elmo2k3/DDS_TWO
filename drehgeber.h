#ifndef __DREHGEBER_H__
#define __DREHGEBER_H__

#include <stdint.h>
// inits timer0

#define DREHGEBER_A PINA7
#define DREHGEBER_B PINA6

#define PHASE_B (PINA & 1<<DREHGEBER_B)
#define PHASE_A (PINA & 1<<DREHGEBER_A)

void drehgeber_init(void);
void drehgeber_work(void);      //call every 1ms
int8_t drehgeber_read(void);    // read four step encoders

#endif
