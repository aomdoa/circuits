#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <stdint.h>

void set_debug(uint8_t val);
uint8_t get_debug(void);
void dbg_printf(const char *fmt, ...);

#endif
