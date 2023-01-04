#ifndef NODE_1
#define NODE_1
#include <EEPROM.h>
#include <Arduino.h>
#include <HCSR04.h>

#include "myColor.h"
#include "user_RFID.h"
#include "user_Slot.h"
#include "eeprom.h"
#include "NRF24.h"
#include "TimerOne.h"

/* Address to comm with gateway */
#if defined NODE_1
const byte addresses[][6] = {"PKIOT0","PKIOT1"};
#elif defined(NODE_2)
const byte addresses[][6] = {"PKIOT0","PKIOT2"};
#endif

RF24 myRadio(7, 8); // CE, CSN

user_data_ID user_booking_slot[N0_NODE_CAR * 2] = { {0, 0, 0, 0} };

user_data_ID user_ID[N0_USER_ID] = { {0} };

byte current_status[N0_NODE_CAR * 2] = {GREEN_COLOR};
int *data_slot_rcv = new int[10];

/* RFID declare */
RFID rfid(SS_2_PIN, RST_PIN); //create an instance rfid for the class RFID
RFID rfid2(SS_1_PIN, RST_PIN); //create an instance rfid for the class RFID

String cardNum;
unsigned long RFID;

byte nuidPICC[4];

/* HC-SR04 pin-out*/
/* initialisation class HCSR04 (trig pin , echo pin, number of sensor) */
HCSR04 hc(A5, new int[2]{1, 5}, 2);
unsigned long int currentMillis = 0;

void init_data_radio() {
  for (int i = 0; i < 30; i++) {
    dataTransmit.text[i] = '\0';
    dataReceive.text[i] = '\0';
  }
}

void init_radio() {
  myRadio.begin(); 
  myRadio.setAutoAck(1);
  myRadio.setRetries(5, 15);
  // #if defined NODE_1
  myRadio.setChannel(115); 
  // #elif defined(NODE_2)
  //   myRadio.setChannel(100); 
  // #endif
  myRadio.setPALevel(RF24_PA_MAX);
  myRadio.setDataRate( RF24_250KBPS );
  #if defined NODE_1
    myRadio.openReadingPipe(1, addresses[0]);
  #elif defined(NODE_2)
    myRadio.openReadingPipe(1, addresses[0]);
  #endif

  myRadio.startListening();
}

void init_slot() {
  for (int i = 0; i < N0_NODE_CAR; i++) {
    slot_car_status[i].startMillisCar = millis();
    slot_car_status[i].distance = UNDEFINED;
    slot_car_status[i].flag_checking_slot = false;
    slot_car_status[i].doneChecking = false;
    slot_car_status[i].status = SLOT_EMPTY;
    slot_car_status[i].is_slot_reserved = false;
    slot_car_status[i].UID_reserved[i] = -1;
  }
}

void get_car_distance() {
  for (int i = 0; i < N0_NODE_CAR; i++)      {
    slot_car_status[i].distance = hc.dist(i);
    // Serial.print(F("Car: ")); Serial.print(i); Serial.print(F(" "));Serial.println(slot_car_status[i].distance);
    delay(60);
  }
}

void check_slot_status() {
  /* Get distance of car */
  get_car_distance();  
  for (int i = 0; i < N0_NODE_CAR; i++) { 
    /* If this slot is reserved, then no need to check status */
    if (slot_car_status[i].status == SLOT_RESERVED) return;
    /* Else */
    if (slot_car_status[i].distance <= 10 && slot_car_status[i].flag_checking_slot == false) {
      slot_car_status[i].startMillisCar = currentMillis;
      slot_car_status[i].flag_checking_slot = true;
      slot_car_status[i].doneChecking = false;
    } else if (slot_car_status[i].distance > 10) {
      slot_car_status[i].status = SLOT_EMPTY;
      slot_car_status[i].flag_checking_slot = false;
    }
    
    if (slot_car_status[i].doneChecking == false) {
      if (slot_car_status[i].flag_checking_slot && (currentMillis - slot_car_status[i].startMillisCar >= TIME_VALID) ){  
        slot_car_status[i].doneChecking = true;
        slot_car_status[i].flag_checking_slot = false;
        slot_car_status[i].status = SLOT_FULL;
      }
      // else {
      //   slot_car_status[i].status = SLOT_EMPTY;
      // }
    }
  }
}

