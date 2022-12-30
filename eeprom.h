
#ifndef eeprom_h
#define eeprom_h

#include "Arduino.h"
#include "user_RFID.h"
#include <EEPROM.h>

extern int address_number_of_users;
extern int address_color_of_slot[2];

void eepromRead(uint16_t addr, int* output, uint16_t length);

void eepromWrite(uint16_t addr, int* input, uint16_t length);

void store_new_UID_into_EEPROM(int *UID);

int check_if_new_id(int *UID);
#endif