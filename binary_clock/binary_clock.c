#include "config.h"
#include "binary_clock.h"
#include "../atmega_lib/usart.h"
#include "../atmega_lib/util.h"
#include "../atmega_lib/twi.h"
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <string.h>

RtcTime current_time;
Colour current_colour;

ISR(INT0_vect) {
    if(debug) {
        printf("Clock interrupt\n");
    }
    current_time = read_time();
    display_time(current_time);
    return;
}

void display_time(RtcTime time) {
    PORTA = ((time.minute << 6) & 0xc0) + time.second;
    if(debug) {
        printf("PB0: %03d, ", PORTA);
    }
    PORTB |= (1 << PB0);
    PORTB &= ~(1 << PB0);
    if(debug) delayms(500);

    PORTA = ((time.hour << 4) & 0xf0) + (time.minute >> 2);
    if(debug) {
        printf("PB1: %03d, ", PORTA);
    }
    PORTB |= (1 << PB1);
    PORTB &= ~(1 << PB1);
    if(debug) delayms(500);

    PORTA = ((time.month << 6) & 0xc0) + ((time.day << 1) & 0x3e) + (time.hour >> 4);
    if(debug) {
        printf("PB2: %03d, ", PORTA);
    }
    PORTB |= (1 << PB2);
    PORTB &= ~(1 << PB2);
    if(debug) delayms(500);

    PORTA = ((time.year << 2) & 0xfc) + (time.month >> 2);
    if(debug) {
        printf("PB3: %03d, ", PORTA);
    }
    PORTB |= (1 << PB3);
    PORTB &= ~(1 << PB3);
    if(debug) delayms(500);

    PORTA = (0xfe ^ ((current_colour.hour << 7) + (current_colour.minute << 4) + (current_colour.second << 1))) + (time.year >> 6);
    if(debug) {
        printf("PB4: %03d, ", PORTA);
    }
    PORTB |= (1 << PB4);
    PORTB &= ~(1 << PB4);
    if(debug) delayms(500);

    PORTA = 0xff ^ ((current_colour.month << 5) + (current_colour.day << 2) + (current_colour.hour >> 1));
    if(debug) {
        printf("PD4: %03d, ", PORTA);
    }
    PORTC |= (1 << PC6);
    PORTC &= ~(1 << PC6);
    if(debug) delayms(500);

    PORTA = ~(current_colour.year) & 0x07;
    if(debug) {
        printf("PD5: %03d\n", PORTA);
    }
    PORTC |= (1 << PC7);
    PORTC &= ~(1 << PC7);
    if(debug) delayms(500);

}

void clear_display() {
    PORTA = 0x00;
    PORTB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3) | (1 << PB4);
    PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3) | (1 << PB4));
}

void print_time(RtcTime print) {
    printf("%02d-%02d-%02d %02d:%02d:%02d\n", print.year, print.month, print.day, print.hour, print.minute, print.second);
}

RtcTime read_time() {
    RtcTime time;
    twi_start(RTC | TWI_WRITE);
    twi_write(0x02);
    twi_stop();
    twi_start(RTC | TWI_READ);
    time.second = bcd2i(twi_readAck() & 0x7f);
    time.minute = bcd2i(twi_readAck() & 0x7f);
    time.hour = bcd2i(twi_readAck() & 0x3f);
    time.day = bcd2i(twi_readAck() & 0x3f);
    twi_readAck(); //weekday - don't need
    time.month = bcd2i(twi_readAck() & 0x1f);
    time.year = bcd2i(twi_readAck());
    twi_stop();
    if(debug) {
        printf("RTC: ");
        print_time(time);
    }
    return time;
}

void write_second(uint8_t second) {
    twi_start(RTC | TWI_WRITE);
    twi_write(0x02);
    twi_write(i2bcd(second) & 0x7f);
    twi_stop();
    return;
}

void write_minute(uint8_t minute) {
    twi_start(RTC | TWI_WRITE);
    twi_write(0x03);
    twi_write(i2bcd(minute) & 0x7f);
    twi_stop();
    return;
}

void write_hour(uint8_t hour) {
    twi_start(RTC | TWI_WRITE);
    twi_write(0x04);
    twi_write(i2bcd(hour) & 0x3f);
    twi_stop();
    return;
}

void write_day(uint8_t day) {
    twi_start(RTC | TWI_WRITE);
    twi_write(0x05);
    twi_write(i2bcd(day) & 0x3f);
    twi_stop();
    return;
}

