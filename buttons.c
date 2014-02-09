#include <avr/io.h>
#include <avr/interrupt.h>
#include "buttons.h"

//code taken from http://www.mikrocontroller.net/articles/Entprellung
//
static volatile uint8_t key_state;
static volatile uint8_t key_press;
static volatile uint8_t key_rpt;

void buttons_every_10_ms()
{
    static uint8_t ct0, ct1, rpt;
    uint8_t i;

    i = key_state ^ ~((PINA & (1 << KEY0)) | (PINA & (1 << KEY1)) |
                      (PINA & ((1 << KEY2) | (1 << KEY3) | (1 << KEY4))));
    ct0 = ~(ct0 & i);
    ct1 = ct0 ^ (ct1 & i);
    i &= ct0 & ct1;
    key_state ^= i;
    key_press |= key_state & i;

    if ((key_state & REPEAT_MASK) == 0) {
        rpt = REPEAT_START;
    }
    if (--rpt == 0) {
        rpt = REPEAT_NEXT;
        key_rpt |= key_state & REPEAT_MASK;
    }
}

///////////////////////////////////////////////////////////////////
//
// check if a key has been pressed. Each pressed key is reported
// only once
//
uint8_t get_key_press(uint8_t key_mask)
{
    cli();                      // read and clear atomic !
    key_mask &= key_press;      // read key(s)
    key_press ^= key_mask;      // clear key(s)
    sei();
    return key_mask;
}

///////////////////////////////////////////////////////////////////
//
// check if a key has been pressed long enough such that the
// key repeat functionality kicks in. After a small setup delay
// the key is reported beeing pressed in subsequent calls
// to this function. This simulates the user repeatedly
// pressing and releasing the key.
//
uint8_t get_key_rpt(uint8_t key_mask)
{
    cli();                      // read and clear atomic !
    key_mask &= key_rpt;        // read key(s)
    key_rpt ^= key_mask;        // clear key(s)
    sei();
    return key_mask;
}
