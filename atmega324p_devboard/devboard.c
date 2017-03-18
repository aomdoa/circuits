#include "devboard.h"

int main() {
    uint8_t counter = 0;
    uart_init();
    twi_init();
    //Disable the reset on io chips
    DDRC |= (1 << PC2);
    PORTC |= (1 << PC2);

    puts("Init");
    //Set port io to out
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

    printf("Staring: %d\n", counter);
    while(1) {
        port_write(PORT_ONE, counter >> 6);
        port_write(PORT_TWO, counter >> 4);
        port_write(PORT_THREE, counter >> 2);
        port_write(PORT_FOUR, counter);
        if(counter == 255) {
            puts("Loop");
            counter = 0;
        } else {
            counter++;
        }
        if(mem_write_byte(0x01, counter) != TWI_SUCCESS) {
            puts("mem write failed");
//            return 1;
        }
        delayms(100);
    }

    return 0;
}
