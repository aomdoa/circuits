#include "memory.h"

uint8_t mem_write_byte(uint8_t address, uint8_t data) {
#ifdef DEBUG
    printf("mem_write_byte: (0x%02x, 0x%02x)\n", address, data);
#endif
    uint8_t result;
    result = twi_start(PORT_MEMORY | TWI_WRITE);
    if(result != TWI_SUCCESS) {
#ifdef DEBUG
        printf("mem_write_byte: twi_start failed\n");
#endif
        return result;
    }
    result = twi_write(address);
    if(result != TWI_SUCCESS) {
#ifdef DEBUG
        printf("mem_write_byte: twi_write address failed\n");
#endif
        return result;
    }
    result = twi_write(data);
    if(result != TWI_SUCCESS) {
#ifdef DEBUG
        printf("mem_write_byte: twi_write data failed\n");
#endif
        return result;
    }
    twi_stop();
#ifdef DEBUG
    printf("mem_write_byte: success\n");
#endif
    return result;
}

uint8_t mem_write_page(uint8_t address, uint8_t *data, uint8_t size) {
    uint8_t result, i;
    result = twi_start(PORT_MEMORY | TWI_WRITE);
    if(result != TWI_SUCCESS) {
        return result;
    }
    result = twi_write(address);
    if(result != TWI_SUCCESS) {
        return result;
    }
    for(i=0;i<size;i++) {
        result = twi_write(data[i]);
        if(result != TWI_SUCCESS) {
            return result;
        }
    }
    twi_stop();
    return result;
}

uint8_t mem_read_byte(uint8_t address, uint8_t *data) {
#ifdef DEBUG
    printf("mem_read_byte: (0x%02x, ...)\n", address);
#endif
    uint8_t result;
    result = twi_start(PORT_MEMORY | TWI_WRITE);
    if(result != TWI_SUCCESS) {
#ifdef DEBUG
        printf("mem_read_byte: twi_start failed\n");
#endif
        return result;
    }
    result = twi_write(address);
    if(result != TWI_SUCCESS) {
#ifdef DEBUG
        printf("mem_read_byte: twi_write address failed\n");
#endif
        return result;
    }
    //twi_stop();
    result = twi_start(PORT_MEMORY | TWI_READ);
    if(result != TWI_SUCCESS) {
#ifdef DEBUG
        printf("mem_read_byte: twi_start read failed\n");
#endif
        return result;
    }
    *data = twi_readNak();
    twi_stop();
#ifdef DEBUG
    printf("mem_read_byte: data = 0x%02x\n", *data);
#endif
    return result;
}

uint8_t mem_read_page(uint8_t address, uint8_t *data, uint8_t size) {
    uint8_t result, i;
    result = twi_start(PORT_MEMORY | TWI_WRITE);
    if(result != TWI_SUCCESS) {
        return result;
    }
    result = twi_write(address);
    if(result != TWI_SUCCESS) {
        return result;
    }
    delayms(1);
    result = twi_start(PORT_MEMORY | TWI_READ);
    if(result != TWI_SUCCESS) {
        return result;
    }
    for(i=0;i<(size-1);i++) {
        data[i] = twi_readAck();
    }
    data[size-1] = twi_readNak();
    return result;
}
