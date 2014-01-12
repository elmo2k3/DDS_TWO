#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/wdt.h>
#include <string.h>
#include <stdio.h>

#include "drehgeber.h"
#include "clock.h"
#include "ks0108/ks0108.h"
#include "page_main.h"
#include "page_main2.h"
#include "page_graph.h"
#include "page_hr20.h"
#include "page_hr20_auto.h"
#include "page_timer.h"
#include "page_timer2.h"
#include "main.h"
#include "page_misc.h"
#include "page_settings.h"
#include "page_mpd.h"
#include "rf_communication.h"
#include "../rfm12lib/rf12.h"
#include "buttons.h"

volatile uint8_t refreshFlags;
int16_t time_off;
int16_t time_on;
uint8_t backlight_timer;
uint8_t idle_timer;
uint32_t uptime;


//// Bochum
//#define NUM_PAGES 14
//static struct menuitem menu[] = {
//    {NULL,0, update_main, NULL, main_button_pressed, page_main, NEW_MAIN_DATA,0,0,0},
//    {NULL,1, update_timer, timer_drehgeber, timer_button_pressed, page_timer, 0,0,0,120},
//    {NULL,2, update_timer2, timer2_drehgeber, timer2_button_pressed, page_timer2, 0,0,0,120},
//    {NULL,3, update_graph, NULL, NULL, page_graph, NEW_GRAPH_DATA,3,11,0}, // Bochum Temp
//    {NULL,4, update_graph, NULL, NULL, page_graph, NEW_GRAPH_DATA,3,12,0}, // Bochum Feuchte
//    {NULL,5, update_graph, NULL, NULL, page_graph, NEW_GRAPH_DATA,5,11,0}, // Bochum Taupunkt
//    {NULL,6, update_graph, NULL, NULL, page_graph, NEW_GRAPH_DATA,3,5,0}, // Lavesum
//    {NULL,7, update_graph, NULL, NULL, page_graph, NEW_GRAPH_DATA,3,8,0}, // Warmsen
//    {NULL,8, update_graph, NULL, NULL, page_graph, NEW_GRAPH_DATA,3,3,0}, // Innen Temp
//    {NULL,9, update_graph, NULL, NULL, page_graph, NEW_GRAPH_DATA,3,4,0}, // Innen Feuchte
//    {NULL,10, update_graph, NULL, NULL, page_graph, NEW_GRAPH_DATA,5,3,0}, // Innen Taupunkt
//    {NULL,11, update_graph, NULL, NULL, page_graph, NEW_GRAPH_DATA,3,2,0}, // Innen Abstell
//    {NULL,12, update_graph, NULL, NULL, page_graph, NEW_GRAPH_DATA,3,6,0}, // Innen Kuehlschrank
//    {NULL,13, update_settings, settings_drehgeber, settings_button_pressed, page_settings, 0,0,0,30}
//};
// Lavesum
#define NUM_PAGES 15
static struct menuitem menu[] = {
    {NULL,0, update_main, NULL, main_button_pressed, page_main, NEW_MAIN_DATA,0,0},
    {NULL,1, update_timer, timer_drehgeber, timer_button_pressed, page_timer, 0,0,0},
    {NULL,2, update_timer2, timer2_drehgeber, timer2_button_pressed, page_timer2, 0,0,0},
    {NULL,3, update_graph, NULL, NULL, page_graph, NEW_GRAPH_DATA,3,11}, // Bochum Temp
    {NULL,4, update_graph, NULL, NULL, page_graph, NEW_GRAPH_DATA,3,12}, // Bochum Feuchte
    {NULL,5, update_graph, NULL, NULL, page_graph, NEW_GRAPH_DATA,5,11}, // Bochum Taupunkt
    {NULL,6, update_graph, NULL, NULL, page_graph, NEW_GRAPH_DATA,3,5}, // Lavesum Ost
    {NULL,7, update_graph, NULL, NULL, page_graph, NEW_GRAPH_DATA,3,13}, // Temp Lavesum
    {NULL,8, update_graph, NULL, NULL, page_graph, NEW_GRAPH_DATA,3,14}, // Feuchte Lavesum
    {NULL,9, update_graph, NULL, NULL, page_graph, NEW_GRAPH_DATA,5,13}, // Taupunkt Lavesum
    {NULL,10, update_graph, NULL, NULL, page_graph, NEW_GRAPH_DATA,3,8}, // Warmsen
    {NULL,11, update_graph, NULL, NULL, page_graph, NEW_GRAPH_DATA,3,9}, // Wohn Temp Lavesum
    {NULL,12, update_graph, NULL, NULL, page_graph, NEW_GRAPH_DATA,3,10}, // Wohn Feuchte Lavesum
    {NULL,13, update_graph, NULL, NULL, page_graph, NEW_GRAPH_DATA,5,9}, // Wohn Taupunkt Lavesum
    {NULL,14, update_settings, settings_drehgeber, settings_button_pressed, page_settings, 0,0,0}
};

