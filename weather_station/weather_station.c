#include "weather_station.h"
void adc_init() {
    ADMUX = (1<<REFS0);
    ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
}

uint16_t adc_read(uint8_t channel) {
    channel &= 0x07;
    ADMUX = (ADMUX & 0xF8) | channel;
    ADCSRA |= (1<<ADSC);
    while(ADCSRA & (1<<ADSC));
    return ADC;
}

int main() {
    uint8_t connected;
    uint16_t power_sample;

    DDRD = (1<<DDD4);
    uart0_init(UART_BAUD_SELECT(BAUD, F_CPU));
    if(bit_is_set(PIND, PD4)) {
        uart1_init(UART_BAUD_SELECT(BAUD, F_CPU));
        set_debug(1);
    }
    sei();
    esp_init();
    adc_init();

    while(1) {
        power_sample = adc_read(0);
        dbg_printf("Read power value: %d", power_sample);
        connected = esp_connect(HOST, PORT);
        if(connected == ESP_SUCCESS) {
            dbg_printf("Connected to %s:%d", HOST, PORT);
            delayms(1000);
            esp_senddata("Hello World!\n");
            dbg_printf("Sent hello world");
            esp_close();
        } else if(connected == ESP_ALREADY_CONNECTED) {
            dbg_printf("Already connected");
            esp_close();
        } else  {
            dbg_printf("Connect failed");
        }
        delayms(2000);
    }
    return 0;
}
