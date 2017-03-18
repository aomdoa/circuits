#ifndef _DUSART_H_
#define _DUSART_H_

#include "config.h"
#include <avr/io.h>
#include <util/setbaud.h>
#include <stdio.h>

void usart_init(void);
uint8_t usart_putc(char, FILE *);
char usart_getc(FILE *);

#endif