void write_month(uint8_t month) {
    twi_start(RTC | TWI_WRITE);
    twi_write(0x07);
    twi_write(i2bcd(month) & 0x1f);
    twi_stop();
    return;
}

void write_year(uint8_t year) {
    twi_start(RTC | TWI_WRITE);
    twi_write(0x08);
    twi_write(i2bcd(year));
    twi_stop();
    return;
}

void initialize_clock() {
    uint8_t commands[2] = { 0x0d, 0x83 };
    twi_init();
    twi_start(RTC | TWI_WRITE);
    twi_write_data(commands, 2);
    twi_stop();
    printf("Clock initialized\n");
}

void echo_input(char *input, uint8_t length) {
    uint8_t index = 0;
    char character;

    while(index < length) {
        character = getchar();
        if(character == '\n' || character == '\r') {
            break;
        }
        putchar(character);
        input[index] = character;
        index++;
    }
    input[index] = '\0';
}

uint8_t i2bcd(uint8_t i) {
    return ((i / 10) << 4) | (i % 10);
}

/**
 * PORT LAYOUT
 *
 * PBO - PORTA
 *  0 - SECOND-1
 *  1 - SECOND-2
 *  2 - SECOND-4
 *  3 - SECOND-8
 *  4 - SECOND-16
 *  5 - SECOND-32
 *  6 - MINUTE-1
 *  7 - MINUTE-2
 *
 * PB1 - PORTA
 *  0 - MINUTE-4
 *  1 - MINUTE-8
 *  2 - MINUTE-16
 *  3 - MINUTE-32
 *  4 - HOUR-1
 *  5 - HOUR-2
 *  6 - HOUR-4
 *  7 - HOUR-8
 * 
 * PB2 - PORTA
 *  0 - HOUR-16
 *  1 - DAY-1
 *  2 - DAY-2
 *  3 - DAY-4
 *  4 - DAY-8
 *  5 - DAY-16
 *  6 - MONTH-1
 *  7 - MONTH-2
 *
 * PB3 - PORTA
 *  0 - MONTH-4
 *  1 - MONTH-8
 *  2 - YEAR-1
 *  3 - YEAR-2
 *  4 - YEAR-4
 *  5 - YEAR-8
 *  6 - YEAR-16
 *  7 - YEAR-32
 *
 * PB4 - PORTA
 *  0 - YEAR-64
 *  1 - SECOND-R
 *  2 - SECOND-G
 *  3 - SECOND-B
 *  4 - MINUTE-R
 *  5 - MINUTE-G
 *  6 - MINUTE-B
 *  7 - HOUR-R
 *
 * PC6 - PORTA
 *  0 - HOUR-G
 *  1 - HOUR-B
 *  2 - DAY-R
 *  3 - DAY-G
 *  4 - DAY-B
 *  5 - MONTH-R
 *  6 - MONTH-G
 *  7 - MONTH-B
 *
 * PC7 - PORTA
 *  0 - YEAR-R
 *  1 - YEAR-G
 *  2 - YEAR-B
 */

uint8_t bcd2i(uint8_t bcd) {
    return ((bcd >> 4) * 10) + (bcd & 0x0f);
}

