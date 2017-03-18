#ifndef _CONFIG_H_
#define _CONFIG_H_

#define F_CPU 4000000UL
#define BAUD 9600

#define NUM_SAMPLES 10
#define LEDFADE 16
#define LEDDELAY 60

#define DEBUG 0
#define debug_print(fmt, ...) do { if(DEBUG) printf(fmt, ##__VA_ARGS__); } while (0)
#define B2B_FMT "%d%d%d%d%d%d%d%d"
#define B2B(byte) \
  (byte & 0x80 ? 1 : 0), \
  (byte & 0x40 ? 1 : 0), \
  (byte & 0x20 ? 1 : 0), \
  (byte & 0x10 ? 1 : 0), \
  (byte & 0x08 ? 1 : 0), \
  (byte & 0x04 ? 1 : 0), \
  (byte & 0x02 ? 1 : 0), \
  (byte & 0x01 ? 1 : 0)

#endif
