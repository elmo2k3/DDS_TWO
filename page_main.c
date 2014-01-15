#include <string.h>
#include <stdio.h>
#include "ks0108/ks0108.h"
#include "page_main.h"
#include "page_misc.h"
#include "ad9910.h"
#include "adc.h"

static uint8_t focus_not_here = 1;
static uint8_t state;
static uint8_t toggle;

uint32_t frequency = 1000000;
int8_t gain = 8;

static enum cursor_state{
	STATE_IDLE,
	STATE_FREQUENCY,
	STATE_GAIN
};

void page_main(struct menuitem *self){
    char tempToPrint[7];

    update_main(self,0);
	dds_set_single_tone_frequency(10,frequency);
}

void update_main(struct menuitem *self, uint8_t event){
    uint8_t xOffset=0;
    char str[15];
	uint32_t mhz,khz;

	mhz = frequency / 1000000ul;
	khz = (frequency-mhz*1000000ul)/1000ul;

    clear_page_main();
	
	sprintf(str,"%d",getPDValue(PD_FORWARD));
    ks0108SelectFont(1,BLACK);
    ks0108GotoXY(0,17);
	ks0108Puts_P(PSTR("FORWARD"));
    ks0108GotoXY(60,17);
    ks0108Puts(str);
	
	sprintf(str,"%d",getPDValue(PD_REFLECT));
    ks0108SelectFont(1,BLACK);
    ks0108GotoXY(0,17+10);
	ks0108Puts_P(PSTR("REVERSE"));
    ks0108GotoXY(60,17+10);
    ks0108Puts(str);
	
	sprintf(str,"%d",getPDValue(PD_TRANSMISSION));
    ks0108SelectFont(1,BLACK);
    ks0108GotoXY(0,17+20);
	ks0108Puts_P(PSTR("TRANS"));
    ks0108GotoXY(60,17+20);
    ks0108Puts(str);
    
	sprintf(str,"v = %d",gain);
    ks0108SelectFont(3,BLACK);
    ks0108GotoXY(95,28);
    ks0108Puts(str);

    sprintf(str,"f = %03ld.%03ldMHz",mhz,khz);
    ks0108SelectFont(3,BLACK);
    ks0108GotoXY(18,46);
    ks0108Puts(str);

    ks0108SelectFont(2,BLACK);

}

uint8_t main_button_pressed(struct menuitem *self, uint8_t button){
   
	if(button == 1){
		focus_not_here = 0;
		if(state == STATE_IDLE){
			state = STATE_FREQUENCY;
		}else if(state == STATE_FREQUENCY){
			state = STATE_GAIN;
		}else if(state == STATE_GAIN){
			state = STATE_IDLE;
		}
	}else if(button == 2){
		update_main(self,0);
	}else if(button == 4){
		focus_not_here ^= 1;
	}

    return focus_not_here;
}

void main_drehgeber(struct menuitem *self, int8_t steps){

	switch(state){
		case STATE_GAIN:
			gain += steps;
			if(gain < 0) gain = 0;
			if(gain > 15) gain = 15;
			pga_set_gain(gain);
			break;
		case STATE_FREQUENCY:
			frequency += (uint32_t)steps*1000ul*1000ul;
			if(frequency > 1000000000) frequency = 0;
			if(frequency > 500000000) frequency = 500000000;
			dds_set_single_tone_frequency(10,frequency);
			break;
	}
    update_main(self,0);
}

