#include "user_Slot.h"

Servo servo;

void open_barrier(){
  servo.write(90);
}
void close_barrier(){
  servo.write(0);
}