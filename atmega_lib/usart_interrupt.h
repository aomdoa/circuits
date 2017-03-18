/**
 * Initialize the usart for use as interrupt
 **/
#ifndef __USARTINTERRUPT_H__
#define __USARTINTERRUPT_H__

#include "config.h"
#include <avr/io.h>
#include <util/setbaud.h>
#include <stdio.h>

/**
 * Initializes USART0 based on the defined BAUD rate in config.h
 **/
void uart_init(void);

#endif
