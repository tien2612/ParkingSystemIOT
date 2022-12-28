#include <EEPROM.h>
#include <Arduino.h>
#include <HCSR04.h>

#include "myColor.h"
#include "user_RFID.h"
#include "user_Slot.h"
#include "eeprom.h"
#include "NRF24.h"

/* Index of led RGB */
int index = 0;

user_data_ID user_ID_Slot[N0_NODE_CAR * 2] = { {0, 0, 0, 0} };

user_data_ID user_ID[N0_USER_ID] = { {0} };

static int UID_Read[4];

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.
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
      } else {
        slot_car_status[i].status = SLOT_EMPTY;
      }
      
    }
  }
}

void get_car_distance() {
  for (int i = 0; i < N0_NODE_CAR; i++)      {
    slot_car_status[i].distance = hc.dist(i);
    Serial.println(slot_car_status[i].distance);
  }
  delay(300);
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

void read_new_user_uid() {
  if (current_user >= N0_USER_ID) return;
  
  for (byte i = 0; i < mfrc522.uid.size; i++) { 
      Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");   
      user_ID[current_user++].ID[i] = mfrc522.uid.uidByte[i];
  }

  /* Push new data to firestore */
  send_new_uid_to_gateWay(user_ID[current_user - 1].ID);
  /* Store data in EEPROM */
  //eepromWrite(address_user_ID, user_ID, sizeof(user_ID));

  for (int i = 0; i < N0_USER_ID; i++) {
    for (int j = 0; j < 4; j++) {
      Serial.println("UID "); Serial.println(i + " "); Serial.print(user_ID[i].ID[j]);  
    }
  }
}

void restoreDataFromEEPROM() {
    // eepromRead(address_user_ID, user_ID, sizeof(user_ID));
    // eepromRead(address_slot_ID, user_ID_Slot, sizeof(user_ID_Slot));
    current_user = EEPROM.read(address_number_of_users);
}

void receive_reverse_booking_slot(int slot, String UID) {
  if (slot >= N0_NODE_CAR) return;

  /* Update yellow color slot (reverse) and start waiting customer within 15 minutes */
  updateColorIndex(slot, YELLOW_COLOR);
  
  /* Store UID to EEPROM */
  int index = 0;
  String data = "";
  for (int i = 0; i < sizeof(UID)/sizeof(UID[0]); i++) {
    if (i == ' ') {
      user_ID_Slot[slot].ID[index++] = data.toInt();  
      data = "";
    } else data += UID[i];
  }
  
  //eepromWrite(address_slot_ID, user_ID_Slot, sizeof(user_ID_Slot));

  confirm_data_receive("Rcv S" + String(slot));
  startWaitingCustomer = millis();
}

void setup() {
  // // put your setup code here, to run once:
  currentMillis = millis();
  startWaitingCustomer = millis();

  Serial.begin(9600);   

  init_slot();
  SPI.begin();    
  mfrc522.PCD_Init();

  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(en1, OUTPUT);
  pinMode(en2, OUTPUT);

  // pinMode(servo_slot1, OUTPUT);
  // pinMode(servo_slot2, OUTPUT);

  // restoreDataFromEEPROM();
  
  // servo_s1.attach(servo_slot1);
  // servo_s1.write(100);

  // servo_s2.attach(servo_slot2);
  // servo_s2.write(100);
  for(int i=0; i< EEPROM.length(); i++){
    EEPROM.write(i,0);
  }
  initLED();
}

int *data_slot_rcv = {0};

void loop() {
  currentMillis = millis();

  if (check_booking_receive() != NULL) {
    int *data_slot_rcv = {0};
    data_slot_rcv = check_booking_receive();
    receive_reverse_booking_slot( data_slot_rcv[0], String(data_slot_rcv[1]) );
    close_slot(data_slot_rcv[0]);
  }

  if (startWaitingCustomer >= TIME_WAITING && !user_ID_Slot[data_slot_rcv[0]].is_arrived) {
    updateColorIndex(data_slot_rcv[0], GREEN_COLOR);
    open_slot(data_slot_rcv[0]);
  }

  ledRGB(index, color_En1, color_En2);
  // update color of ledRGB controlled by en1
  if (index >= N0_NODE_CAR - 1) index = 0;
  else index++;

  //get_car_distance();
  
  updateColorCorrespondingToCarSLot(slot_car_status[0].status, color_En1);
  updateColorCorrespondingToCarSLot(slot_car_status[1].status, color_En2);

  delay(20);

  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  { 
    return; 
  }
  
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {  
    return;  
  }
  
  Serial.print("UID của thẻ: ");   
  
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  { 
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");   
    UID_Read[i] = mfrc522.uid.uidByte[i];
    Serial.print(UID_Read[i]);
  }

  Serial.println("   ");
  
  /* If new UID is read, store it into EEPROM */
  if (check_if_new_id(UID_Read)) {
    store_new_UID_into_EEPROM(UID_Read);
  }

  mfrc522.PICC_HaltA();  
  mfrc522.PCD_StopCrypto1();
}
