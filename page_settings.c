#include <string.h>
#include <avr/eeprom.h>
#include <math.h>
#include <stdio.h>

#include "DDS_TWO.h"
#include "page_misc.h"
#include "ks0108/ks0108.h"
#include "page_settings.h"

static uint8_t state;
static uint8_t backlight_eemem EEMEM;
static uint8_t backlight_night_eemem EEMEM;
static uint8_t backlight;
static uint8_t backlight_night;
static uint16_t time_off_eemem EEMEM;
static uint16_t time_on_eemem EEMEM;

#define ACTIVE_STATE 1
#define BL_STATE 2
#define BL_NIGHT_STATE 3
#define BL_ON_STATE 4
#define BL_OFF_STATE 5

void load_settings(){

    backlight = eeprom_read_byte(&backlight_eemem);
    backlight_night = eeprom_read_byte(&backlight_night_eemem);
    time_off = eeprom_read_word(&time_off_eemem);
    time_on = eeprom_read_word(&time_on_eemem);
    if(time_off > 24*60 || time_off < 0) time_off = 23*60;
    if(time_on > 24*60 || time_on < 0) time_on = 6*60;

    OCR1A = backlight;
}

void save_settings(){
    eeprom_write_byte(&backlight_eemem, backlight);
    eeprom_write_byte(&backlight_night_eemem, backlight_night);
    eeprom_write_word(&time_off_eemem, time_off);
    eeprom_write_word(&time_on_eemem, time_on);
}

void draw_backlight(){
    char *buf;
    buf = intToString((int)((float)backlight/2.55),0);
    draw_settings_line(PSTR("Beleuchtung ein:"), buf, 0);
}

void draw_backlight_night(){
    char *buf;
    buf = intToString((int)((float)backlight_night/2.55),0);
    draw_settings_line(PSTR("Beleuchtung aus:"), buf, 1);
}

void draw_backlight_on_time(){
    uint16_t hours, minutes;
    char buf[7];

    hours = time_on / 60;
    minutes = time_on - hours*60;

    strcpy(buf,intToString(hours,1));
    strcat(buf,":");
    strcat(buf,intToString(minutes,1));

    draw_settings_line(PSTR("Uhrzeit ein:"), buf, 2);
}

void draw_backlight_off_time(){
    uint16_t hours, minutes;
    char buf[7];

    hours = time_off / 60;
    minutes = time_off - hours*60;

    strcpy(buf,intToString(hours,1));
    strcat(buf,":");
    strcat(buf,intToString(minutes,1));

    draw_settings_line(PSTR("Uhrzeit aus:"), buf, 3);
}

void page_settings(struct menuitem *self){
    clear_page_main();
    draw_backlight();
    draw_backlight_night();
    draw_backlight_on_time();
    draw_backlight_off_time();
}

void update_settings(struct menuitem *self, uint8_t event){
}

uint8_t settings_button_pressed(struct menuitem *self, uint8_t button){
#ifndef ONE_BUTTON_ONLY 
    if(button == 4){ //rotary button
        if(state != 0){
            state = 0;
            save_settings();

            ks0108FillRect(90,17,5,10,WHITE);
            ks0108FillRect(90,28,5,10,WHITE);
            ks0108FillRect(90,39,5,10,WHITE);
            ks0108FillRect(90,50,5,10,WHITE);

            return 1;
        }else{
            state = BL_STATE;
            draw_arrow(90,20);
            return 0;
        }
    }else if(state && (button == 2 || button == 3)){
#else
    {
#endif
        if(state == 0){
            state = BL_STATE;
            draw_arrow(90,20);
        }else if(state == BL_STATE){
            state = BL_NIGHT_STATE;
            ks0108FillRect(90,17,5,10,WHITE);
            draw_arrow(90,31);
        }else if(state == BL_NIGHT_STATE){
            state = BL_ON_STATE;
            ks0108FillRect(90,28,5,10,WHITE);
            draw_arrow(90,42);
        }else if(state == BL_ON_STATE){
            state = BL_OFF_STATE;
            ks0108FillRect(90,39,5,10,WHITE);
            draw_arrow(90,53);
        }else if(state == BL_OFF_STATE){
            state = BL_STATE;
            ks0108FillRect(90,50,5,10,WHITE);
#ifndef ONE_BUTTON_ONLY 
            draw_arrow(90,17);
#else
            state = 0;
            save_settings();
            return 1;
#endif
        }
        return 0;
    }
}

void settings_drehgeber(struct menuitem *self, int8_t steps){
    int16_t temp;
    if(state == BL_STATE){
        temp = backlight + steps*4;
        if(temp > 255) temp = 255;
        if(temp < 0) temp = 0;
        OCR1A = temp;
        draw_backlight();
    }else if(state == BL_NIGHT_STATE){
        temp = backlight_night + steps*4;
        if(temp > 255) temp = 255;
        if(temp < 0) temp = 0;
        if(temp == 4) temp = 1;
        if(temp == 5) temp = 4;
        OCR1A = temp;
        backlight_night = temp;
        draw_backlight_night();
    }else if(state == BL_ON_STATE){
        time_on += steps;
        if(time_on > 24*60) time_on = 0;
        if(time_on < 0) time_on = 24*60;
        draw_backlight_on_time();
    }else if(state == BL_OFF_STATE){
        time_off += steps;
        if(time_off > 24*60) time_off = 0;
        if(time_off < 0) time_off = 24*60;
        draw_backlight_off_time();
    }
}

