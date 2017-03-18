#ifndef _MEMORY_H_
#define _MEMORY_H_

#include "config.h"
#include "twi.h"

/**
 * Write a single byte to memory at the specifed address
 */
uint8_t mem_write_byte(uint8_t address, uint8_t data);
/**
 * Write data starting at the specified address for the given length
 */
uint8_t mem_write_page(uint8_t address, uint8_t *data, uint8_t length);
/**
 * Read the byte of data from the specified address
 */
uint8_t mem_read_byte(uint8_t address, uint8_t *data);
/**
 * Read data starting at the specified address for the given length
 */
uint8_t mem_read_page(uint8_t address, uint8_t *data, uint8_t length); 

#endif
