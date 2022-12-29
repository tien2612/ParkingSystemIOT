#ifndef user_RFID_h
#define user_RFID_h

#include "Arduino.h"

#define N0_USER_ID  250

struct user_data_ID{
  int ID[4];
};

extern int current_index;

extern user_data_ID user_ID[N0_USER_ID];

int check_id(int* user_id, int* data);

#endif
