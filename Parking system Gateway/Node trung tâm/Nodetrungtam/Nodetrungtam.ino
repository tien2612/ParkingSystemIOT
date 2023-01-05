#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
// #include <DigitalIO.h>

#include <Arduino.h>

#include <Servo.h>

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
# define SERVO_PIN     5

RF24 myRadio(CE_PIN,CSN_PIN);// 7 : CE and 8 : CSN
// const byte addresses[6][6] = {"PKIOT0","PKIOT1", "PKIOT2"  }; // Node 0(trungtam) : Node 1 : Node 2

const byte addresses[6] = {'x','y','z'};

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

unsigned long timer0 = 0;
unsigned long timer1 = 0;

// int count = 0;
void restoreDataFromEEPROM() {
  for (int i = 0; i< 4; i++){
    status_slot[i] = EEPROM.read(address_number_of_users+i);
  }
}

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
      eepromWrite(address_status,&status_slot[0],4);
      delay(50);
  
  } else if (data_split[0] == RESERVED_Reg) {// Format : !RESERVED:slot:uid#
      int slot = data_split[1].toInt();
      // Serial.println(slot);   
      
      // update_user_ID(slot, data_split[2]);
      // Serial.println(data_split[2]);        
      status_slot[slot] = 2;
      updateDisplayLCD(slot);      
      Transmit_Data_To_NRF();
      Feedback_ESP(slot);      
    }
}

void GetDistance(void){
    if (millis() - timer0 > 1000){
        timer0 = millis();
        int distance = hc.dist();
        if (distance <= 5){
          detected_car = 1;
          // Serial.println(distance);       
          // Serial.println("Detected Car");
        } else {
          detected_car = 0;
          // Serial.println(distance);  
          // Serial.println("Khong phat hien Car");
        }
        delay(100);
    // Serial.println("Open ");
  }
}


void setup() {
  Serial.begin(115200);
    
  Serial.println("initilizing RFID...");
  rfid.init(); // initilize the RFID module
  Serial.println("start ");

  myRadio.begin(); 
  myRadio.setAutoAck(true); 
  myRadio.setRetries(15, 15);
  // myRadio.setChannel(80); 
  myRadio.setPALevel(RF24_PA_MAX);
  myRadio.setDataRate( RF24_2MBPS );
  
  myRadio.openWritingPipe(addresses[0]);
  myRadio.openReadingPipe(1, addresses[1]);
  myRadio.openReadingPipe(2, addresses[2]);
  myRadio.startListening();

  /////////////////
  Arduino_softSerial.begin(9600);
  
  servo.attach(SERVO_PIN);
  servo.write(90);

  /* setup for LCD*/
  setupLCD();

  // for(int i=0; i< EEPROM.length(); i++){
  //   EEPROM.write(i,0);
  // }  
  
  // restoreDataFromEEPROM();
  // for (int i = 0; i< 30 ; i++){    
  //   Serial.println(EEPROM.read(i));
  // }
}

