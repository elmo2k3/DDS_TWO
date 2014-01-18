#include <string.h>
#include <stdio.h>
#include "ks0108/ks0108.h"
#include "page_main.h"
#include "page_misc.h"
#include "ad9910.h"
#include "adc.h"

static uint8_t focus_not_here = 1;
static uint8_t state;

uint32_t frequency = 50000000;
int8_t gain = 8;
static uint8_t toggle = 0;

static enum cursor_state{
	STATE_IDLE,
	STATE_FREQUENCY_MHZ,
	STATE_FREQUENCY_KHZ,
	STATE_GAIN
};

void page_main(struct menuitem *self){

	printGain(0);
	printFrequencyMhz();
	printFrequencyKhz();
    update_main(self,0);
	dds_set_single_tone_frequency(10,frequency);
}

void printForwardPower(){
    char str[15];
	static int16_t last_val = -1;
	int16_t current_val;

	current_val = getPDValue(PD_FORWARD);

	if(current_val != last_val){
		ks0108FillRect(60, 16, 20, 10, WHITE);
		sprintf(str,"%d",current_val);
		ks0108SelectFont(1,BLACK);
		ks0108GotoXY(0,17);
		ks0108Puts_P(PSTR("FORWARD"));
		ks0108GotoXY(60,17);
		ks0108Puts(str);
	}
	last_val = current_val;
}

void printReflectPower(){
    char str[15];
	static int16_t last_val = -1;
	int16_t current_val;

	current_val = getPDValue(PD_REFLECT);

	if(current_val != last_val){
		ks0108FillRect(60, 16+10, 20, 10, WHITE);
		sprintf(str,"%d",current_val);
		ks0108SelectFont(1,BLACK);
		ks0108GotoXY(0,17+10);
		ks0108Puts_P(PSTR("REVERSE"));
		ks0108GotoXY(60,17+10);
		ks0108Puts(str);
	}
	last_val = current_val;
}

void printTransmitPower(){
    char str[15];
	static int16_t last_val = -1;
	int16_t current_val;

	current_val = getPDValue(PD_TRANSMISSION);

	if(current_val != last_val){
		ks0108FillRect(60, 16+20, 20, 10, WHITE);
		sprintf(str,"%d",current_val);
		ks0108SelectFont(1,BLACK);
		ks0108GotoXY(0,17+20);
		ks0108Puts_P(PSTR("TRANS"));
		ks0108GotoXY(60,17+20);
		ks0108Puts(str);
	}
	last_val = current_val;
}

void printGain(uint8_t toggle){
    char str[15];
	static uint8_t gain_last = 16;

	if(gain != gain_last){
		sprintf(str,"v = %02d",gain);
		ks0108SelectFont(3,BLACK);
		ks0108FillRect(93, 28, 34, 20, WHITE);
		ks0108GotoXY(85,28);
		ks0108Puts(str);
		gain_last = gain;
	}else if(toggle){
		ks0108FillRect(106, 28, 21, 20, WHITE);
		gain_last = 16;
	}
}

void printFrequencyMhz(uint8_t toggle){
    char str[15];
	uint32_t mhz;
	static uint32_t frequency_last = 500000001;

	if(frequency != frequency_last){
		mhz = frequency / 1000000ul;
		sprintf(str,"f = %03ld",mhz);
		ks0108SelectFont(3,BLACK);
		ks0108FillRect(12, 48, 33, 20, WHITE);
		ks0108GotoXY(12,48);
		ks0108Puts(str);
		frequency_last = frequency;
	}else if(toggle){
		ks0108FillRect(25, 48, 33, 20, WHITE);
		frequency_last = 500000001;
	}
}

void printFrequencyKhz(uint8_t toggle){
    char str[15];
	uint32_t mhz,khz;
	static uint32_t frequency_last = 500000001;

	if(frequency != frequency_last){
		mhz = frequency / 1000000ul;
		khz = (frequency-mhz*1000000ul)/1000ul;
		sprintf(str,".%03ldMHz",khz);
		ks0108SelectFont(3,BLACK);
		ks0108FillRect(60, 48, 66, 20, WHITE);
		ks0108GotoXY(60,48);
		ks0108Puts(str);
		frequency_last = frequency;
	}else if(toggle){
		ks0108FillRect(60, 48, 66, 20, WHITE);
		frequency_last = 500000001;
	}
}

void printBootloader(){
	clear_page_main();
    ks0108SelectFont(3,BLACK);
    ks0108GotoXY(14,46);
    ks0108Puts_P(PSTR("Bootloader.."));
}

void update_main(struct menuitem *self, uint8_t event){

    //clear_page_main();
	
	printForwardPower();
	printReflectPower();
	printTransmitPower();
	switch(state){
		case STATE_FREQUENCY_MHZ:
			printFrequencyMhz(toggle == 0);
			break;
		case STATE_FREQUENCY_KHZ:
			printFrequencyMhz(0);
			printFrequencyKhz(toggle == 0);
			break;
		case STATE_GAIN:
			printFrequencyKhz(0);
			printGain(toggle == 0);
			break;
		default:
			printGain(0);
	}
	if(++toggle == 10)
		toggle = 0;
}

uint8_t main_button_pressed(struct menuitem *self, uint8_t button){
   
   	toggle = 0;
	if(button == 1){
		focus_not_here = 0;
		if(state == STATE_IDLE){
			state = STATE_FREQUENCY_MHZ;
		}else if(state == STATE_FREQUENCY_MHZ){
			state = STATE_FREQUENCY_KHZ;
		}else if(state == STATE_FREQUENCY_KHZ){
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
			printGain(0);
			break;
		case STATE_FREQUENCY_MHZ:
			frequency += (uint32_t)steps*1000ul*1000ul;
			if(frequency > 1000000000) frequency = 0;
			if(frequency > 500000000) frequency = 500000000;
			dds_set_single_tone_frequency(10,frequency);
			printFrequencyMhz(0);
			break;
		case STATE_FREQUENCY_KHZ:
			frequency += (uint32_t)steps*1000ul;
			if(frequency > 1000000000) frequency = 0;
			if(frequency > 500000000) frequency = 500000000;
			dds_set_single_tone_frequency(10,frequency);
			printFrequencyKhz(0);
			break;
	}
}

