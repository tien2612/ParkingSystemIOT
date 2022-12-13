#ifndef eeprom_h
#define eeprom_h

#include "Arduino.h"

extern unsigned int address_user_ID;
extern unsigned int address_slot_ID;

void eepromRead(uint16_t addr, void* output, uint16_t length);

void eepromWrite(uint16_t addr, void* input, uint16_t length);

#endif