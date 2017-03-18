#ifndef __PORT_H__
#define __PORT_H__

#include "config.h"
#include "twi.h"

/**
 * Initialize port with specified input/output
 */
uint8_t port_init(uint8_t port, uint8_t io);
/**
 * Write data to the specified port
 */
uint8_t port_write(uint8_t port, uint8_t data);

#endif
