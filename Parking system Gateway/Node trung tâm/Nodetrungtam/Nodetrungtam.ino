#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
// #include <DigitalIO.h>

#include <Arduino.h>

#include <SoftwareSerial.h>

#include <Wire.h> //Gọi thư viện I2C để sử dụng các thư viện I2C
#include <LiquidCrystal_I2C.h> //Thư viện LCD I2C

#include <HCSR04.h>

#include <MFRC522.h>

#include "RFID.h"

#include "eeprom.h"

#include "user_Slot.h"
#include "user_RFID.h"

#define RST_PIN         9
#define SS_PIN          6

#define CE_PIN         7
#define CSN_PIN          8

RF24 myRadio(CE_PIN,CSN_PIN);// 7 : CE and 8 : CSN
const byte addresses[6][6] = {"PKIOT1", "KIOT2", "KIOT3" }; // Node 0(trungtam) : Node 1 : Node 2

SoftwareSerial Arduino_softSerial(A2, A3); // RX: 5 - TX: 6

HCSR04 hc(A0, A1); // //initialisation class HCSR04 (trig pin , echo pin)

// LiquidCrystal_I2C lcd(0x3F,16,2); //Khai báo địa chỉ I2C (0x27 or 0x3F) và LCD 16x02

// MFRC522 mfrc522(SS_PIN, RST_PIN);
RFID rfid(SS_PIN, RST_PIN); //create an instance rfid for the class RFID

int UID[4];

struct package {
  int id = 1;
  char  text[30];
};

typedef struct package Package;

Package send_data_nrf;
Package receive_data_nrf;

String send_data_esp;
String receive_data_esp;

String cardNum;
int RFID;

// int count = 0;

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
  // Serial.println(data_split[0]);
  
  if (data_split[0] == UID_Reg){ // Format :!UID:String#
      // store new UID to EEPROM
      // Serial.println(data_split[1]);
      String data_temp = "";   
      int k = 0;
      for (int i = 0; i < data_split[1].length(); i++) {
        if (data_split[1][i] == ' ') {
          
          UID[k] = data_temp.toInt();
          data_temp = "";
          k++;
        } else data_temp += data_split[1][i];
      }
      if (check_if_new_id(UID)){
          store_new_UID_into_EEPROM(UID);
      }
            // update_user_ID(current_index_user_ID, data_split[1]);
     Transmit_Data_To_ESP();
       
  } else if (data_split[0] == SLOT_Reg) {// Format : !SLOT:slot:status#      
  
      int slot = data_split[1].toInt();
      // Serial.println(slot);

      int status = data_split[2].toInt();
      // Serial.println(status);
      
      status_slot[slot] = status;
      updateDisplayLCD(slot);
      Transmit_Data_To_ESP();
      
  } else if (data_split[0] == RESERVED_Reg) {// Format : !RESERVED:slot:uid#
      int slot = data_split[1].toInt();
      // Serial.println(slot);   
      
      // update_user_ID(slot, data_split[2]);
      // Serial.println(data_split[2]);        
      status_slot[slot] = 2;
      updateDisplayLCD(slot);      
      Transmit_Data_To_NRF();      
    }
}

void setup() {
  Serial.begin(9600);

  /* setup for RFID*/

  // before();
    
  Serial.println("initilizing RFID...");
  rfid.init(); // initilize the RFID module
  Serial.println("start ");

  myRadio.begin();  
  myRadio.setChannel(115); 
  myRadio.setPALevel(RF24_PA_LOW);
  myRadio.setDataRate( RF24_250KBPS );
  
  myRadio.openReadingPipe(1, addresses[0]);
  myRadio.startListening();

  /////////////////
  Arduino_softSerial.begin(9600);
  
  pinMode(servo_checkin, OUTPUT);
  servo.write(0);

  /* setup for LCD*/
  // lcd.begin(); //Khởi tạo màn hình LCD
  // lcd.backlight(); //Bật đèn màn hình lCD  
  setupLCD();

  // SPI.begin();    
  // mfrc522.PCD_Init(); 
  
}

// int *data_buffer_return = new int[10];

// int *parse_command(String cmd) {
//   cmd.remove(0, 1);
//   cmd.remove(cmd.length() - 1, 1);
//   Serial.println(cmd);
//   const char UID_Reg[] = "UID";
//   const char SLOT_Reg[] = "SLOT";
//   const char RESERVED_Reg[] = "RESERVED";
//   String data_split[10]; 
//   String data = "";
//   int j = 0;
//   for (int i = 0; i < cmd.length(); i++) {
//     if (cmd[i] == ':') {
//       data_split[j] = data;
//       data = "";
//       j++;
//     } else data += cmd[i];
//   }
  
//   data_split[j] = data;

//   Serial.println(data_split[0]);
//   if (data_split[0] == UID_Reg) {
//     // UID
//   } else if (data_split[0] == SLOT_Reg) {
//     // slot + status
//   } else if (data_split[0] == RESERVED_Reg) {
//     // slot + UID
//     data_buffer_return[0] = data_split[1].toInt();
//     int k = 1;
//     String data_temp = "";
    
//     for (int i = 0; i < data_split[2].length(); i++) {
     
//       if (data_split[2][i] == ' ') {
        
