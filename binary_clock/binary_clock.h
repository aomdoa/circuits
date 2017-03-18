#ifndef __BINARY_CLOCK_H__
#define __BINARY_CLOCK_H__

#include <stdint.h>

#define RTC 0xa2
#define RED 0x01
#define GREEN 0x02
#define BLUE 0x04


/**
 * When set to 1 turns debug messaging on on the serial port
 */
volatile uint8_t debug = 0;
typedef struct {
    uint8_t second; //integer
    uint8_t minute; //integer
    uint8_t hour;   //integer
    uint8_t day;    //integer
    uint8_t month;   //integer
    uint8_t year;    //integer
} RtcTime;

typedef struct {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint8_t year;
} Colour;

/**
 * Start TWI and inialize the heartbeat from RTC
 */
void initialize_clock(void);
/**
 * Read full date and time from RTC
 */
RtcTime read_time(void);
/**
 * Display the passed time on the binary clock
 */
void display_time(RtcTime);
/**
 * Clear the binary clock display
 */
void clear_display(void);
/**
 * Print the provided time to serial
 */
void print_time(RtcTime);
/**
 * Write the appropriate time to RTC
 */
void write_second(uint8_t);
void write_minute(uint8_t);
void write_hour(uint8_t);
void write_month(uint8_t);
void write_year(uint8_t);
/**
 * Receive and display characters through serial
 * char * input string
 * uint8_t maximum number of characters
 */
void echo_input(char *, uint8_t);
/**
 * i2bcd - convert integer to bcd
 */
uint8_t i2bcd(uint8_t);
uint8_t bcd2i(uint8_t);
int main(void);

#endif
