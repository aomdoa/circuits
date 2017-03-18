#define F_CPU 8000000
#define BAUD 9600

#include <avr/io.h>
#include <util/setbaud.h>
#include <util/delay.h>
#include <stdio.h>

void uart_init() {
  UBRRH = UBRRH_VALUE;
  UBRRL = UBRRL_VALUE;
#if USE_2X
  UCSRA |= (1 << U2X);
#else
  UCSRA &= ~(1 << U2X);
#endif
  UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);
  UCSRB = (1 << RXEN) | (1 << TXEN);
}

void uart_putc(char c, FILE *stream) {
  if(c == '\n')
    uart_putc('\r', stream);
  loop_until_bit_is_set(UCSRA, UDRE);
  UDR = c;
}

char uart_getc(FILE *stream) {
  loop_until_bit_is_set(UCSRA, RXC);
  return UDR;
}

void delayms(uint16_t millis) {
  while(millis) {
    _delay_ms(1);
    millis--;
  }
}

FILE uart_output = FDEV_SETUP_STREAM(uart_putc, NULL, _FDEV_SETUP_WRITE);
FILE uart_input  = FDEV_SETUP_STREAM(NULL, uart_getc, _FDEV_SETUP_READ);
FILE uart_io     = FDEV_SETUP_STREAM(uart_putc, uart_getc, _FDEV_SETUP_RW);

int main() {
  uart_init();
  stdout =& uart_output;
  stdin =& uart_input;
  DDRB |= 1<<PB0;

  while(1) {
    PORTB &= ~(1<<PB0);
    delayms(500);
    PORTB |= 1<<PB0;
    puts("This is a test\n");
//    delayms(500);
  }

  return 0;
}
