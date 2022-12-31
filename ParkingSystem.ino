#include <EEPROM.h>
#include <Arduino.h>
#include <HCSR04.h>

#include "myColor.h"
#include "user_RFID.h"
#include "user_Slot.h"
#include "eeprom.h"
#include "NRF24.h"

user_data_ID user_booking_slot[N0_NODE_CAR * 2] = { {0, 0, 0, 0} };

user_data_ID user_ID[N0_USER_ID] = { {0} };

package Package;
/* Index of led RGB */
int index = 0;
//MFRC522 mfrc522(SS_PIN, RST_PIN); // instatiate a MFRC522 reader object.
MFRC522 mfrc522[NR_OF_READERS];   // Create MFRC522 instance.
MFRC522::MIFARE_Key key; //create a MIFARE_Key struct named 'key', which will hold the card information

byte ssPins[] = {SS_1_PIN, SS_2_PIN};

byte nuidPICC[4];

uint8_t reader, i;

static int UID_Read[4];
int ID1[4] = {160, 59, 216, 32}; //Thẻ mở đèn
int ID2[4] = {144, 239, 110, 32} ; //Thẻ mở đèn

/* HC-SR04 pin-out*/
/* initialisation class HCSR04 (trig pin , echo pin, number of sensor) */
HCSR04 hc(2, new int[2]{4, 5}, 2);
unsigned long int currentMillis = 0;

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
    Serial.print(F("Car: ")); Serial.print(i); Serial.print(F(" "));Serial.println(slot_car_status[i].distance);
    delay(100);
  }
  delay(100);
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
    eepromReadStruct(address_slot_ID, user_booking_slot, sizeof(user_booking_slot));
    color_En1 = EEPROM.read(address_color_of_slot[0]);
    color_En2 = EEPROM.read(address_color_of_slot[1]);

    current_user = EEPROM.read(address_number_of_users);
}

void receive_reserved_booking_slot(int slot, int *UID) {
  if (slot >= N0_NODE_CAR) return;
  
  slot_car_status[slot].status = SLOT_RESERVED;
  /* Store UID to EEPROM */
  int index = 0;
  for (int i = 0; i < 4; i++) {
      user_booking_slot[slot].ID[index++] = UID[i];
  }
  
  eepromWriteStruct(address_slot_ID, user_booking_slot, sizeof(user_booking_slot));
  /* Send back code of package to confirm package is received */
  confirm_data_receive(Package.code);
  user_booking_slot[slot].startWaitingCustomer = millis();
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
  if (user_booking_slot[slot].startWaitingCustomer >= TIME_WAITING || user_booking_slot[slot].is_arrived) {
    user_ID[i].startWaitingCustomer = false;
    clear_user_slot(slot);
    slot_car_status[slot].is_slot_reserved = 0;
    slot_car_status[slot].status = SLOT_EMPTY;
    open_slot(slot);
  }
}

void setup() {
  // put your setup code here, to run once:
  // user_booking_slot[0].ID[0] = 144;
  // user_booking_slot[0].ID[1] = 239;
  // user_booking_slot[0].ID[2] = 110;
  // user_booking_slot[0].ID[3] = 32;
  // user_booking_slot[0].startWaitingCustomer = 1;
  // slot_car_status[0].status = SLOT_RESERVED;

  currentMillis = millis();
  for (int i = 0; i < N0_NODE_CAR * 2; i++) {
    user_ID[i].startWaitingCustomer = millis();
  } 

  Serial.begin(9600);   

  init_slot();
  SPI.begin();    

  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(en1, OUTPUT);
  pinMode(en2, OUTPUT);
  pinMode(7, OUTPUT);
  // pinMode(servo_slot1, OUTPUT);
  // pinMode(servo_slot2, OUTPUT);

  restoreDataFromEEPROM();
  
  // servo_s1.attach(servo_slot1);
  // servo_s1.write(100);

  // servo_s2.attach(servo_slot2);
  // servo_s2.write(100);
  // for(int i=0; i< EEPROM.length(); i++){
  //   EEPROM.write(i,0);
  // }

  Serial.begin(9600); // Initialize serial communications with the PC
  SPI.begin(); // Init SPI bus
  for (reader = 0; reader < NR_OF_READERS; reader++) {
    mfrc522[reader].PCD_Init(ssPins[reader], RST_PIN); // Init each MFRC522 card
    Serial.print(F("Reader "));
    Serial.print(reader);
    Serial.print(F(": "));
    mfrc522[reader].PCD_DumpVersionToSerial();
  }
  Serial.println("Scan a MIFARE Classic card");
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF; //keyByte is defined in the "MIFARE_Key" 'struct' definition in the .h file of the library
  }
  
  initLED();
  // slot_car_status[0].status = SLOT_RESERVED;
  // slot_car_status[0].is_slot_reserved = 1;
}

