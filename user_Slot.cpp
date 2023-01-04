#include "user_Slot.h"

slot_status slot_car_status[N0_NODE_CAR];

ServoTimer2 servo_s1;
ServoTimer2 servo_s2;
const int servo_slot1 = 0;
const int servo_slot2 = 15;


void open_slot(int slot) {
  if (slot == 1) {
    servo_s1.attach(servo_slot1);
    servo_s1.write(1500);  //pulse width for 90 degree
  } else if (slot == 0) {
    servo_s1.attach(servo_slot1);
    servo_s1.write(1500);  //pulse width for 90 degree
  }
}

void close_slot(int slot) {
  if (slot == 1) {
    servo_s1.attach(servo_slot1);
    servo_s1.write(750);  //min pulse width for 0 degree
  } else if (slot == 0) {
    servo_s2.attach(servo_slot2);
    servo_s2.write(750);  //min pulse width for 0 degree
  }
}