int main() {
    char command, input[16];
    uint8_t value, i;
    uart_init();
    printf("Waiting for settle\n");
    current_colour.second = 7;
    current_colour.minute = 7;
    current_colour.hour = 7;
    current_colour.day = 7;
    current_colour.month = 7;
    current_colour.year = 7;

    /**
     * PORTA is output for latches
     * PB0,PB1, PB2, PB3, PB4, PC6, PC7 is output to latches
     */
    DDRA = 0xff;
    DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3) | (1 << PB4);
    DDRC |= (1 << PC6) | (1 << PC7);

    //Turn everything on
    PORTB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3) | (1 << PB4);
    PORTC |= (1 << PC6) | (1 << PC7);
    PORTA = 0xff;
    delayms(1000);
    PORTA = 0x00;
    PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3) | (1 << PB4));
    PORTC &= ~((1 << PC6) | (1 << PC7));


    printf("Start\n");
    initialize_clock();

    // Rising edge for ISR0 - this is interrupt for read/display
    EICRA |= (1 << ISC01);// | (1 << ISC00);
    EIMSK |= (1 << INT0);

    //Clear everything
    PORTB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3) | (1 << PB4);
    PORTC |= (1 << PC6) | (1 << PC7);
    PORTA = 0x00;
    PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3) | (1 << PB4));
    PORTC &= ~((1 << PC6) | (1 << PC7));

    sei();
    while(1) {
        command = getchar();
        cli();
        if(command == 'd' && debug > 0) {
            printf("Turning debug off\n");
            debug = 0;
        } else if(command == 'd' && debug == 0) {
            printf("Turning debug on\n");
            debug = 1;
        } else if(command == 'h') {
            printf("Commands are:\n");
            printf("\td = debug toggle\n");
            printf("\tS = set second\n");
            printf("\tM = set minute\n");
            printf("\tH = set hour\n");
            printf("\tD = set day\n");
            printf("\tO = set month\n");
            printf("\tY = set year\n");
            printf("\tC = set colour (SMHDMY) (1=R, 2=G, 4=B)\n");
        } else if(command == 'S') {
            printf("Enter second: ");
            echo_input(input, 3);
            value = atoi(input);
            if(value < 0 || value > 59) {
                printf("\nValue %s must be between 0 and 60.\n", input);
            } else {
                printf("\nWill set second to: %d\n", value);
                write_second(value);
                delayms(50);
            }
        } else if(command == 'M') {
            printf("Enter minute: ");
            echo_input(input, 3);
            value = atoi(input);
            if(value < 0 || value > 59) {
                printf("\nValue %s must be between 0 and 60.\n", input);
            } else {
                printf("\nWil set minute to: %d\n", value);
                write_minute(value);
                delayms(50);
            }

        } else if(command == 'H') {
            printf("Enter hour: ");
            echo_input(input, 3);
            value = atoi(input);
            if(value < 0 || value > 23) {
                printf("\nValue %s must be between 0 and 24.\n", input);
            } else {
                printf("\nWill set hour to %d\n", value);
                write_hour(value);
                delayms(50);
            }
        } else if(command == 'D') {
            printf("Enter day: ");
            echo_input(input, 3);
            value = atoi(input);
            if(value < 1 || value > 31) {
                printf("\nValue %s must be between 1 and 31.\n", input);
            } else {
                printf("\nWill set day to %d\n", value);
                write_day(value);
                delayms(50);
            }
        } else if(command == 'O') {
            printf("Enter month: ");
            echo_input(input, 3);
            value = atoi(input);
            if(value < 1 || value > 12) {
                printf("\nValue %s must be between 1 and 12.\n", input);
            } else {
                printf("\nWill set month to %d\n", value);
                write_month(value);
                delayms(50);
            }
        } else if(command == 'Y') {
            printf("Enter year: ");
            echo_input(input, 3);
            value = atoi(input);
            if(value < 1 || value > 99) {
                printf("\nValue %s must be between 1 and 99.\n", input);
            } else {
                printf("\nWill set year to %d\n", value);
                write_year(value);
                delayms(50);
            }
        } else if(command == 'C') {
            printf("Enter colour (SMHDMY): ");
            echo_input(input, 6);
            printf("\nSetting colour: %s\n", input);
            for(i=0;i<7;i++) {
                if(input[i] == '\0') {
                    break;
                }
                if(i == 0) {
                    current_colour.second = (input[i] - '0') & 0x07;
                    if(debug) {
                        printf("\nsecond=%d\n", current_colour.second);
                    }
                } else if(i == 1) {
                    current_colour.minute = (input[i] - '0') & 0x07;
                    if(debug) {
                        printf("\nminute=%d\n", current_colour.minute);
                    }
                } else if(i == 2) {
                    current_colour.hour = (input[i] - '0') & 0x07;
                    if(debug) {
                        printf("\nhour=%d\n", current_colour.hour);
                    }
                } else if(i == 3) {
                    current_colour.day = (input[i] - '0') & 0x07;
                    if(debug) {
                        printf("\nday=%d\n", current_colour.day);
                    }
                } else if(i == 4) {
                    current_colour.month = (input[i] - '0') & 0x07;
                    if(debug) {
                        printf("\nmonth=%d\n", current_colour.month);
                    }
                } else if(i == 5) {
                    current_colour.year = (input[i] - '0') & 0x07;
                    if(debug) {
                        printf("\nyear=%d\n", current_colour.year);
                    }
                }
            }
        }
        sei();
    }

    return 0;
}
