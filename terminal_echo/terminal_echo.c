#include "terminal_echo.h"
#include <string.h>

int main() {
    char buffer[50], output[50];
    uint8_t counter = 0;
    uart_init();
    DDRC |= (1 << PB0) & (1 << PB1) & (1 << PB2) & (1 << PB3) & (1 << PB4);

    delayms(1000);


    while(1) {
        PORTB = counter;
        if(counter >= 31) {
            counter = 0;
        } else {
            counter++;
        }
        puts("Please enter your string:\n ");
        gets(buffer);
        sprintf(output, "You entered: %s", buffer);
        puts(output);
        delayms(500);
    }
}
