#include <string.h>
#include <math.h>
#include <stdio.h>
#include "page_misc.h"
#include "ks0108/ks0108.h"
#include "page_graph.h"
#include "ad9910.h"
#include "adc.h"

void drawCoordinateSystem(void){
    for(char counter=0;counter<25;counter++){
        if(counter == 12)
            ks0108DrawLine(7+counter*5,58,7+counter*5,63, BLACK);
        else
            ks0108DrawLine(7+counter*5,60,7+counter*5,63, BLACK);
    }
    ks0108DrawHoriLine(5,60,122,BLACK);
    //ks0108DrawHoriLine(13,20,3,BLACK);
    //ks0108DrawHoriLine(13,40,3,BLACK);
    //ks0108DrawHoriLine(124,40,3,BLACK);
}

void drawMinMaxTemps(void){
    char *buf;
    ks0108GotoXY(100,16);
    buf = intToString(1,0);
    ks0108Puts(buf);
    ks0108Puts(".");
    buf = intToString(1,0);
    ks0108Puts(buf);
    ks0108Puts("C");
    ks0108GotoXY(100,26);
    buf = intToString(1,0);
    ks0108Puts(buf);
    ks0108Puts(".");
    buf = intToString(1,0);
    ks0108Puts(buf);
    ks0108Puts("C");
}

void clearGraph(void){
    ks0108FillRect(0, 20, 127, 43, WHITE);
}

void drawYLegend(int temp_max, int temp_min){
    char *buf;
    ks0108GotoXY(1,47);
    buf = intToString(temp_min,0);
    ks0108Puts(buf);
    ks0108GotoXY(1,19);
    buf = intToString(temp_max,0);
    ks0108Puts(buf);    
}

void draw_graph(){
    int y1,y2;
    int temp_max,temp_min;
    uint16_t power_val;
    
    //clearGraph();
    drawCoordinateSystem();
    //drawMinMaxTemps();
    temp_max = 10;
    temp_min = 0;

    for(uint8_t i=0;i<128;i++){
        dds_set_single_tone_frequency(10,(i*2+1)*1e6);
        power_val = getPDValue(PD_REFLECT) - 60;
        ks0108FillRect(i, 20, 1, 39, WHITE);
        ks0108SetDot(i,60-power_val/2,BLACK);
    }
    
//    for(uint8_t counter=0;counter<graphData.numberOfPoints;counter++){
//        y1 = graphData.temperature_history[counter-1];
//        y2 = graphData.temperature_history[counter];
//        if(counter!=0)
//            ks0108DrawLine(7+counter,60-y1,8+counter,60-y2,BLACK);
//    }
    //drawYLegend(temp_max,temp_min);
}

void page_graph(struct menuitem *self){
    clear_page_main();
    draw_graph();
}

