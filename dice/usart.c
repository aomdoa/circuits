#include "usart.h"

FILE usart_output = FDEV_SETUP_STREAM(usart_putc, NULL, _FDEV_SETUP_WRITE);
FILE usart_input  = FDEV_SETUP_STREAM(NULL, usart_getc, _FDEV_SETUP_READ);
FILE usart_io     = FDEV_SETUP_STREAM(usart_putc, usart_getc, _FDEV_SETUP_RW);

void usart_init() {
  stdout =& usart_output;
  stdin =& usart_input;

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

uint8_t usart_putc(char c, FILE *stream) {
  if(c == '\n')
    usart_putc('\r', stream);
  loop_until_bit_is_set(UCSRA, UDRE);
  UDR = c;
  return 0;
}

char usart_getc(FILE *stream) {
  loop_until_bit_is_set(UCSRA, RXC);
  return UDR;
}
