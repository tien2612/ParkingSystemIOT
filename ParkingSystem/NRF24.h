#include "WString.h"
#ifndef NRF24_H
#define NRF24_H

#include "Arduino.h"
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "user_Slot.h"

#define GOT_BOOK        20
#define NOTHING         21

extern const byte addresses[][6];


struct package {
  int id = 123;
  char text[100] = "TEXT TO SENT";
};

typedef struct package Package;

extern Package dataReceive;
extern Package dataTransmit;

/* Address to comm with gateway */
extern const byte address[6][6];

/* Data receive from radio */
extern int booking_slot;
extern int *UID;
/* Start calculate time-out */

int* check_booking_receive();

void confirm_data_receive(int code);

int *parse_command(String cmd);
#endif
