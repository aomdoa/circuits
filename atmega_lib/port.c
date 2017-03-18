#include "port.h"

uint8_t port_init(uint8_t port, uint8_t io) {
#ifdef DEBUG
    printf("port_init: (0x%02x, 0x%02x)\n", port, io);
#endif
    uint8_t result;
    result = twi_start(port | TWI_WRITE);
    if(result != TWI_SUCCESS) {
        return result;
    }
    result = twi_write(0x00);
    if(result != TWI_SUCCESS) {
        return result;
    }
    result = twi_write(io);
    if(result != TWI_SUCCESS) {
        return result;
    }
    twi_stop();
#ifdef DEBUG
    printf("port_init: done\n");
#endif
    return result;
}

uint8_t port_write(uint8_t port, uint8_t data) {
#ifdef DEBUG
    printf("port_write: (0x%02x, 0x%02x)\n", port, data);
#endif
    uint8_t result;
    result = twi_start(port | TWI_WRITE);
    if(result != TWI_SUCCESS) {
        return result;
    }
    result = twi_write(0x09);
    if(result != TWI_SUCCESS) {
        return result;
    }
    result = twi_write(data);
    if(result != TWI_SUCCESS) {
        return result;
    }
    twi_stop();
#ifdef DEBUG
    printf("port_write: done\n");
#endif
    return result;
}
