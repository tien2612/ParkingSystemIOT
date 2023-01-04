#ifndef user_Slot_h
#define user_Slot_h

#include <Arduino.h>
#include <ServoTimer2.h>

#define N0_NODE_CAR   2

#define SLOT_EMPTY     3
#define SLOT_FULL      4
#define SLOT_RESERVED  5

#define UNDEFINED     999
#define TIME_VALID    3000
#define TIME_WAITING  30000

/* Servo */
extern ServoTimer2 servo_s1;
extern ServoTimer2 servo_s2;

extern const int servo_slot1;
extern const int servo_slot2;

struct slot_status {
  float distance;
  int status;
  bool flag_checking_slot;
  bool doneChecking;
  unsigned long int startMillisCar;
  byte UID_reserved[4];
  bool is_slot_reserved;
};

/* Declare car slot infor */
extern slot_status slot_car_status[N0_NODE_CAR];

void open_slot(int slot);

void close_slot(int slot);
#endif