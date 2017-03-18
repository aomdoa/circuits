#include "config.h"
#include "usart.h"

#include <stdlib.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <util/delay.h>

typedef struct {
  double adc, resistance, temperature;
} temp_result;

void delayms(uint16_t millis) {
  while(millis) {
    _delay_ms(1);
    millis--;
  }
}

void adc_init(void) {
  debug_print("DEBUG: adc_init\n");
  ADMUX = (1 << REFS0);
  //ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS0);
  ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS0);
}

uint16_t adc_value(uint8_t port) {
  debug_print("DEBUG: adc_value: %d\n", port);
  ADMUX &= ~(1 << MUX0 | 1 << MUX1 | 1 << MUX2 | 1 << MUX3);
  if(port == 1)
    ADMUX |= (1 << MUX0);
  else if(port == 2)
    ADMUX |= (1 << MUX1);
  else if(port == 3)
    ADMUX |= (1 << MUX1) | (1 << MUX0);
  ADCSRA |= (1 << ADIF) | (1 << ADSC);
  while(1) {
    delayms(25);
    debug_print("DEBUG: check adc\n");
    if(ADCSRA & (1 << ADIF)) break;
  }
  debug_print("DEBUG: adc = %d\n", ADC);
  return ADC;
}

double adc_average(uint8_t port) {
  double value = 0;
  uint8_t i;

  for(i=0;i<NUM_SAMPLES;i++) {
    value += (double)adc_value(port);
    debug_print("DEBUG: adc_average sample = %.2f\n", value);
  }
  value = value / NUM_SAMPLES;
  return value;
}

temp_result probe_temp_sensor(uint8_t port) {
  temp_result result;
  result.adc = adc_average(port);

  if(port == 0)
    result.resistance = 17980/(1023/result.adc-1);
  else
    result.resistance = 17830/(1023/result.adc-1);
  result.temperature = (1.0 / ((1.0/(25+273.15)) + log(result.resistance / 10000.0) / 3880.0) - 273.15);
  return result;
}

temp_result board_temp_sensor(uint8_t port) {
  temp_result result;
  if(port == 0)
    result.adc = adc_average(2);
  else
    result.adc = adc_average(3);
  result.resistance = 553/(1023/result.adc-1);
  result.temperature = (1.0 / ((1.0/(25+273.15)) + log(result.resistance / 5000.0) / 3880.0) - 273.15);
  return result;
}

volatile double sample_count = 0;
volatile int loops = 0;

void run_probes(void) {
  //TCCR1B = 0x00;
  temp_result adc_results[4];
  //LED Fader
  DDRB = (1 << PB3);
  delayms(LEDDELAY);
  for(int i=0;i<LEDFADE;i++) {
    OCR2 = i;
    delayms(LEDDELAY);
  }

  //Real work for data
  printf("DATA\r\n");
  debug_print("DEBUG: process is set\n");
  sample_count++;
  adc_results[0] = probe_temp_sensor(0);
  adc_results[1] = probe_temp_sensor(1);
  adc_results[2] = board_temp_sensor(0);
  adc_results[3] = board_temp_sensor(1);
    
  printf("{\"s\":[{\"a\": %.2f,\"r\":%.2f,\"t\":%.2f},{\"a\":%.2f,\"r\":%.2f,\"t\":%.2f},{\"a\":%.2f,\"r\":%.2f,\"t\":%.2f},{\"a\":%.2f,\"r\":%.2f,\"t\":%.2f}],\"sample\":%.0f}\r\n",
      adc_results[0].adc, adc_results[0].resistance, adc_results[0].temperature,
      adc_results[1].adc, adc_results[1].resistance, adc_results[1].temperature,
      adc_results[2].adc, adc_results[2].resistance, adc_results[2].temperature,
      adc_results[3].adc, adc_results[3].resistance, adc_results[3].temperature,
      sample_count);

  //LED Fader
  for(int i=LEDFADE-1;i>=0;i--) {
    OCR2 = i;
    delayms(LEDDELAY);
  }
  PORTB = 0x00;
  DDRB = 0x00;
}

ISR(TIMER1_COMPA_vect) {
  loops++;
  if(loops >= 4) {
    loops = 0;
    run_probes();
    OCR1AH = 0xb7;
    OCR1AL = 0x1a;
    return;
  }
  OCR1AH = 0xf4;
  OCR1AL = 0x23;
};

int main(void) {
  usart_init();
  adc_init();
  //Init the PWM
  TCCR2 |= (1 << WGM21) | (1 << WGM20) | (1 << COM21) | (1 << CS21);
  //First run
  run_probes();
  //Init 16 second timer @ 4Mhz  - 1 run at 12, 3 at 16
  TCCR1A = 0x00;
  TCCR1B = (1 << WGM12);
  TIMSK |= (1 << OCIE1A);
  OCR1AH = 0xb7;
  OCR1AL = 0x1a;
  TCCR1B |= (1 << CS12) | (1 << CS10);
  sei();

  debug_print("DEBUG: finished init\n");
  while(1) {
    sleep_enable();
    sleep_cpu();
  }
  return 0;
}
