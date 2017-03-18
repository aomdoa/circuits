#include "usart.h"

static FILE uart_output = FDEV_SETUP_STREAM(uart_putc, NULL, _FDEV_SETUP_WRITE);
static FILE uart_input  = FDEV_SETUP_STREAM(NULL, uart_getc, _FDEV_SETUP_READ);
static FILE uart_io     = FDEV_SETUP_STREAM(uart_putc, uart_getc, _FDEV_SETUP_RW);

void uart_init() {
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    stdout =& uart_output;
    stdin =& uart_input;
}

void uart_putc(char c, FILE *stream) {
    if(c == '\n') {
        uart_putc('\r', stream);
    }
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = c;
}

char uart_getc(FILE *stream) {
    loop_until_bit_is_set(UCSR0A, RXC0);
    return UDR0;
}

