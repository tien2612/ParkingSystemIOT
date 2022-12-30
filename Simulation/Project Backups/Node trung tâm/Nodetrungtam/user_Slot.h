#ifndef user_Slot_h
#define user_Slot_h



#include "Arduino.h"
#include "Servo.h"

const int servo_checkin = 5;
extern Servo servo;

void open_barrier();
void close_barrier();


#endif