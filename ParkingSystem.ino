#include <EEPROM.h>
#include <Arduino.h>
#include <HCSR04.h>

#include "myColor.h"
#include "user_RFID.h"
#include "user_Slot.h"
#include "eeprom.h"
#include "NRF24.h"

user_data_ID user_ID_Slot[N0_NODE_CAR * 2] = { {0, 0, 0, 0} };

user_data_ID user_ID[N0_USER_ID] = { {0} };
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
    Serial.print("Car: "); Serial.print(i); Serial.print(" ");Serial.println(slot_car_status[i].distance);
  }
  delay(100);
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

void restoreDataFromEEPROM() {
    // eepromRead(address_user_ID, user_ID, sizeof(user_ID));
    // eepromRead(address_slot_ID, user_ID_Slot, sizeof(user_ID_Slot));
    color_En1 = EEPROM.read(address_color_of_slot[0]);
    color_En2 = EEPROM.read(address_color_of_slot[1]);

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
  user_ID[slot].startWaitingCustomer = millis();
}

void check_customer_arrived(int slot) {
  /* If the slot hadn't got reserved, then return */
  if (slot_car_status[slot].is_slot_reserved == 0) return;

  /* If time-out for customer is exceeded, then open the slot */
  if (user_ID[slot].startWaitingCustomer >= TIME_WAITING && !user_ID_Slot[slot].is_arrived) {
    updateColorIndex(slot, GREEN_COLOR);
    user_ID[i].startWaitingCustomer = false;
    open_slot(slot);
  }
}

void setup() {
  // put your setup code here, to run once:
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

  // pinMode(servo_slot1, OUTPUT);
  // pinMode(servo_slot2, OUTPUT);

  //restoreDataFromEEPROM();
  
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
}

int *data_slot_rcv = {0};
void(* resetFunc) (void) = 0; //declare reset function @ address 0
long int delta = 0;

void loop() {
  currentMillis = millis();
  /* Reset all RFID readers after 10s */
  if (millis() > delta + 10000){
    delta = millis();
    for (reader = 0; reader < NR_OF_READERS; reader++) {
      mfrc522[reader].PCD_SoftPowerDown();
      delay(10);
      mfrc522[reader].PCD_SoftPowerUp();

      delay(10);
      mfrc522[reader].PCD_Init();
      delay(4);
      resetFunc(); 
    }
  }
  /* Updating all led by switching sequentially */ 
  ledRGB(index, color_En1, color_En2);
  
  if (index >= N0_NODE_CAR - 1) index = 0;
  else index++;

  /* Check if any slot is reserved */
  if (check_booking_receive() != NULL) {
    int *data_slot_rcv = {0};
    data_slot_rcv = check_booking_receive();
    int slot_temp = data_slot_rcv[0];
    int UID_temp = data_slot_rcv[1];
    receive_reverse_booking_slot( slot_temp, String(UID_temp) );
    close_slot(slot_temp);

    slot_car_status[slot_temp].is_slot_reserved = true;
    user_ID[slot_temp].startWaitingCustomer = true;
  }

  /* Check all slot status if it having been reserved */
  for (int i = 0; i < N0_NODE_CAR * 2; i++) {
    check_customer_arrived(i);
  }

  get_car_distance();
  
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
        
        if (nuidPICC[i] == ID1[i]) {
          //updateColorIndex(color_En1, HIGH);
          digitalWrite(2, HIGH);
        }
        else if (nuidPICC[i] == ID2[i]) {
          //updateColorIndex(color_En1, LOW);
          digitalWrite(2, LOW);
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