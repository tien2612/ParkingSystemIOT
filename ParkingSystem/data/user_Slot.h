#ifndef user_Slot_h
#define user_Slot_h

#include "Arduino.h"


#define N0_NODE_CAR   2

#define SLOT_EMPTY     3
#define SLOT_FULL      4
#define SLOT_REVERSE   5
#define UNDEFINED     999
#define TIME_VALID    1000

struct slot_status {
  float distance;
  int status;
  bool flag_checking_slot;
  bool doneChecking = false;
  unsigned long int startMillisCar;
};

#endif