/**
 * The base functions required to get i2c working from the atmega
 * Based on: http://www.avrfreaks.net/comment/842342#comment-842342
 **/
#ifndef _TWI_H_
#define _TWI_H_

#include "config.h"
#include "util.h"
#include <avr/io.h>
#include <util/twi.h>
#include <stdio.h>

#define TWI_READ 0x01
#define TWI_WRITE 0x00
#define TWI_ERROR 1
#define TWI_SUCCESS 0

/**
 * Initialize thw TWI
 */
void twi_init(void);
/**
 * Start TWI with the specified chip. Or with the READ/WRITE to initalize 
 * in the appropriate mode
 */
uint8_t twi_start(uint8_t);
/**
 * Stop communication with the chip
 */
void twi_stop(void);
/**
 * Write a single data packet to chip. ERROR or SUCCESS returned
 */
uint8_t twi_write(uint8_t);
/**
 * Write an array of data elements to the chip. If success is returned everything
 * was written without error.
 * List of data and length
 */
uint8_t twi_write_data(uint8_t *, uint8_t);
/**
 * Read with ACK
 */
uint8_t twi_readAck(void);
/**
 * Read with Nack
 */
uint8_t twi_readNak(void);

#endif