int *data_slot_rcv = {0};
void(* resetFunc) (void) = 0; //declare reset function @ address 0
long int delta = 0;

void loop() {
  currentMillis = millis();
  /* Reset all RFID readers after 10s */
  // if (millis() > delta + 10000){
  //   delta = millis();
  //   for (reader = 0; reader < NR_OF_READERS; reader++) {
  //     mfrc522[reader].PCD_SoftPowerDown();
  //     delay(10);
  //     mfrc522[reader].PCD_SoftPowerUp();

  //     delay(10);
  //     mfrc522[reader].PCD_Init();
  //     delay(4);
  //     resetFunc(); 
  //   }
  // }
  /* Updating all led by switching sequentially */ 
  ledRGB(index, color_En1, color_En2);
  
  if (index >= N0_NODE_CAR - 1) index = 0;
  else index++;

  /* Check if any package received from gateway */
  if (check_booking_receive() != NULL) {
    int *data_slot_rcv = {0};
    String data_received = Package.text;
    data_slot_rcv = parse_command(data_received);
    int slot_temp = data_slot_rcv[0];
    int UID_temp[4];
    for (int i = 1; i < 5; i++) {
      UID_temp[i - 1] = data_slot_rcv[i];
    }    
    receive_reserved_booking_slot( slot_temp, UID_temp );
    close_slot(slot_temp);

    slot_car_status[slot_temp].is_slot_reserved = true;
    user_ID[slot_temp].startWaitingCustomer = true;
    delay(50);
  }

  /* Check all slot status if it having been reserved */
  for (int i = 0; i < N0_NODE_CAR; i++) {
    check_customer_arrived(i);
  }

  check_slot_status();

  updateColorCorrespondingToCarSLot(slot_car_status[0].status, color_En1);
  updateColorCorrespondingToCarSLot(slot_car_status[1].status, color_En2);

  for (reader = 0; reader < NR_OF_READERS; reader++){
    if (mfrc522[reader].PICC_IsNewCardPresent() && mfrc522[reader].PICC_ReadCardSerial()) {

        Serial.print("Card UID: ");   
        for (byte i = 0; i < 4; i++) {
          Serial.print(mfrc522[reader].uid.uidByte[i] < 0x10 ? " 0" : " ");   
          nuidPICC[i] = mfrc522[reader].uid.uidByte[i];
          Serial.print(nuidPICC[i]);
        }
        
        if (nuidPICC[i] == user_booking_slot[reader].ID[i]) {
          //updateColorIndex(color_En1, HIGH);
            user_booking_slot[reader].is_arrived = 1;
            Serial.println("Arrived\n");
        }
        else if (nuidPICC[i] == ID2[i]) {
          //updateColorIndex(color_En1, LOW);
          analogWrite(red, 0);
        }
        else Serial.println("Wrong card");
        // if (slot_car_status[0].is_slot_reserved == true) {
        //   if (nuidPICC[i] == slot_car_status[0].UID_reserved) {
        //     slot_car_status[0].is_slot_reserved = false;
        //     user_ID[0].is_arrived = true;
        //     open_slot(0);
        //   }
        // } else if (slot_car_status[1].is_slot_reserved == true) {
        //   if (nuidPICC[i] == slot_car_status[1].UID_reserved) {
        //     slot_car_status[1].is_slot_reserved = false;
        //     user_ID[1].is_arrived = true;
        //     open_slot(1);
        //   }
        // } else Serial.println("Wrong card!");
        
        Serial.println("   ");             
    }

      mfrc522[reader].PICC_HaltA();  
      mfrc522[reader].PCD_StopCrypto1();
      delay(100);
  }
}