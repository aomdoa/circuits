#include "esp.h"
#include "../atmega_lib/util.h"

#define DELAY 100

static char command_buffer[128];
static char response_buffer[128];

/**
 * Fill the provided buffer - 1 means buffer filled and 0 means no data
 */
uint8_t esp_gets(char *gets_buffer, uint8_t wait) {
    uint8_t index = 0, loop = 0;
    uint16_t data;
    do {
        data = uart0_getc();
        if(data == UART_NO_DATA) {
            loop++;
            delayms(1);
        } else {
            if((char)data == '\r') {
                continue;
            } else if((char)data == '\n') {
                if(index > 0 && gets_buffer[index-1] != ' ') {
                    data = ' ';
                } else {
                    continue;
                }
            }
            loop = 0;
            gets_buffer[index] = (char)data;
            index++;
        }
    } while(loop < wait);
    if(index > 0 && gets_buffer[index-1] == ' ') {
        gets_buffer[index-1] = '\0';
        index--;
    } else {
        gets_buffer[index] = '\0';
    }
    //DEBUG
    //uart0_puts(gets_buffer);
    //uart0_puts("\r\n");
    //delayms(DELAY);
    //DEBUG
    uart0_flush();
    return index;
}

uint8_t esp_connect(const char *ip_address, uint16_t port) {
    sprintf(command_buffer, "AT+CIPSTART=\"TCP\",\"%s\",%d\r\n", ip_address, port);
    if(esp_command(command_buffer, response_buffer) > 0) {
        if(strcmp(response_buffer, "CONNECT OK") == 0 || strcmp(response_buffer, "ALREAY CONNECT") == 0) {
            return ESP_SUCCESS;
        } else if(strcmp(response_buffer, "ALREAY CONNECT") == 0) {
            return ESP_ALREADY_CONNECTED;
        } else if(strcmp(response_buffer, "IP ERROR") == 0) {
            return ESP_IP_ERROR;
        } else {
            return ESP_ERROR;
        }
    }
    return ESP_NORESPONSE;
}

uint8_t esp_senddata(const char *data) {
    sprintf(command_buffer, "AT+CIPSEND=%d\r\n", strlen(data));
    if(esp_command(command_buffer, response_buffer) == 1) {
        //No data is good for this
        if(esp_command(data, response_buffer) > 0) {
            if(strcmp(response_buffer, "SEND OK") == 0) {
                return ESP_SUCCESS;
            }
        }
        return ESP_SEND_ERROR;
    }
    return ESP_ERROR;
}

uint8_t esp_close() {
    if(esp_command("AT+CIPCLOSE\r\n", response_buffer) > 0) {
        if(strcmp(response_buffer, "CLOSED OK") == 0) {
            return ESP_SUCCESS;
        }
    } else {
        return ESP_NORESPONSE;
    }
    return ESP_ERROR;
}

uint8_t esp_command(const char *command, char *result) {
    uart0_flush();
    uart0_puts(command);
    delayms(DELAY);
    return esp_gets(result, DELAY);
}

uint8_t esp_active() {
    if(esp_command("AT\r\n", response_buffer) > 0) {
        if(strcmp(response_buffer, "AT OK") == 0 || strcmp(response_buffer, "OK") == 0) {
            return ESP_SUCCESS;
        }
    } else {
        return ESP_NORESPONSE;
    }
    return ESP_ERROR;
}

uint8_t esp_echooff() {
    if(esp_command("ATE0\r\n", response_buffer) > 0) {
        if(strcmp(response_buffer, "OK") == 0) {
            return ESP_SUCCESS;
        }
    } else {
        return ESP_NORESPONSE;
    }
    return ESP_ERROR;
}

void esp_init() {
    do {
        delayms(500);
        if(esp_active() == ESP_SUCCESS) {
            if(esp_echooff() == ESP_SUCCESS) {
                break;
            }
        }
    } while(1);
}
