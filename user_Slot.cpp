#include "user_Slot.h"

slot_status slot_car_status[N0_NODE_CAR];

Servo servo_s1;
Servo servo_s2;

void open_slot(int slot) {
  Serial.print("open slot"); Serial.println(slot);
  if (slot == 1) {
    servo_s1.write(100);
  } else if (slot == 2) {
    servo_s2.write(100);
  }
}

void close_slot(int slot) {
  if (slot == 1) {
    servo_s1.write(0);
  } else if (slot == 2) {
    servo_s2.write(0);
  }
}