//         data_buffer_return[k] = data_temp.toInt();
//         data_temp = "";
//         k++;
//       } else data_temp += data_split[2][i];
//     }

//     data_buffer_return[k] = data_temp.toInt();
//   }
  
//   return data_buffer_return;
// }

// int *arr = new int[10];

void Receive_Data_From_NRF(void){
    if ( myRadio.available()) {
    while (myRadio.available()){
        // Serial.println("aaaa"); // Format : !UID:String# or !SLOT:slot:status#  
      myRadio.read( &receive_data_nrf, sizeof(receive_data_nrf) );
    }
    Serial.println("Receive data from NRF: ");
    String data = String(receive_data_nrf.text); 
    Serial.println(data);
    // arr = parse_command(data);
    comparser_command_data(data);
    data = "";
    // for (int i = 0; i < 5; i++) Serial.println(data_buffer_return[i]);
    delay(100);
    // Serial.print("\n");
  }
  delay(200);
}

void Transmit_Data_To_NRF(void){
  
  myRadio.stopListening();
  // send_data_nrf.id = send_data_nrf.id + 1;
  for(int i = 0; i< receive_data_esp.length();i++){
    send_data_nrf.text[i] = receive_data_esp[i];
  }
  Serial.println("Transmit data to nrf: ");
  // Serial.print("Package:");
  // Serial.print(send_data_nrf.id);
  // Serial.print("\n");
  
  Serial.println(send_data_nrf.text);
  // Serial.print("\n");
  myRadio.openWritingPipe(addresses[0]);
  
  // Format : !RESERVED:slot:uid#
  if (!myRadio.write(&send_data_nrf, sizeof(send_data_nrf))) {
    Serial.println("Don't send to NRF");
  }
  myRadio.openReadingPipe(1, addresses[0]);
  myRadio.startListening();
  // clear data;  
  for(int i = 0; i< 30; i++){
    send_data_nrf.text[i] = '\0';
  }
  delay(100);
}

void Receive_Data_From_ESP(void){
    /* Received data from ESP8266 */
  while(Arduino_softSerial.available()) {
    receive_data_esp = Arduino_softSerial.readStringUntil('\n');
    receive_data_esp.remove(receive_data_esp.length() - 1, 1);
    Serial.println("Du lieu nhan tu ESP8266");
    // Format : !RESERVED:slot:uid#
    // String data = String(receive_data_nrf.text); 
    Serial.println(receive_data_esp);
    // arr = parse_command(data);
    comparser_command_data(receive_data_esp);
    receive_data_esp = ""; 
  }
}

void Transmit_Data_To_ESP(void){
  send_data_esp = String(receive_data_nrf.text);
  Arduino_softSerial.println(send_data_esp);
  Serial.println("Gui du lieu toi ESP8266");
   // Format : !UID:String# or !SLOT:slot:status#  
  // String data = String(send_data_esp); 
  // Serial.println(data);
  Serial.println(send_data_esp);
  send_data_esp = "";
}


void loop() {
  // Serial.println("abc");
  // comparser_command_data("!UID:100 15 20 30#");
  // comparser_command_data("!SLOT:2:1#"); 
  // comparser_command_data("!RESERVED:2:10 78 26 99#");  

  // for (int i = 0; i < 4 ; i++){
  //     status_slot[i] = count % 3;
  //     count++;
  //     displayLCD();
  //     delay(1000);
  // }
  // lcd.blink();
  readRfid();
  printRfid();

  Receive_Data_From_NRF();
  Receive_Data_From_ESP();

  if (check_if_new_id(UID) == 0 && flag_closed_barrier == 1){// quet the va di vao
    open_barrier();
    flag_closed_barrier = 0;        
  }

  if (hc.dist() > 5 && flag_closed_barrier == 0){ // thay xe da di vao thi dong cong 
      close_barrier();
      flag_closed_barrier = 1;
  }
  
  if (hc.dist() > 5 && flag_closed_barrier == 1){ // di tu trong ra va phat hien co xe 
      open_barrier();
      flag_closed_barrier = 0;
  }

  if (hc.dist() < 5 && flag_closed_barrier == 0 ) { // Xe di ra ngoai roi thi dong cong
      close_barrier();
      flag_closed_barrier = 1;
  }
}

void readRfid()
{
  if (rfid.isCard())
  {
    if (rfid.readCardSerial())
    {
      for (int i=0; i<4; i++)//card value: "xyz xyz xyz xyz xyz" (15 digits maximum; 5 pairs of xyz)hence 0<=i<=4 //
      {
        RFID = rfid.serNum[i];
        cardNum += RFID; // store RFID value into string "cardNum" and concatinate it with each iteration
        UID[i] = rfid.serNum[i];
      }
    }
  }
  rfid.halt();
}

void printRfid()
{
 if (cardNum != '\0')//if string cardNum is not empty, print the value
 {
    Serial.println("Card found");
    Serial.print("Cardnumber: ");
    for (int i=0; i<4; i++){
      Serial.println(UID[i]);
    }
    Serial.println(cardNum);
    cardNum.remove(0);
  //This is an arduino function.
  //remove the stored value after printing. else the new card value that is read
  // will be concatinated with the previous string.
  delay(500); 
 }
}
