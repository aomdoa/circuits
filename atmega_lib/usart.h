/**
 * Initialize the usart and prepare to work with printf et. al.
 **/
#ifndef __DEVBOARD_H__
#define __DEVBOARD_H__

#include "config.h"
#include <avr/io.h>
#include <util/setbaud.h>
#include <stdio.h>

/**
 * Initializes USART0 based on the defined BAUD rate in config.h
 **/
void uart_init(void);
/**
 * Place a character on the stream - used with FDEV_SETUP
 **/
void uart_putc(char, FILE*);
/**
 * Get a character from the stream - used with FDEV_SETUP
 **/
char uart_getc(FILE*);

#endif
