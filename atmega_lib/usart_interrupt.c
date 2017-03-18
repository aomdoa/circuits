#include "usart_interrupt.h"


static volatile unsigned char received[64];
static volatile unsigned char tosend[64];
static volatile uint8_t received_tail = 0, received_head = 0, tosend_tail = 0, tosend_head = 0;

ISR(UART0_RECEIVE_INTERRUPT) {
    unsigned char data, status;
    status = (UART0_STATUS & (_BV(FE)|_BV(DOR)));
    data = UDR0;



}

void uart_init() {
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
}