void Receive_Data_From_NRF(void){
  if ( myRadio.available()) {
    while (myRadio.available()){
      // Serial.println("Da nhan du lieu"); // Format : !UID:String# or !SLOT:slot:status#  
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
  // delay(10);
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
  // myRadio.openWritingPipe(addresses[0]);
  // myRadio.setRetries(15,15);
  
  // Format : !RESERVED:slot:uid#
  if (!myRadio.write(&send_data_nrf, sizeof(send_data_nrf))) {
    Serial.println("Don't send to NRF");
  } else { Serial.println("Send to NRF successfull");}
  // myRadio.openReadingPipe(1, addresses[0]);
  // myRadio.openReadingPipe(1, addresses[2]);  
  
  myRadio.startListening();
  // clear data;  
  for(int i = 0; i< 30; i++){
    send_data_nrf.text[i] = '\0';
  }
  // delay(10);
}

void Receive_Data_From_ESP(void){
    /* Received data from ESP8266 */
        // if ( myRadio.available()) {    
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
  for(int i = 0; i< 30; i++){
    receive_data_nrf.text[i] = '\0';
  }
}

void Feedback_ESP(int slot){
    send_data_esp = "!OK"+ String(slot+1) +  + "#";
    Arduino_softSerial.println(send_data_esp);
    Serial.println(send_data_esp);
    Serial.println("Phan hoi toi ESP8266 đã nhận đặt Slot" + String(slot + 1));
    send_data_esp = "";
}

void loop() {
  // comparser_command_data("!UID:100 15 20 30#");
  // comparser_command_data("!SLOT:2:1#"); 
  // comparser_command_data("!RESERVED:2:10 78 26 99#"); 
  
  readRfid();
  printRfid();
  GetDistance();

    // if (millis() - timer0 > 1000){
    //   timer0 = millis();
    //   servo.write(0);
    //   Serial.println("Open ");
    //   // Serial.println(hc.dist());
    // }

    // if (millis() - timer1 > 2000){
    //   timer1 = millis();
    //   servo.write(90);
    //   Serial.println("Close ");
    // }

  Receive_Data_From_NRF();
  Receive_Data_From_ESP();

  if (check_if_new_id(UID) == 0 && flag_closed_barrier == 1 && detected_car == 0 //&& get_in_car == 1 
  && is_swiped == 0 ){// quet the va xe di vao
    Serial.print("Distance: ");
    Serial.println(hc.dist());
    Serial.println("1 - open ");  
    open_barrier();
    flag_closed_barrier = 0; 
    get_in_car = 1 ;
    is_swiped = 1;
    for (int i = 0; i<4;i++){
      UID[i] = -1;      
    }
    // delay(500);
  }
  else if (detected_car == 1 && flag_closed_barrier == 0 //&& get_in_car == 1 
  && is_swiped == 1){ // thay xe da di vao thi dong cong 
    Serial.print("Distance: ");
    Serial.println(hc.dist());
    Serial.println("2 - Close ");
      close_barrier();
      flag_closed_barrier = 1;
    // delay(500);

  } else if (detected_car == 0 && flag_closed_barrier == 1 //&& get_in_car == 1 
  &&  is_swiped == 1 ){
      Serial.println("Car get in completed");
      get_in_car = 0 ;
      is_swiped = 0;
      // delay(500);
  }
  else if (detected_car == 1 && flag_closed_barrier == 1 //&& get_in_car == 0 
  &&  is_swiped == 0) { // di tu trong ra va phat hien co xe 
      Serial.print("Distance: ");
      Serial.println(hc.dist());
      Serial.println("3 - open ");  
      open_barrier();
      flag_closed_barrier = 0;
      get_in_car = 0;
    // delay(5000);
  }
  else if (detected_car == 0 && flag_closed_barrier == 0 //&& get_in_car == 0 
  &&  is_swiped == 0) { // Xe di ra ngoai roi thi dong cong
        Serial.print("Distance: ");
        Serial.println(hc.dist());
        Serial.println("4 - Close ");  
        close_barrier();
        flag_closed_barrier = 1;
        get_in_car = 1;
    // delay(500);
  }
  //  lcd.setCursor(0, 0);
  //  lcd.print("0:EMPTY 1:FULL  ");
  // lcd.setCursor(0, 1);
  //  lcd.print("2:RESER 3:FULL  ");   
}

void readRfid()
{
  // Serial.println("Card found");
  if (rfid.isCard())
  {
    // Serial.println("Card found");
    if (rfid.readCardSerial())
    {
      
      for (int i=0; i<4; i++)//card value: "xyz xyz xyz xyz xyz" (15 digits maximum; 5 pairs of xyz)hence 0<=i<=4 //
      {
        RFID = rfid.serNum[i];
        cardNum += RFID; // store RFID value into string "cardNum" and concatinate it with each iteration
        UID[i] = rfid.serNum[i];
      }
      // if (check_if_new_id(&UID[0]) == 1){  
      //     store_new_UID_into_EEPROM(&UID[0]);      
      // }
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
    // for (int i=0; i<4; i++){
    //   Serial.println(UID[i]);
    // }
    Serial.println(cardNum);
    cardNum.remove(0);
  //This is an arduino function.
  //remove the stored value after printing. else the new card value that is read
  // will be concatinated with the previous string.
  delay(500); 
 }
}
