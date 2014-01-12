#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdint.h>
#include <avr/pgmspace.h>

#define CHANNEL         1           // Sende/Empfangskanal (0-3) (nur gültig wenn kein DIP Schalter verwendet wird)
#define RF_BAUDRATE     20000       // Baudrate des RFM12 (nur gültig wenn kein DIP Schalter verwendet wird)
#define UART_BAUDRATE   19200       // Baudrate des UARTs (nur gültig wenn kein DIP Schalter verwendet wird)

#define MY_ADDRESS 0x30

#define KEY_INPUT (PINB & (1<<PB1))

/* Info welches Paket kommt */
#define GP_PACKET 0 // General purpose packet
#define GRAPH_PACKET 1 // Packet with first part of graph
#define GRAPH_PACKET_PART2 2 // Dirty hack
#define MPD_PACKET 3
#define RGB_PACKET 4

#define MPD_PLAYING 1
#define MPD_RANDOM 2

#define SEC 0
#define MINUTE 1
#define HOUR 2
#define DAY 3
#define MONTH 4
#define YEAR 5

#define NEW_RF_DATA 6

//#define HARDWARE_V1

extern volatile uint8_t refreshFlags;
extern int16_t time_off;
extern int16_t time_on;
extern uint32_t uptime;

struct menuitem
{
    PGM_P name;
    uint8_t num;
    void (*refresh_func)(struct menuitem *self, uint8_t event);
    void (*drehgeber_func)(struct menuitem *self, int8_t steps);
    uint8_t (*taster_func)(struct menuitem *self, uint8_t taster);
    void (*draw_func)(struct menuitem *self);
    uint8_t rf_package_update_type;
    uint8_t sensor;
    uint8_t module;
    uint8_t idle_timeout;
};

struct __attribute__((packed)) glcdMainPacket
{
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t day;
    uint8_t month;
    uint8_t year;
    uint8_t weekday;
    int16_t temperature[4];
    uint8_t backlight; 
    uint8_t wecker;
    uint8_t hr20_celsius_is;
    uint8_t hr20_decicelsius_is;
    uint8_t hr20_celsius_set;
    uint8_t hr20_decicelsius_set;
    uint8_t hr20_valve;
    uint8_t hr20_mode;
    uint8_t hr20_auto_t[4];
    uint8_t hr20_auto_t_deci[4];
    uint8_t sun_rise_hour;
    uint8_t sun_rise_minute;
    uint8_t sun_set_hour;
    uint8_t sun_set_minute;
}glcdMainPacket;

struct __attribute__((packed)) glcdMpdPacket
{
    char title[20];
    char artist[20];
    char album[20];
    uint16_t length;
    uint16_t pos;
    uint8_t status;
}glcdMpdPacket;

struct graphPacket
{
    uint8_t numberOfPoints;
    int8_t max[2];
    int8_t min[2];
    uint8_t temperature_history[115]; // Array fuer Aussentemperaur Diagramm
}graphData;

#endif

