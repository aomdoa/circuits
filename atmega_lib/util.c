#include "util.h"

void delayms(uint16_t millis) {
    while(millis) {
        _delay_ms(1);
        millis--;
    }
}
