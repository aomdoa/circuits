#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <avr/io.h>
#include <stdio.h>

#define TWI_SUCCESS 0
#define TWI_START_FAIL 1
#define TWI_STOP_FAIL 2
#define TWI_ADDRESS_FAIL 3
#define TWI_DATA_FAIL 4

void twi_init(void);
uint8_t twi_start(void);
uint8_t twi_stop(void);
uint8_t twi_address(uint8_t);
uint8_t twi_put_data(uint8_t);
uint8_t twi_get_data(uint8_t *, uint8_t);
uint8_t mem_address(uint16_t, uint8_t);
uint8_t mem_write_byte(uint16_t, uint8_t);
uint8_t mem_write_page(uint16_t, uint8_t *, uint8_t);
uint8_t mem_read_byte(uint16_t, uint8_t *);
uint8_t mem_read_page(uint16_t, uint8_t *, uint8_t); 
uint16_t mem_write(uint16_t, void *, uint8_t);
uint16_t mem_read(uint16_t, void *, uint8_t);

#endif