//// RE
//#define NUM_PAGES 8
//static struct menuitem menu[] = {
//    {NULL,0, update_main, NULL, main_button_pressed, page_main, NEW_MAIN_DATA,0,0},
//    {NULL,1, update_timer, timer_drehgeber, timer_button_pressed, page_timer, 0,0,0},
//    {NULL,2, update_timer2, timer2_drehgeber, timer2_button_pressed, page_timer2, 0,0,0},
//    {NULL,3, update_graph, NULL, NULL, page_graph, NEW_GRAPH_DATA,0,0}, // Aussen
//    {NULL,4, update_graph, NULL, NULL, page_graph, NEW_GRAPH_DATA,0,2}, // Feuchte
//    {NULL,5, update_graph, NULL, NULL, page_graph, NEW_GRAPH_DATA,0,1}, // Innen 1
//    {NULL,6, update_graph, NULL, NULL, page_graph, NEW_GRAPH_DATA,0,3}, // Innen 2
//    {NULL,7, update_settings, settings_drehgeber, settings_button_pressed, page_settings, 0,0,0}
//};

//static const unsigned NUM_PAGES = sizeof(menu) / sizeof(menu[0])-1;

void ports_init(void){

    //PORTA: PA5        Pushbutton SW4
    //       PA6        Pushbutton SW3
    //       PA7        Pushbutton SW2
    DDRA &= ~((1<<PA5) | (1<<PA6) | (1<<PA7));
    PORTA |= ((1<<PA5) | (1<<PA6) | (1<<PA7));

    //PORTB: PB0        rotary phase B
    //       PB1        Pushbutton of rotary encoder
    //       PB4-PB7    SPI for RFM12
    DDRB &= ~((1<<PB0)|(1<<PB1));
    PORTB |= (1<<PB0)|(1<<PB1); // Pullup
    
    //PORTD: PD0        Pushbutton SW5
    //       PD2        INT0 input of RFM12
    //       PD3        rotary encoder phase A
    //       PD5        backlight output of lcd (OCR1A)
    //       PD6        reset pin of lcd
    //       PD7        beeper
    DDRD = (1<<PD5) | (1<<PD6) | (1<<PD7);
    PORTD |= (1<<PD0) | (1<<PD3) | (1<<PD6) | (1<<PD7);
    
    //PORTC: data output for lcd
    
    memset(&glcdMainPacket,0,sizeof(glcdMainPacket));
    memset(&glcdMpdPacket,0,sizeof(glcdMpdPacket));
    memset(&graphData,0,sizeof(graphData));

    //timer settings for backlight pwm
    TCCR1A = (1<<WGM10) | (1<<COM1A1);
    TCCR1B = (1<<CS10);
}

