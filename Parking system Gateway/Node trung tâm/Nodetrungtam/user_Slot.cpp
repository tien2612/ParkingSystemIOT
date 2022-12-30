#include "user_Slot.h"

Servo servo;

int status_slot[N0_NODE_CAR * N0_SLOT_IN_NODE];

void open_barrier(){
  servo.write(90);
}
void close_barrier(){
  servo.write(0);
}