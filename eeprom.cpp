#include "eeprom.h"

int address_number_of_users = 0;

/* Read data from EEPROM */
void eepromRead(uint16_t addr, void* output, uint16_t length) {
  uint8_t* src; 
  uint8_t* dst;
  src = (uint8_t*)addr;
  dst = (uint8_t*)output;
  for (uint16_t i = 0; i < length; i++) {
      *dst++ = eeprom_read_byte(src++);
  }
}

/* Store data into EEPROM */
void eepromWrite(uint16_t addr, int* input, uint16_t length) {
    int index_address = addr;
    for (byte i = 0; i < 4; i++) {       
      EEPROM.write(index_address, input[i]);
      index_address = index_address + 1;
    }
}

/* Store new UID into EEPROM */
void store_new_UID_into_EEPROM(int *UID) {
  int current_user = EEPROM.read(address_number_of_users);
  /* UID array has 4 elements, so we need store 4 bytes into EEPROM. 
  we multiply number of users by 4 and plus 1 to get current index address */
  eepromWrite(current_user * 4 + 1, UID, 4);

  /* Increase number of users by 1 */
  EEPROM.write(address_number_of_users, ++current_user);
}

/* Traverse all UID in EEPROM, start index 1, step = 1 + j * cur_user */
int check_if_new_id(int *UID) {
  int current_user = EEPROM.read(address_number_of_users);

  for (int i = 1, j = 0; j < current_user; i += 4, ++j ) {
    
    if (UID[0] == EEPROM.read(i) && UID[1] == EEPROM.read(i + 1) && UID[2] == EEPROM.read(i + 2)
                                 && UID[3] == EEPROM.read(i + 3) ) return 0;

    
                     
  }

  return 1;
}