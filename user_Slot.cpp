#ifndef NODE_1
#define NODE_1
#include "user_Slot.h"

slot_status slot_car_status[N0_NODE_CAR];

ServoTimer2 servo_s1;
ServoTimer2 servo_s2;
const int servo_slot1 = 0;
const int servo_slot2 = 15;


void open_slot(int slot) {
  // slot_car_status[0].status = 4;
  if (slot == 1) {
    #if defined NODE_1
      servo_s2.attach(servo_slot2);
      servo_s2.write(750);  //min pulse width for 0 degree
      delay(215);
      servo_s2.write(1500);  //pulse width for 90 degree
    #elif defined(NODE_2)
      servo_s1.attach(servo_slot1);
      servo_s1.write(1500);  //pulse width for 90 degree
    #endif
  } else if (slot == 0) {
    #if defined NODE_1
      servo_s1.attach(servo_slot1);
      servo_s1.write(750);  //min pulse width for 0 degree
      delay(215);
      servo_s1.write(1500);  //pulse width for 90 degree

    #elif defined(NODE_2)
      servo_s1.attach(servo_slot1);
      servo_s1.write(1500);  //pulse width for 90 degree
    #endif
  }
}

void close_slot(int slot) {
  if (slot == 1) {
    // servo_s1.attach(servo_slot1);
    // servo_s1.write(750);  //min pulse width for 0 degree
    #if defined NODE_1
      servo_s2.attach(servo_slot2);
      servo_s2.write(2250);  //max pulse width for around 180 degree
      delay(220);
      servo_s2.write(1500);  //pulse width for 90 degree
    #elif defined(NODE_2)
      servo_s1.attach(servo_slot1);
      servo_s1.write(2500);  //min pulse width for 0 degree
    #endif
  } else if (slot == 0) {
    #if defined NODE_1
    servo_s1.attach(servo_slot1);
    servo_s1.write(2250);  //max pulse width for around 180 degree
    delay(220);
    servo_s1.write(1500);  //pulse width for 90 degree
    #elif defined(NODE_2)
      servo_s1.attach(servo_slot1);
      servo_s1.write(2500);  //min pulse width for 0 degree
    #endif
  }
}

#endif