#include "config.h"
#include "memory.h"
/**
 * Chip memory addressing works by using the upper bits to determine chip
 * XXXXXCCC WWWWWWWWW
 * e.g.
 * 00000000 00111000 - 0x0038 - Chip 0 at address 0x38
 * 00000001 00010010 - 0x0112 - Chip 1 at address 0x12
 *
 */ 
void twi_init() {
  debug_print("twi_init: start\n");
  TWSR = 0x00;
  TWBR = 0xC0;
  TWCR = (1 << TWEN);
  debug_print("twi_init: done\n");
}

//0 is success
//Start two wire communication
uint8_t twi_start() {
  debug_print("twi_start: start\n");
  TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
  while(!(TWCR & (1 << TWINT)));
  return TWI_SUCCESS;
}

//Stop two wire communication
uint8_t twi_stop() {
  debug_print("twi_stop: start\n");
  TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
  return TWI_SUCCESS;
}

//Configure the address point for chip
uint8_t twi_address(uint8_t twi_address) {
  debug_print("twi_address "B2B_FMT": start\n", B2B(twi_address));
  TWDR = twi_address;
  TWCR = (1 << TWINT) | (1 << TWEN);// | (enable_ack << TWEA);
  while(!(TWCR & (1 << TWINT)));
  return TWI_SUCCESS;
}

//Write some data to chip
uint8_t twi_put_data(uint8_t data) {
  debug_print("twi_put_data "B2B_FMT" - %x - %c: start\n", B2B(data), data, data);
  TWDR = data;
  TWCR = (1 << TWINT) | (1 << TWEN);
  while(!(TWCR & (1 << TWINT)));
  //if((TWSR & 0xf8) != MT_DATA_ACK &&
  //   (TWSR & 0xf8) != MT_DATA_NACK)
  //    return TWI_DATA_FAIL;
  return TWI_SUCCESS;
}

//Get some data from the chip
uint8_t twi_get_data(uint8_t *data, uint8_t enable_ack) {
  debug_print("twi_get_data");
  TWCR = (1 << TWINT) | (1 << TWEN) | (enable_ack << TWEA);
  while(!(TWCR & (1 << TWINT)));
  *data = TWDR;
  debug_print(":"B2B_FMT" - %x - %c\n", B2B(*data), *data, *data);
  return TWI_SUCCESS;
}

//Write the address to the correct chip, optionally enable write
uint8_t mem_address(uint16_t address, uint8_t write) {
  uint8_t chip = 0x0e & (address >> 7);
  debug_print("mem_address chip:"B2B_FMT"\n", B2B(chip));
  if(twi_start()) return 0;
  if(twi_address(0xa0 | chip | (~write & 0x01))) return 0;
  if(write)
    if(twi_put_data(0xff & address)) return 0;
  return 1;
}

//Write a byte to the specified address
uint8_t mem_write_byte(uint16_t address, uint8_t byte) {
  if(!mem_address(address, 1)) return 0;
  if(twi_put_data(byte)) return 0;
  if(twi_stop()) return 0;
  return 1;
}

//Write continuous stream of bytes of size
uint8_t mem_write_page(uint16_t address, uint8_t *page, uint8_t size) {
  uint8_t i = 0, j, max;
  for(j=0;j<=((size-1) / 16);j++) {
    mem_address(address + j * 16, 1);
    max = (j+1) * 16;
    if(max > size) max = size;

    for(i=j*16;i<max;i++)
      if(twi_put_data(page[i])) return 0;
    if(twi_stop()) return 0;
  }
  return i;
}

//Read a byte from address on chip
uint8_t mem_read_byte(uint16_t address, uint8_t *byte) {
  if(!mem_address(address, 1)) return 0;
  if(!mem_address(address, 0)) return 0;
  if(twi_get_data(byte, 0)) return 0;
  twi_stop();
  return 1;
}

//Read size of bytes starting from address (chip + location)
uint8_t mem_read_page(uint16_t address, uint8_t *page, uint8_t size) {
  debug_print("mem_read_page: start\n");
  uint8_t i;
  debug_print("mem_read_page: write address\n");
  if(!mem_address(address, 1)) return 0;
  debug_print("mem_read_page: start read\n");
  if(!mem_address(address, 0)) return 0;
  debug_print("mem_read_page: get data\n");
  for(i=0;i<(size-1);i++) {
    if(twi_get_data(&page[i], 1)) return i;
  }
  if(twi_get_data(&page[i], 0)) return i;
  twi_stop();
  debug_print("mem_read_page: done\n");
  return i+1;
}

//Write data to address crossing chips if necessary
uint16_t mem_write(uint16_t address, void *data, uint8_t size) {
  int16_t chip_mem = 256 - ((int16_t)(0x00ff & address) + size);
  uint8_t written;

  if(chip_mem < 0) {
    debug_print("Crossing chips: %d\n", chip_mem);
    written = size + chip_mem;
    mem_write_page(address, (uint8_t *)data, written);
    address = (address & 0xf00) + 0x100;
    written = mem_write_page(address, data + written, -chip_mem);
    return address + written;
  } else if(chip_mem == 0) {
    debug_print("Moving to next chip\n");
    mem_write_page(address, (uint8_t *)data, size);
    return (address & 0xf00) + 0x100;
  } else {
    debug_print("Writing size %d to %04x\n", size, address);
    written = mem_write_page(address, (uint8_t *)data, size);
    return address + written;
  }
  return 0;
}

//Read data of size crossing chips if necessary
uint16_t mem_read(uint16_t address, void *data, uint8_t size) {
  int16_t chip_mem = 256 - ((int16_t)(0xff & address) + size);
  debug_print("mem_read - chip_mem: %d\n", chip_mem);
  uint8_t read;
  if(chip_mem < 0) {
    debug_print("mem_read: reading across chips\n");
    read = size + chip_mem;
    mem_read_page(address, (uint8_t *)data, read);
    address = (address & 0xf00) + 0x100;
    mem_read_page(address, (uint8_t *)(data + read), -chip_mem);
  } else {
    mem_read_page(address, (uint8_t *)data, size);
  }
  return size;
}
