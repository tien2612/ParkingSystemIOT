#ifndef user_RFID_h
#define user_RFID_h

#include "Arduino.h"
#include <SPI.h>
#include <MFRC522.h>
#include "user_Slot.h"
#include "eeprom.h"
#include "RFID.h"
#include <DigitalIO.h> // our software SPI library

// 4 car slot
#define N0_USER_ID      250 

#define SS_1_PIN 10  //slave select pin
#define SS_2_PIN 6  //slave select pin
#define RST_PIN 9  //reset pin
#define NR_OF_READERS 2

extern int current_user;

struct user_data_ID {
  byte ID[4];
  int is_arrived;
  bool startWaitingCustomer;
  unsigned long int currentTimeOut;
};

/* 4 car in 2 node
1 node - 2 cars
each RFID contains 1 UID to open barrier */
extern user_data_ID user_ID_Slot[N0_NODE_CAR * 2];
/* User RFID */
extern user_data_ID user_ID[N0_USER_ID];

int check_id(int *arr, int *data);


#endif