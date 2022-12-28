#ifndef user_RFID_h
#define user_RFID_h

#include "Arduino.h"
#include <SPI.h>
#include <MFRC522.h>
#include "user_Slot.h"
#include "eeprom.h"

// 4 car slot
#define N0_USER_ID      250 

<<<<<<< Updated upstream
#define SS_1_PIN 10  //slave select pin
#define SS_2_PIN 6  //slave select pin
#define RST_PIN 9  //reset pin
#define NR_OF_READERS 2
=======
#define RST_PIN         9
#define SS_PIN          10
>>>>>>> Stashed changes

extern int current_user;

struct user_data_ID {
  int ID[4];
  int is_arrived;
};

/* 4 car in 2 node
1 node - 2 cars
each RFID contains 1 UID to open barrier */
extern user_data_ID user_ID_Slot[N0_NODE_CAR * 2];
/* User RFID */
extern user_data_ID user_ID[N0_USER_ID];

int check_id(int *arr, int *data);

<<<<<<< Updated upstream
void send_new_data_to_gateWay(int slot, int status, int *uid);
=======
void send_new_uid_to_gateWay(int *uid);
>>>>>>> Stashed changes

#endif