void updateColorCorrespondingToCarSLot(int status_slot, int &colorEn) {
  switch (status_slot) {
    case SLOT_EMPTY:
      updateColorIndex(colorEn, GREEN_COLOR);
      break;
    case SLOT_FULL:
      updateColorIndex(colorEn, RED_COLOR);
      break;
    case SLOT_RESERVED:
      updateColorIndex(colorEn, YELLOW_COLOR);
      break;
  }
}

void assign_color() {
  int slot = 0;
  // #if defined NODE_2
  //     slot += 2;
  // #endif
  current_status[slot] = color_En1;
  current_status[slot + 1] = color_En2;
}

int check_any_changes() {
  int slot = 0;
  // #if defined NODE_2
  //     slot += 2;
  // #endif
  
  if (current_status[slot] != color_En1) return 2;
  if (current_status[slot + 1] != color_En2) return 3;

  return 0;
}

void restoreDataFromEEPROM() {
    //eepromWriteStruct(address_user_ID, user_ID, sizeof(user_ID));
    eepromReadStruct(address_slot_ID, user_booking_slot, sizeof(user_booking_slot));
    color_En1 = EEPROM.read(address_color_of_slot[0]);
    color_En2 = EEPROM.read(address_color_of_slot[1]);
    assign_color();
    current_user = EEPROM.read(address_number_of_users);
}

void receive_reserved_booking_slot(int slot, int *UID, int id) {  
  /* Update slot status and start waiting customer */
  #if defined NODE_1
    if (slot >= 2) return;
  #elif defined(NODE_2)
    if (slot < 2) return;
    else slot -= 2;
  #endif
  
  slot_car_status[slot].status = SLOT_RESERVED;
  slot_car_status[slot].is_slot_reserved = true;
  user_booking_slot[slot].startWaitingCustomer = true;
  user_booking_slot[slot].is_arrived = false;  
  /* Store UID to EEPROM */
  int index = 0;
  for (int i = 0; i < 4; i++) {
    user_booking_slot[slot].ID[index++] = *UID++; 
    
  }
  
  //eepromWriteStruct(address_slot_ID, user_booking_slot, sizeof(user_booking_slot));
  /* Send back code of package to confirm package is received */
  close_slot(slot);
  confirm_data_receive(id);
  user_booking_slot[slot].currentTimeOut = millis();
  // for (int i = 0; i < 4; i++) Serial.println(user_booking_slot[slot].ID[i]);
}

void clear_user_slot(int slot) {
  for (int i = 0; i < 4; i++) {
    user_booking_slot[slot].ID[i] = -1;
  }
}

void check_customer_arrived(int slot) {
  /* If the slot hadn't got reserved, then return */
  if (slot_car_status[slot].is_slot_reserved == 0) return;
  /* If time-out for customer is exceeded or the customer is arrived, then open the slot */
  if (millis() - user_booking_slot[slot].currentTimeOut >= TIME_WAITING || user_booking_slot[slot].is_arrived) {
    user_ID[slot].startWaitingCustomer = false;
    clear_user_slot(slot);
    slot_car_status[slot].is_slot_reserved = 0;
    slot_car_status[slot].status = SLOT_EMPTY;
    open_slot(slot);
  }
}

void receive_from_radio() {
  if ( myRadio.available()) {
    while (myRadio.available()){
      myRadio.read( &dataReceive, sizeof(dataReceive) );
    }
    // Serial.println("Receive: ");
    String data = String(dataReceive.text);
    // Serial.println(data);
    // data.remove(0, 4);
    /* Parse command from radio */
    data_slot_rcv = parse_command(data);

    if (data_slot_rcv == NULL) return;
      
    receive_reserved_booking_slot(data_slot_rcv[0], &data_slot_rcv[1], dataReceive.id);
    delay(20);
  } else {
    // Serial.println("not available");
  }
}

void send_package(String str){
  myRadio.stopListening();
  // send_data_nrf.id = send_data_nrf.id + 1;
  for(int i = 0; i < str.length();i++){
    dataTransmit.text[i] = str[i];
  }
  myRadio.openWritingPipe(addresses[1]);
  
  // Format : !RESERVED:slost:uid#
  if (!myRadio.write(&dataTransmit, sizeof(dataTransmit))) {
    // Serial.println("Don't send to NRF");
  } else {
    // Serial.print("Sent"); Serial.println(str);
  }

  #if defined NODE_1
    myRadio.openReadingPipe(1, addresses[0]);
  #elif defined(NODE_2)
    myRadio.openReadingPipe(1, addresses[0]);
  #endif
  
  myRadio.startListening();
  // clear data;  
  for(int i = 0; i < 30; i++){
    dataTransmit.text[i] = '\0';
  }
  delay(100);
}

