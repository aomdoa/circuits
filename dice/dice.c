#include "config.h"
#include "usart.h"
#include "memory.h"

#include <stdlib.h>
#include <avr/interrupt.h>
#include <util/delay.h>

struct dice {
  uint8_t count;
  uint8_t value;
  uint8_t roll;
};

//Tracks the current roll
struct dice current_roll;
uint16_t address;

void delayms(uint16_t millis) {
  while(millis) {
    _delay_ms(1);
    millis--;
  }
}

uint8_t dice() {
  uint8_t result = 0;
  for(uint8_t i=0;i<current_roll.count;i++)
    result += (uint8_t)((double)rand() / ((double)RAND_MAX + 1) * current_roll.value) + 1;
  return result;
}

void inc_count() {
  current_roll.count++;
  if(current_roll.count > 5)
    current_roll.count = 1;
  display_count(current_roll.count);
}

void inc_value() {
  if(current_roll.value <= 4)       current_roll.value = 6;
  else if(current_roll.value <= 6)  current_roll.value = 8;
  else if(current_roll.value <= 8)  current_roll.value = 10;
  else if(current_roll.value <= 10) current_roll.value = 12;
  else if(current_roll.value <= 12) current_roll.value = 20;
  else                              current_roll.value = 4;
  display_value(current_roll.value);
}
  
void roll() {
  for(uint8_t i=0;i<10;i++) {
    current_roll.roll = dice();
    display_value(current_roll.roll);
    delayms(100);
  }
  address = mem_write(address, &current_roll, sizeof(current_roll));
  mem_write(UPPER, &address, 2);
}

//Button interrupt
ISR(INT0_vect) {
  uint8_t pin = PIND;
  uint8_t button = 0xe0 & PIND;
  switch(button) {
  case 128:
    //    printf("Roll!!\n");
    roll();
    break;
  case 64:
    //    printf("Count\n");
    inc_count();
    break;
  case 32:
    //    printf("Value\n");
    inc_value();
    break;
  default:
    printf("i have no idea: %d/%d\n", button, pin);
    break;
  }
  return;
}

void display_count(uint8_t count) {
  //Clear port for count
  PORTC &= ~(1 << PC0 | 1 << PC1 | 1 << PC2 | 1 << PC3);
  PORTD &= ~(1 << PD4);
  //Enable count latch
  PORTB |= 1<<PB0;
  if(count == 1)
    PORTC |= 1<<PC0;
  else if(count == 2)
    PORTC |= 1<<PC1;
  else if(count == 3)
    PORTC |= 1<<PC2;
  else if(count == 4)
    PORTC |= 1<<PC3;
  else if(count == 5)
    PORTD |= 1<<PD4;

  PORTB &= ~(1<<PB0);
}

void display_value(uint8_t value) {
  //Clear port
  PORTC &= ~(1 << PC0 | 1 << PC1 | 1 << PC2 | 1 << PC3);
  if(value > 99) value = 99;
  uint8_t upper = (uint8_t)(value / 10);
  uint8_t lower = value - (upper * 10);
  PORTB &= ~(1<<PB1);
  PORTC = upper;
  asm("nop");
  PORTB |= (1<<PB1);
  PORTB &= ~(1<<PB2);
  PORTC = lower;
  asm("nop");
  PORTB |= (1<<PB2);
}

int main() {
  char input;
  uint16_t mem = 0x00;
  usart_init();
  twi_init();

  //Enable INT0 and all interrupts
  MCUCR = (1 << ISC01) | (1 << ISC00);
  GICR |= (1 << INT0);
  sei();

  //Configure PD ports for input 5,6,7 - button value, count and roll
  //PD4 is output for upper count
  DDRD |= (1 << PD4); //(1 << PD5) | (1 << PD6) | (1 << PD7);
  //Configure PC ports for output 0,1,2,3 - BCD + lower 4 count
  DDRC |= (1 << PC0) | (1 << PC1) | (1 << PC2) | (1 << PC3);
  //Configure PB ports for output 0,1,2 - count, seg1, seg2
  DDRB |= (1 << PB0 ) | (1 << PB1) | (1 << PB2);
  PORTB = 0x06;
  PORTC = 0x00;
  PORTD = 0x00;

  //Initialize structure
  current_roll.count = 1;
  current_roll.value = 4;
  display_count(current_roll.count);
  display_value(current_roll.value);

  //Read the first two bytes to determine current write point
  mem_read(UPPER, &address, 2);
  printf("Start address: %04x\n", address);

  while(1) {
    printf("Command (c,v,r,R,C):");
    input = getchar();
    if(input == 'c') inc_count();
    else if(input == 'v') inc_value();
    else if(input == 'r') roll();
    else if(input == 'R') {
      mem = 0x00;
      while(mem < address) {
	mem += mem_read(mem, &current_roll, sizeof(current_roll));
	printf("C=%d,V=%d,R=%d\n", current_roll.count, current_roll.value, current_roll.roll);
	display_count(current_roll.count);
	display_value(current_roll.roll);
      }
    }
    else if(input == 'C') { address = 0x00; mem_write(UPPER, &address, 2); printf("Cleared"); }
    printf("\n");
  }

  return 0;
}
