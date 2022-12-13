#include <Arduino.h>
#include <HCSR04.h>

#include "myColor.h"
#include "user_RFID.h"
#include "user_Slot.h"
//#include "user_Slot.h"

/* Index of led RGB */
int index = 0;
slot_status slot_car_status[N0_NODE_CAR];


/* Declare car slot infor */

/* HC-SR04 pin-out*/
/* initialisation class HCSR04 (trig pin , echo pin, number of sensor) */
HCSR04 hc(2, new int[2]{4, 6}, 2);
unsigned long int currentMillis = 0;

void init_slot() {
  for (int i = 0; i < N0_NODE_CAR; i++) {
    slot_car_status[i].startMillisCar = millis();
    slot_car_status[i].distance = UNDEFINED;
    slot_car_status[i].flag_checking_slot = false;
    slot_car_status[i].doneChecking = false;
    slot_car_status[i].status = SLOT_EMPTY;
  }
}

void check_slot_status() {
  for (int i = 0; i < N0_NODE_CAR; i++) {
    if (slot_car_status[i].distance <= 5 && slot_car_status[i].flag_checking_slot == false) {
      slot_car_status[i].startMillisCar = currentMillis;
      slot_car_status[i].flag_checking_slot = true;
      slot_car_status[i].doneChecking = false;
    } else if (slot_car_status[i].distance > 5) {
      slot_car_status[i].status = SLOT_EMPTY;
      slot_car_status[i].flag_checking_slot = false;
    }
    
    if (slot_car_status[i].doneChecking == false) {

      if (slot_car_status[i].flag_checking_slot && (currentMillis - slot_car_status[i].startMillisCar >= TIME_VALID)){  
        slot_car_status[i].doneChecking = true;
        slot_car_status[i].flag_checking_slot = false;
        slot_car_status[i].status = SLOT_FULL;
        Serial.println(slot_car_status[0].status);
      } else {
        slot_car_status[i].status = SLOT_EMPTY;
      }
      
    }
  }
}

void get_car_distance() {
  for (int i = 0; i < N0_NODE_CAR; i++)      {
    slot_car_status[i].distance = hc.dist(i);
  }
  delay(60);
}

void updateColorCorrespondingToCarSLot(int status_slot, int &colorEn) {
  switch (status_slot) {
    case SLOT_EMPTY:
      updateColorIndex(colorEn, GREEN_COLOR);
      break;
    case SLOT_FULL:
      updateColorIndex(colorEn, RED_COLOR);
      break;
    case SLOT_REVERSE:
      updateColorIndex(colorEn, YELLOW_COLOR);
      break;
  }
}
unsigned long int test;
void setup() {
  // put your setup code here, to run once:
  currentMillis = millis();
  test = millis();
  init_slot();

  Serial.begin(9600);
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(en1, OUTPUT);
  pinMode(en2, OUTPUT);

  initLED();
}

String data = "";
int test33 = 0;
void loop() {
  currentMillis = millis();

  ledRGB(index, color_En1, color_En2);
  // update color of ledRGB controlled by en1
  if (index >= N0_NODE_CAR - 1) index = 0;
  else index++;

  //get_car_distance();
  



  
  while(Serial.available()) {
        data = Serial.readStringUntil('\n');
        Serial.println(data);
        slot_car_status[0].distance = data.toFloat();
        Serial.println(slot_car_status[0].distance);
  }
  
  
 // slot_car_status[1].distance = 4;
check_slot_status();
  // for (int i = 0; i < 2; i++) {
  //       Serial.print("car "); Serial.print(i);Serial.print(" ");Serial.println(slot_car_status[i].distance);
  // }
  updateColorCorrespondingToCarSLot(slot_car_status[0].status, color_En1);
  updateColorCorrespondingToCarSLot(slot_car_status[1].status, color_En2);
  
  delay(20);
}