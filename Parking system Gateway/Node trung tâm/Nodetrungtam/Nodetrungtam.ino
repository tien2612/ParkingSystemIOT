#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <DigitalIO.h>

#include <Arduino.h>

#include <SoftwareSerial.h>

#include <Wire.h> //Gọi thư viện I2C để sử dụng các thư viện I2C
#include <LiquidCrystal_I2C.h> //Thư viện LCD I2C

#include <HCSR04.h>

#include <SPI.h>
#include <MFRC522.h>

#include "user_Slot.h"
#include "user_RFID.h"

#define RST_PIN         9
#define SS_PIN          10

RF24 radio(2,3);// 7 : CE and 8 : CSN
const byte Address[6][6] = {"PKIOT1", "PKIOT2", "PKIOT3" }; // Node 0(trungtam) : Node 1 : Node 2

SoftwareSerial Arduino_softSerial(5, 6); // RX: 5 - TX: 6

HCSR04 hc(2, 4); // //initialisation class HCSR04 (trig pin , echo pin)

// LiquidCrystal_I2C lcd(0x3F,16,2); //Khai báo địa chỉ I2C (0x27 or 0x3F) và LCD 16x02

MFRC522 mfrc522(SS_PIN, RST_PIN);
int UID[4], i;

String send_data_nrf;
String receive_data_nrf;

String send_data_esp;
String receive_data_esp;


void comparser_command_data(String cmd){
  
  cmd.replace("!", "");
  cmd.replace("#", "");

  const char UID_Reg[] = "UID";
  const char SLOT_Reg[] = "SLOT";
  const char RESERVED_Reg[] = "RESERVED";
  
  String data_split[10]; 
  String data = "";
  int j = 0;
  for (int i = 0; i < cmd.length(); i++) {
    if (cmd[i] == ':') {
      data_split[j] = data;
      data = "";
      j++;
    } else data += cmd[i];
  }

  data_split[j] = data;
  Serial.println(data_split[0]);
  
  if (data_split[0] == UID_Reg){ // Format :!UID:String#
      // store new UID to EEPROM
      Serial.println(data_split[1]);
      update_user_ID(current_index_user_ID, data_split[1]);         

    } else if (data_split[0] == SLOT_Reg) {// Format : !SLOT:slot:status#      
      
      int slot = data_split[1].toInt();
      Serial.println(slot);

      int status = data_split[2].toInt();
      Serial.println(status);
      
      status_slot[slot] = status;
    } else if (data_split[0] == RESERVED_Reg) {// Format : !RESERVED:slot:uid#
      int slot = data_split[1].toInt();
      Serial.println(slot);   
         
      update_user_ID(slot, data_split[2]);
      Serial.println(data_split[2]);        

    }
}

// void before() {
//   // Make sure MFRC is disabled from the SPI bus
//   pinMode(RST_PIN, OUTPUT);
//   digitalWrite(RST_PIN, LOW);
//   pinMode(SS_PIN, OUTPUT);
//   digitalWrite(SS_PIN, LOW);
// }

void setup() {
  Serial.begin(9600);

  /* setup for RFID*/
  SPI.begin();    
  mfrc522.PCD_Init(); 
  // before();
  // pinMode(RST_PIN, INPUT);
  // digitalWrite(RST_PIN, HIGH);
  // pinMode(SS_PIN, OUTPUT);
  // digitalWrite(SS_PIN, LOW);


  if (!radio.begin()){
    Serial.println("Module isn't connected ...!!");
    while (1){}
  }
  radio.openWritingPipe(Address[0]); // Only write on a channel 
    // Open a channel to write on address"PKIOT0" 
  radio.openReadingPipe(1,Address[1]); // Open a channel to read on address"PKIOT1" (Node 1)
  radio.openReadingPipe(2,Address[2]); // Open a channel to read on address"PKIOT2" (Node 2)


  radio.setPALevel(RF24_PA_MIN); //Cài bộ khuyết địa công suất ở mức MIN
  radio.setChannel(80);
  radio.setDataRate(RF24_250KBPS);    

  if (!radio.available())
  {
    Serial.println("Chưa kết nối được với Node con...!!");
    Serial.println("CHỜ KẾT NỐI.......");
  }

  /////////////////
  Arduino_softSerial.begin(9600);
  
  pinMode(servo_checkin, OUTPUT);
  servo.write(0);

  /* setup for LCD*/
  lcd.begin(); //Khởi tạo màn hình LCD
  lcd.backlight(); //Bật đèn màn hình lCD  
 

}

void loop() {

  // comparser_command_data("!UID:100 15 20 30#");
  // comparser_command_data("!SLOT:2:1#"); 
  // comparser_command_data("!RESERVED:2:10 78 26 99#");  
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
    UID[i] = mfrc522.uid.uidByte[i];
    Serial.print(UID[i]);
  }
  
  Serial.println("   ");
  mfrc522.PICC_DumpToSerial(&mfrc522.uid);
  


  radio.stopListening();
  /////

  /////
  // Format : !RESERVED:slot:uid#
  send_data_nrf = receive_data_esp;
  radio.write(&send_data_nrf , sizeof(send_data_nrf));
  delay(10);

  radio.startListening();
  while (!radio.available());
  // Format : !UID:String# or !SLOT:slot:status#  
  radio.read(&receive_data_nrf , sizeof(receive_data_nrf));
  //////
  comparser_command_data(receive_data_nrf);
  //////
  delay(10);


  // send_data_esp = receive_data_nrf;
  // Arduino_softSerial.println(send_data_esp);
  // Serial.println("Gui du lieu toi ESP8266");
  //  // Format : !UID:String# or !SLOT:slot:status#  
  // Serial.println(send_data_esp);
  // send_data_esp = "";
  
  
  
  /* Received data from ESP8266 */
  // while(Arduino_softSerial.available()) {
  //   receive_data_esp = Arduino_softSerial.readStringUntil('\n');
  //   receive_data_esp.remove(receive_data_esp.length() - 1, 1);
  //   Serial.println("Du lieu nhan tu ESP8266");
  //   // Format : !RESERVED:slot:uid#
  //   Serial.println(receive_data_esp);
    
  //   comparser_command_data(receive_data_esp);
    
  //   receive_data_esp = "";    
  // }
  // delay(1000);

  Serial.println( hc.dist() ); //return current distance (cm) in serial
  delay(60);                   // we suggest to use over 60ms measurement cycle, in order to prevent trigger signal to the echo signal.

  mfrc522.PICC_HaltA();  
  mfrc522.PCD_StopCrypto1();

}

// void before() {
// // Make sure MFRC is disabled from the SPI bus
// pinMode(RST_PIN, INPUT);
// digitalWrite(RST_PIN, HIGH);
// pinMode(SS_PIN, OUTPUT);
// digitalWrite(SS_PIN, LOW);
// }
