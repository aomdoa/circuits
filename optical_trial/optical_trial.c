#include "optical_trial.h"

int main() {
    uint8_t counter = 0;
    uint8_t last = 0;
    uart_init();
    twi_init();

    //Disable I2C reset
    DDRC |= (1 << PC2);
    PORTC |= (1 << PC2);
    
    DDRB &= ~(1 << PB0);
    PORTB &= ~(1 << PB0);

    puts("Init");

    if(port_init(PORT_ONE, 0x00) != TWI_SUCCESS) {
        puts("port one init failed");
        return 1;
    }
    if(port_init(PORT_TWO, 0x00) != TWI_SUCCESS) {
        puts("port two init failed");
        return 1;
    }
    if(port_init(PORT_THREE, 0x00) != TWI_SUCCESS) {
        puts("port three init failed");
        return 1;
    }
    if(port_init(PORT_FOUR, 0x00) != TWI_SUCCESS) {
        puts("port four init failed");
        return 1;
    }
    if(mem_read_byte(0x01, &counter) != TWI_SUCCESS) {
        puts("mem read failed");
        return 1;
    }

    printf("Starting: %d\n", counter);
    while(1) {
        if((PINB & (1<<PB0)) == 1) {
            if(last == 0) {
                counter++;
                last = 1;
                printf("ON - counter: %d\n", counter);
                if(mem_write_byte(0x01, counter) != TWI_SUCCESS) {
                    puts("mem write failed");
                    //return 1;
                }
            }
        } else if(last != 0) {
            last = 0;
        }
        port_write(PORT_ONE, counter >> 6);
        port_write(PORT_TWO, counter >> 4);
        port_write(PORT_THREE, counter >> 2);
        port_write(PORT_FOUR, counter);
    }
    return 0;
}
