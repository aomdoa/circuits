#include "twi.h"

void twi_init() {
    TWSR = 0x00;
    TWBR = ((F_CPU / F_SCL) - 16) / 2;
}

uint8_t twi_start(uint8_t address) {
#ifdef DEBUG
    printf("twi_start: (0x%02x)\n", address);
#endif
    uint8_t status, wait = 0;
    //Send start
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while(!(TWCR & (1 << TWINT))) {
        delayms(1);
        wait++;
        if(wait > 254) {
            printf("ERRROR\n");
            return TWI_ERROR;
        }
    }
    status = TWSR;
    if((status & 0xf8) != TW_START && (status & 0xf8) != TW_REP_START) {
#ifdef DEBUG
        printf("twi_start: error on init, 0x%02x\n", TWSR & 0xf8);
#endif
        return TWI_ERROR;
    }

    //Send address
    TWDR = address;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while(!(TWCR & (1 << TWINT)));
    uint8_t tw_status = TW_STATUS & 0xf8;
    if((tw_status != TW_MT_SLA_ACK) && (tw_status != TW_MR_SLA_ACK)) {
#ifdef DEBUG
        printf("twi_start: error on address\n");
#endif
        return TWI_ERROR;
    }
    return TWI_SUCCESS;
}

void twi_stop() {
#ifdef DEBUG
    printf("twi_stop\n");
#endif
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
    delayms(1);
}

uint8_t twi_write(uint8_t data) {
#ifdef DEBUG
    printf("twi_write: (0x%02x)\n", data);
#endif
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while(!(TWCR & (1 << TWINT)));
    if((TWSR & 0xf8) != TW_MT_DATA_ACK) {
#ifdef DEBUG
        printf("twi_write: failure on send\n");
#endif
        return TWI_ERROR;
    }
    return TWI_SUCCESS;
}

uint8_t twi_write_data(uint8_t *data, uint8_t length) {
    uint8_t i = 0;
    for(;i<length;i++) {
        if(twi_write(data[i]) == TWI_ERROR) {
            return TWI_ERROR;
        }
    }
    return TWI_SUCCESS;
}

uint8_t twi_readAck() {
    TWDR = 0x00;
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
    while(!(TWCR & (1 << TWINT)));
    if((TWSR & 0xf8) != TW_MR_DATA_ACK) {
#ifdef DEBUG
        printf("twi_readAck: failure on read\n");
#endif
        return 0x00;
    }
    return TWDR;
}

uint8_t twi_readNak() {
    TWDR = 0x00;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while(!(TWCR & (1 << TWINT)));
    if((TWSR & 0xf8) != TW_MR_DATA_NACK) {
#ifdef DEBUG
        printf("twi_readNak: failure on read, 0x%02x\n", TWSR & 0xf8);
#endif
        return 0x00;
    }
    return TWDR;
}