void send_new_data_to_gateWay(int slot, int status, String UID) {
  /* Check which type data send to gateway, UID or slot status
  * if data send is UID */
  #if defined NODE_2
    slot += 2;
  #endif
  String data_send;
    if ( UID != "") {
      // use RF
      data_send += "!UID:" + UID + "#";
    } else { // data send is slot status format !SLOT:slot-value:status-value#
      data_send = "!SLOT:" + String(slot) + ":" + String(status) + "#";
    }
    send_package(data_send);
}

void setup() {
  
  currentMillis = millis();
  init_slot();
  SPI.begin();    
  // Serial.begin(9600);
  for (int i = 0; i < N0_NODE_CAR * 2; i++) {
    user_booking_slot[i].currentTimeOut = millis();
    user_booking_slot[i].is_arrived = 0;
    user_booking_slot[i].startWaitingCustomer = 0;
  } 
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(en1, OUTPUT);
  pinMode(en2, OUTPUT);

  //restoreDataFromEEPROM();
  
  servo_s1.detach();
  servo_s2.detach();
  // for(int i=0; i< EEPROM.length(); i++){
  //   EEPROM.write(i,0);
  // }

  rfid.init();
  rfid2.init();
  initLED();
  init_radio();
  init_data_radio();

  Timer1.initialize(1000);
  Timer1.attachInterrupt(scan_led);
}

void loop() {
  currentMillis = millis();
  assign_color();
  readRfid(rfid, 0);
  readRfid(rfid2, 1);
  // /* Check if any package received from gateway */
  receive_from_radio();

  /* Check all slot status if it having been reserved */
  for (int i = 0; i < N0_NODE_CAR; i++) {
    check_customer_arrived(i);
  }

  check_slot_status();

  updateColorCorrespondingToCarSLot(slot_car_status[0].status, color_En1);
  updateColorCorrespondingToCarSLot(slot_car_status[1].status, color_En2);

  /* Check if any slot status changes, if so, send status data to gateway */
  if (check_any_changes() == 2) {
    int slot_temp = 0;
    send_new_data_to_gateWay(slot_temp, slot_car_status[0].status, "");
  } else if (check_any_changes() == 3){
    int slot_temp = 0;
    send_new_data_to_gateWay(slot_temp + 1, slot_car_status[1].status, "");
  }
}


void readRfid(class RFID rfid, int slot)
{
  int count = 0;
  int temp[4];
  if (slot == 0) {
    for (int i = 0; i < 4; i++) {
      temp[i] = user_booking_slot[slot].ID[i];
      // Serial.println(temp[i]);
    }
  }

  if (rfid.isCard())
  {
    // for (int i = 0; i < 4; i++) Serial.println(temp[i]);

    if (rfid.readCardSerial())
    {
      for (int i = 0; i < 4; i++)
      {
        RFID = rfid.serNum[i];
        nuidPICC[i] = RFID;
        cardNum += RFID; // store RFID value into string "cardNum" and concatinate it with each iteration
        // Serial.println(RFID);
        // Serial.println(user_booking_slot[slot].startWaitingCustomer);
        
        if (user_booking_slot[slot].ID[i] == nuidPICC[i] || temp[i] == nuidPICC[i]) count++;
        
      }
    }

    // if (check_if_new_id(&nuidPICC[0])) {
    //   store_new_UID_into_EEPROM(&nuidPICC[0]);
    //   send_new_data_to_gateWay(-1, -1, &nuidPICC[0]);
    // }
    if (count == 4 && user_booking_slot[slot].startWaitingCustomer) {
      user_booking_slot[slot].is_arrived = 1;
      clear_user_slot(slot);
    }

    printRfid();    
  }  
  
  rfid.halt();
}

void printRfid()
{
 if (cardNum != '\0')//if string cardNum is not empty, print the value
 {
    // Serial.println("Card found");
    // Serial.print("Cardnumber: ");
    // Serial.println(cardNum);
    cardNum.remove(0);
  //This is an arduino function.
  //remove the stored value after printing. else the new card value that is read
  // will be concatinated with the previous string.
  delay(500); 
 }
}

#endif
