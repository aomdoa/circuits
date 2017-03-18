#include "debug.h"
#include "config.h"
#include "uart.h"
#include <stdarg.h>
#include <stdio.h>

uint8_t debug = 0;
static char debug_message[256];

void set_debug(uint8_t val) {
    debug = val;
}

uint8_t get_debug() {
    return debug;
}

void dbg_printf(const char *fmt, ...) {
    if(debug) {
        va_list ap;
        va_start(ap, fmt);
        vsprintf(debug_message, fmt, ap);
        uart1_puts(debug_message);
        uart1_puts("\r\n");
        va_end(ap);
    }
}

