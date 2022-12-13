#include "eeprom.h"

unsigned int address_user_ID = 40;
unsigned int address_slot_ID = 80;

void eepromRead(uint16_t addr, void* output, uint16_t length) {
  uint8_t* src; 
  uint8_t* dst;
  src = (uint8_t*)addr;
  dst = (uint8_t*)output;
  for (uint16_t i = 0; i < length; i++) {
      *dst++ = eeprom_read_byte(src++);
  }
}

void eepromWrite(uint16_t addr, void* input, uint16_t length) {
    uint8_t* src; 
    uint8_t* dst;
    src = (uint8_t*)input;
    dst = (uint8_t*)addr;
    for (uint16_t i = 0; i < length; i++) {
        eeprom_write_byte(dst++, *src++);
    }
}