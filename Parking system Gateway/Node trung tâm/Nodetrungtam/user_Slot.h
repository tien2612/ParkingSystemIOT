#ifndef user_Slot_h
#define user_Slot_h

#include "Arduino.h"
#include "Servo.h"

#include <Wire.h> //Gọi thư viện I2C để sử dụng các thư viện I2C
#include <LiquidCrystal_I2C.h> //Thư viện LCD I2C

#define N0_NODE_CAR 2
#define N0_SLOT_IN_NODE 2

#define SLOT_EMPTY     3
#define SLOT_FULL      4
#define SLOT_REVERSE   5

const int servo_checkin = 5;
extern Servo servo;

extern LiquidCrystal_I2C lcd; //Khai báo địa chỉ I2C (0x27 or 0x3F) và LCD 16x02
extern bool flag_closed_barrier;
void setupLCD(void);
void open_barrier(void);
void close_barrier(void);

void displayLCD(void);
void updateDisplayLCD(int slot);

extern int status_slot[N0_NODE_CAR * N0_SLOT_IN_NODE];

#endif