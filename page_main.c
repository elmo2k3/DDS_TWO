#include <string.h>
#include <stdio.h>
#include "ks0108/ks0108.h"
#include "page_main.h"
#include "page_misc.h"

uint8_t frequency;

void page_main(struct menuitem *self){
    char tempToPrint[7];

    update_main(self,0);

}

void update_main(struct menuitem *self, uint8_t event){
    uint8_t xOffset=0;
    char str[15];

    sprintf(str,"%d",frequency);

    clear_page_main();
    ks0108SelectFont(3,BLACK);
    ks0108GotoXY(3,30);
    ks0108Puts(str);
//    ks0108Puts_P(PSTR("104.402M"));
    ks0108SelectFont(2,BLACK);

}

uint8_t main_button_pressed(struct menuitem *self, uint8_t button){
    
    return 0;
}

void main_drehgeber(struct menuitem *self, int8_t steps){
    int16_t temp;

    frequency += steps;
    update_main(self,0);
}