uint8_t second_timer(struct menuitem *self){

    uint8_t return_to_main_page = 0;
    uint8_t timer1_active, timer2_active;

    uptime++;
    clock();
    if(backlight_timer){
        backlight_timer--;
        if(backlight_timer == 0)
            check_backlight();
    }
    
    timer1_active = page_timer_decrement();
    timer2_active = page_timer2_decrement();
    if(!timer1_active && !timer2_active) // timers are not active
    {
        if(idle_timer)
        {
            if(--idle_timer == 0) // jump to main page
            {
                return_to_main_page = 1;
            }
        }
    }

    if(self->refresh_func)
        self->refresh_func(self, SEC);

    return return_to_main_page;
}

void minute_timer(struct menuitem *self){
    if(!backlight_timer)
        check_backlight();
    rf_request_time_packet();
    if(self->refresh_func)
        self->refresh_func(self, MINUTE);
}

void hour_timer(struct menuitem *self){
    if(self->refresh_func)
        self->refresh_func(self, HOUR);
}


int main(void){

    int8_t drehgeber_delta = 0;
    int8_t menu_position = 0;
    int8_t old_menu_position = 0;
    uint8_t focus_here = 1;
    uint8_t i;
  
// lavesum
//    menu[0].name = PSTR("Home Automation");
////    menu[1].name = PSTR("Thermostat");
////    menu[2].name = PSTR("Thermostat 2");
//    menu[1].name = PSTR("Timer");
//    menu[2].name = PSTR("Aussen Bochum");
//    menu[3].name = PSTR("Aussen OE");
//    menu[4].name = PSTR("Innen OE");
////    menu[7].name = PSTR("Heizung ist");
////    menu[8].name = PSTR("Heizung soll");
////    menu[9].name = PSTR("Vorlauf");
////    menu[10].name = PSTR("Ruecklauf");
//    menu[5].name = PSTR("Einstellungen");
// Bochum
    i=0;
//    menu[i++].name = PSTR("Home Automation");
//    menu[i++].name = PSTR("Timer");
//    menu[i++].name = PSTR("Timer 2");
//    menu[i++].name = PSTR("Bochum Temp");
//    menu[i++].name = PSTR("Bochum Feuchte");
//    menu[i++].name = PSTR("Bochum Taupunkt");
//    menu[i++].name = PSTR("Lavesum");
//    menu[i++].name = PSTR("Warmsen");
//    menu[i++].name = PSTR("Innen Temp");
//    menu[i++].name = PSTR("Innen Feuchte");
//    menu[i++].name = PSTR("Innen Taupunkt");
//    menu[i++].name = PSTR("Innen Abstell");
//    menu[i++].name = PSTR("Kuehlschrank");
//    menu[i++].name = PSTR("Einstellungen");
//    i=0;
    menu[i++].name = PSTR("Home Automation");
    menu[i++].name = PSTR("Timer");
    menu[i++].name = PSTR("Timer 2");
    menu[i++].name = PSTR("Bochum Temp");
    menu[i++].name = PSTR("Bochum Feuchte");
    menu[i++].name = PSTR("Bochum Taupunkt");
    menu[i++].name = PSTR("Lavesum Ost");
    menu[i++].name = PSTR("Lavesum");
    menu[i++].name = PSTR("Lavesum Feuchte");
    menu[i++].name = PSTR("Lavesum Taupunkt");
    menu[i++].name = PSTR("Warmsen");
    menu[i++].name = PSTR("Wohn Temp");
    menu[i++].name = PSTR("Wohn Feuchte");
    menu[i++].name = PSTR("Wohn Taupunkt");
    menu[i++].name = PSTR("Einstellungen");
////RE
//    i=0;
//    menu[i++].name = PSTR("Home Automation");
//    menu[i++].name = PSTR("Timer");
//    menu[i++].name = PSTR("Timer 2");
//    menu[i++].name = PSTR("Aussen");
//    menu[i++].name = PSTR("Aussen Feuchte");
//    menu[i++].name = PSTR("Arbeitszimmer");
//    menu[i++].name = PSTR("Schlafzimmer");
//    menu[i++].name = PSTR("Einstellungen");

    wdt_enable(WDTO_2S);
    ports_init();
    ks0108Init();
    drehgeber_init();
    rf12_init(1);
    rf12_config(RF_BAUDRATE, CHANNEL, 0, QUIET);
    load_settings();

    draw_page_header(&menu[menu_position]);
    rf_request_time_packet();
    sei();

    while(1){
        wdt_reset();
        if(refreshFlags & (1<<SEC)){
            if(second_timer(&menu[menu_position]) == 1) // return to main page because of idle timeout
            {
                menu_position = 0;
                old_menu_position = 0;
                menu[menu_position].draw_func(&menu[menu_position]);
                draw_page_header(&menu[menu_position]);
            }
            refreshFlags &= ~(1<<SEC);
        }
        if(refreshFlags & (1<<MINUTE)){
            minute_timer(&menu[menu_position]);
            refreshFlags &= ~(1<<MINUTE);
        }
        if(refreshFlags & (1<<HOUR)){
            hour_timer(&menu[menu_position]);
            refreshFlags &= ~(1<<HOUR);
        }
        if(refreshFlags & (1<<DAY)){
//            minute_timer(&menu[menu_position]);
            refreshFlags &= ~(1<<DAY);
        }

        if((drehgeber_delta = drehgeber_read())){ // Drehgeber in action
            backlight_on();
            backlight_timer = 10;

            if(focus_here){
                menu_position += drehgeber_delta;
                if(menu_position > (NUM_PAGES-1)) menu_position = NUM_PAGES-1;
                if(menu_position < 0) menu_position = 0;

                if(menu_position != old_menu_position)
                {
                    idle_timer = menu[menu_position].idle_timeout;
                    if(menu[menu_position].draw_func){
                        menu[menu_position].draw_func(&menu[menu_position]);
                    }
                    draw_page_header(&menu[menu_position]);
                    old_menu_position = menu_position;
                }
            }else{
                if(menu[menu_position].drehgeber_func){
                    menu[menu_position].drehgeber_func(&menu[menu_position], drehgeber_delta);
                }
            }
        }
        if(get_key_press(1<<KEY2)){ //button top right
            if(menu[menu_position].taster_func){
                menu[menu_position].taster_func(&menu[menu_position],1);
            }
        }
        if(get_key_press(1<<KEY3)){ //button top left
            if(menu[menu_position].taster_func){
                menu[menu_position].taster_func(&menu[menu_position],0);
            }
        }
        if(get_key_press(1<<KEY4)){ //button buttom left
            if(menu[menu_position].taster_func){
                menu[menu_position].taster_func(&menu[menu_position],2);
            }
        }
        if(get_key_press(1<<KEY0)){ //button bottom right
            if(menu[menu_position].taster_func){
                menu[menu_position].taster_func(&menu[menu_position],3);
            }
        }
        if(get_key_press(1<<KEY1)){ //button of rotary encoder
            if(menu[menu_position].taster_func){
                focus_here = menu[menu_position].taster_func(&menu[menu_position],4);
            }
        }
        if(rf_new_data(menu[menu_position].rf_package_update_type)){
            if(menu[menu_position].refresh_func)
                menu[menu_position].refresh_func(&menu[menu_position], NEW_RF_DATA);
        }
        rx_handler();
    }
}

ISR(SIG_OUTPUT_COMPARE0) // 1ms
{
    cli();
    static uint16_t prescaler = 1000;
    
    drehgeber_work();
    rf12_every_1_ms();

    if(--prescaler == 0){
        refreshFlags |= (1<<SEC);
        prescaler = 1000;
    }else if(!(prescaler % 10)){ // 10ms
        buttons_every_10_ms();
        rf_every_10ms();
    }
    sei();
}
