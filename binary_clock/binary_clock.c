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

ISR(TIMER0_OVF_vect) {
    uint8_t display_button = PINC & (1 << PC2);
    uint8_t counter_button = PINC & (1 << PC3);
    uint8_t seconds_button = PINC & (1 << PC4);
    uint8_t minutes_button = PINC & (1 << PC5);
    uint8_t hours_button = PINC & (1 << PC6);
    RtcTime new_time = read_time();

    if(display_button) {
        if(display_pushed == 0) {
            display_pushed = 1;
            if(seconds_button || minutes_button || hours_button) {
                if(seconds_button) {
                    if(debug) {
                        printf("Change seconds colour\n");
                    }
                    current_colour.second++;
                    if(current_colour.second > 7) {
                        current_colour.second = 1;
                    }
                }
                if(minutes_button) {
                    if(debug) {
                        printf("Change minutes colour\n");
                    }
                    current_colour.minute++;
                    if(current_colour.minute > 7) {
                        current_colour.minute = 1;
                    }
                }
                if(hours_button) {
                    if(debug) {
                        printf("Change hours colour\n");
                    }
                    current_colour.hour++;
                    if(current_colour.hour > 7) {
                        current_colour.hour = 1;
                    }
                }
            } else {
                if(debug) {
                    printf("Toggling display\n");
                }
                display_mode++;
                if(display_mode > DISPLAY_CYCLE) {
                    display_mode = DISPLAY_OFF;
                    current_colour.second = 7;
                    current_colour.minute = 7;
                    current_colour.hour = 7;
                }
            }
        }
    } else {
        display_pushed = 0;
    }

    if(counter_button) {
        if(counter_pushed == 0) {
            counter_pushed = 1;
            if(seconds_button) {
                write_second(0);
                new_time.second = 0;
            }
            if(minutes_button) {
                new_time.minute++;
                if(new_time.minute > 59) {
                    new_time.minute = 0;
                }
                if(debug) {
                    printf("Change minutes to %d\n", new_time.minute);
                }
                write_minute(new_time.minute);
            }
            if(hours_button) {
                new_time.hour++;
                if(new_time.hour > 23) {
                    new_time.hour = 0;
                }
                if(debug) {
                    printf("Change hours to %d\n", new_time.hour);
                }
                write_hour(new_time.hour);
            }
        }
    } else {
        counter_pushed = 0;
    }

    if(display_mode == DISPLAY_CYCLE) {
        current_colour.second = current_time.second % 7 + 1;
        current_colour.minute = current_time.minute % 7 + 1;
        current_colour.hour = current_time.hour % 7 + 1;
    } else if(display_mode == DISPLAY_RANDOM) {
        if(current_time.second != new_time.second) {
            current_colour.second = (rand() + current_time.second) % 7 + 1;
            current_colour.minute = (rand() + current_time.minute) % 7 + 1;
            current_colour.hour = (rand() + current_time.hour) % 7 + 1;
        }
    }

    current_time = new_time;
    if(display_mode != DISPLAY_OFF) {
        display_time(current_time);
    } else {
        clear_display();
    }
}

void display_time(RtcTime time) {
    if(debug) {
        printf("Colour: %d - %d - %d\n", current_colour.hour, current_colour.minute, current_colour.second);
    }

    PORTA = ((time.minute << 6) & 0xc0) + time.second;
    if(debug) {
        printf("PB0: %03d, ", PORTA);
    }
    PORTB |= (1 << PB0);
    PORTB &= ~(1 << PB0);
    if(debug) delayms(100);

    PORTA = ((time.hour << 4) & 0xf0) + (time.minute >> 2);
    if(debug) {
        printf("PB1: %03d, ", PORTA);
    }
    PORTB |= (1 << PB1);
    PORTB &= ~(1 << PB1);
    if(debug) delayms(100);

    PORTA = (0xfe ^ ((current_colour.hour << 7) + (current_colour.minute << 4) + (current_colour.second << 1))) + (time.hour >> 4);
    if(debug) {
        printf("PB2: %03d, ", PORTA);
    }
    PORTB |= (1 << PB2);
    PORTB &= ~(1 << PB2);
    if(debug) delayms(100);

    PORTA = 0xff ^ ((current_colour.hour >> 1));
    if(debug) {
        printf("PB3: %03d, ", PORTA);
    }
    PORTB |= (1 << PB3);
    PORTB &= ~(1 << PB3);
    if(debug) delayms(100);
}

void clear_display() {
    PORTA = 0x00;
    PORTB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3);
    PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3));
}

void print_time(RtcTime print) {
    printf("%02d:%02d:%02d\n", print.hour, print.minute, print.second);
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
    twi_stop();
    if(debug) {
        printf("RTC: ");
        print_time(time);
    }
    return time;
}

void write_second(uint8_t second) {
    printf("Broken, not setting\n");
    /*twi_start(RTC | TWI_WRITE);
    twi_write(0x02);
    twi_write((i2bcd(second) & 0x7f) + 128);
    twi_stop();*/
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
 *  1 - SECOND-R
 *  2 - SECOND-G
 *  3 - SECOND-B
 *  4 - MINUTE-R
 *  5 - MINUTE-G
 *  6 - MINUTE-B
 *  7 - HOUR-R

 * PB3 - PORTA
 *  0 - HOUR-G
 *  1 - HOUR-B
 *
 * PC2 - Input for display
 * PC3 - Input for counter
 * PC4 - Input for seconds
 * PC5 - Input for minutes
 * PC6 - Input for hours
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

    /**
     * PORTA is output for latches
     * PB0, PB1, PB2, PB3 is output to latches
     * PC2 for input
     */
    DDRA = 0xff;
    DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3);
    DDRC &= ~((1 << PC2) | (1 << PC3) | (1 << PC4) | (1 << PC5) | (1<< PC6));

    //Turn everything on
    PORTB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3);
    PORTA = 0xff;
    delayms(1000);
    PORTA = 0x00;
    PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3));


    printf("Start\n");
    initialize_clock();

    // Rising edge for ISR0 - this is interrupt for read/display
//    EICRA |= (1 << ISC01);// | (1 << ISC00);
//    EIMSK |= (1 << INT0);

    // Timer 0 interrupt
    TCCR0A = 0x00;
    TCCR0B = (1<<CS00) | (1<<CS02);
    TIMSK0 = 1<<TOIE0;

    //Clear everything
    PORTB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3);
    PORTA = 0x00;
    PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3));

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
            printf("\tC = set colour (SMH) (1=R, 2=G, 4=B)\n");
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
        } else if(command == 'C') {
            printf("Enter colour (SMH): ");
            echo_input(input, 3);
            printf("\nSetting colour: %s\n", input);
            for(i=0;i<4;i++) {
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
                }
            }
        }
        sei();
    }

    return 0;
}
