#ifndef user_RFID_h
#define user_RFID_h

#include "Arduino.h"
#include <Servo.h>


#define N0_USER_ID  4

struct user_data_ID{
  int ID[4];
};

// extern int current_index_user_ID;

extern user_data_ID user_ID[N0_USER_ID];

// int check_id(int* user_id, int* data);

// int check_id_all_user_ID(int* data);

// void update_user_ID(int slot, String UID);

#endif
