#include "config.h"

#include <avr/interrupt.h>
#include <util/delay.h>

#define LEDFADE 16
#define DELAY 60

void delayms(uint16_t millis) {
  while(millis) {
    _delay_ms(1);
    millis--;
  }
}

int main(void) {
  TCCR2 = (1 << WGM21) | (1 << WGM20) | (1 << COM21) | (1 << CS21);
  sei();

  while(1) {
    DDRB = (1 << PB3);
    delayms(DELAY);
    for(int i=0;i<LEDFADE;i++) {
      OCR2 = i;
      delayms(DELAY);
    }
    for(int i=LEDFADE-1;i>=0;i--) {
      OCR2 = i;
      delayms(DELAY);
    }
    PORTB = 0x00;
    DDRB = 0x00;
    delayms(2000);
   /* OCR1AL = 255;
    delayms(500);
    OCR1AL = 200;
    delayms(500);
    OCR1AL = 150;
    delayms(500);
    OCR1AL = 100;
    delayms(500);
    OCR1AL = 50;
    delayms(500);
    OCR1AL = 0;
    delayms(500);*/
  }
  return 0;
}
