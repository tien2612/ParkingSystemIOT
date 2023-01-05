#ifndef user_RFID_h
#define user_RFID_h

#include "Arduino.h"
#include <SPI.h>
#include <MFRC522.h>

// 4 car slot
#define N0_USER_ID      4   

#define RST_PIN         9
#define SS_1_PIN        10

struct user_data_ID {
  int ID1[4];
};


int check_id(int *arr, int *data);

#endif
