#include <EEPROM.h>
#include <Arduino.h>
#include <HCSR04.h>

#include "myColor.h"
#include "user_RFID.h"
#include "user_Slot.h"
#include "eeprom.h"
#include "NRF24.h"
#include "TimerOne.h"
RF24 myRadio(7, 8); // CE, CSN

user_data_ID user_booking_slot[N0_NODE_CAR * 2] = { {0, 0, 0, 0} };

user_data_ID user_ID[N0_USER_ID] = { {0} };
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
  // dataReceive.id = -1;
  // dataReceive.text[0] = "";
  // dataTransmit.text[0] = "";
  // dataTransmit.id = -1;
}

void init_radio() {
  myRadio.begin();  
  myRadio.setChannel(115); 
  myRadio.setPALevel(RF24_PA_MAX);
  myRadio.setDataRate( RF24_250KBPS );
  
  myRadio.openReadingPipe(1, addresses[0]);
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

void restoreDataFromEEPROM() {
    //eepromWriteStruct(address_user_ID, user_ID, sizeof(user_ID));
    // eepromReadStruct(address_slot_ID, user_booking_slot, sizeof(user_booking_slot));
    // color_En1 = EEPROM.read(address_color_of_slot[0]);
    // color_En2 = EEPROM.read(address_color_of_slot[1]);

    current_user = EEPROM.read(address_number_of_users);
}

void receive_reserved_booking_slot(int slot, int *UID, int id) {  
  /* Update slot status and start waiting customer */
  slot_car_status[slot].status = SLOT_RESERVED;
  slot_car_status[slot].is_slot_reserved = 1;
  user_booking_slot[slot].startWaitingCustomer = 1;
  /* Store UID to EEPROM */
  int index = 0;
  for (int i = 0; i < 4; i++) {
    user_booking_slot[slot].ID[index++] = *UID++; 
  }
  
  //eepromWriteStruct(address_slot_ID, user_booking_slot, sizeof(user_booking_slot));
  /* Send back code of package to confirm package is received */
  confirm_data_receive(id);
  user_booking_slot[slot].currentTimeOut = millis();
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
  if (currentMillis - user_booking_slot[slot].currentTimeOut >= TIME_WAITING || user_booking_slot[slot].is_arrived) {
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
    data.remove(0, 4);
    /* Parse command from radio */
    data_slot_rcv = parse_command(data);
    // for (int i = 0; i < 5; i++) Serial.println(data_slot_rcv[i]);
    // Serial.print("\n");
    receive_reserved_booking_slot(data_slot_rcv[0], &data_slot_rcv[1], dataReceive.id);
    delay(20);
  }
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
  pinMode(servo_slot1, OUTPUT);
  pinMode(servo_slot2, OUTPUT);

  //restoreDataFromEEPROM();
  
  servo_s1.attach(servo_slot1);
  servo_s1.write(100);

  servo_s2.attach(servo_slot2);
  servo_s2.write(100);
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
  // Serial.println("begin");
  user_booking_slot[0].ID[0] = 177;
  user_booking_slot[0].ID[1] = 55;
  user_booking_slot[0].ID[2] = 68;
  user_booking_slot[0].ID[3] = 29;
  data_slot_rcv = parse_command("!RESERVED:0:177 55 68 29#");
  receive_reserved_booking_slot(data_slot_rcv[0], &data_slot_rcv[1], 19);
  // slot_car_status[0].is_slot_reserved = 1;
}

void loop() {
  currentMillis = millis();
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
}


void readRfid(class RFID rfid, int slot)
{
  int count = 0;
  if (rfid.isCard())
  {
    if (rfid.readCardSerial())
    {
      // Serial.println(slot);
      for (int i=0; i<4; i++)//card value: "xyz xyz xyz xyz xyz" (15 digits maximum; 5 pairs of xyz)hence 0<=i<=4 //
      {
        RFID = rfid.serNum[i];
        nuidPICC[i] = RFID;
        cardNum += RFID; // store RFID value into string "cardNum" and concatinate it with each iteration
        if (user_booking_slot[slot].ID[i] == nuidPICC[i]) count++;
        // Serial.println(user_booking_slot[slot].ID[i]);
      }
    }

    if (count == 4 && user_booking_slot[slot].startWaitingCustomer) {
      user_booking_slot[slot].is_arrived = 1;
      clear_user_slot(slot);
    }

    printRfid();
    // Serial.println(count);
    
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

