#ifndef user_Slot_h
#define user_Slot_h

#include "Arduino.h"
#include "Servo.h"

#define N0_NODE_CAR 2
#define N0_SLOT_IN_NODE 2

#define SLOT_EMPTY     3
#define SLOT_FULL      4
#define SLOT_REVERSE   5

const int servo_checkin = 5;
extern Servo servo;

void open_barrier();
void close_barrier();


extern int status_slot[N0_NODE_CAR * N0_SLOT_IN_NODE];

#endif