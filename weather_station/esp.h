#ifndef __ESP_H__
#define __ESP_H__

#include "config.h"
#include "uart.h"
#include <string.h>
#include <stdio.h>
#include <avr/interrupt.h>

#define ESP_SUCCESS 0x00
#define ESP_NORESPONSE 0x01
#define ESP_ERROR 0x02
#define ESP_ALREADY_CONNECTED 0x03
#define ESP_SEND_ERROR 0x04
#define ESP_IP_ERROR 0x05

/**
 * Initialize the uart, then esp. connect, send and close
 * int main() {
 *     uint8_t connected;
 *     uart0_init(UART_BAUD_SELECT(BAUD, F_CPU));
 *     sei();
 *    esp_init();
 * 
 *     while(1) {
 *         connected = esp_connect("192.168.0.151", 9000);
 *         if(connected == ESP_SUCCESS) {
 *             delayms(1000);
 *             esp_senddata("Hello World!\n");
 *             esp_close();
 *         } else if(connected == ESP_ALREADY_CONNECTED) {
 *             esp_close();
 *         }
 *         delayms(2000);
 *     }
 *     return 0;
 * }
 */

uint8_t esp_gets(char *gets_buffer, uint8_t wait);
uint8_t esp_connect(const char *ip_address, uint16_t port);
uint8_t esp_senddata(const char *data);
uint8_t esp_close(void);
uint8_t esp_command(const char *command, char *response);
uint8_t esp_active(void);
uint8_t esp_echooff(void);
void esp_init(void);

#endif
