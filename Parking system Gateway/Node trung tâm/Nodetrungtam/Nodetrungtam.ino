#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#include <Arduino.h>

#include <SoftwareSerial.h>

#include <Wire.h> //Gọi thư viện I2C để sử dụng các thư viện I2C

#include <LiquidCrystal_I2C.h> //Thư viện LCD I2C

#include <HCSR04.h>

#include <SPI.h>
#include <MFRC522.h>

#include "user_Slot.h"

#define RST_PIN         9
#define SS_PIN          10

RF24 radio(7,8);// 7 : CE and 8 : CSN
const byte Address[6][6] = {"PKIOT1", "PKIOT2", "PKIOT3" }; // Node 0(trungtam) : Node 1 : Node 2

SoftwareSerial Arduino_softSerial(5, 6); // RX: 5 - TX: 6

HCSR04 hc(2, 4); // //initialisation class HCSR04 (trig pin , echo pin)

LiquidCrystal_I2C lcd(0x3F,16,2); //Khai báo địa chỉ I2C (0x27 or 0x3F) và LCD 16x02

MFRC522 mfrc522(SS_PIN, RST_PIN);
int UID[4], i;

String send_data;
String receive_data;

void setup() {
  Serial.begin(9600);
  
  // if (!radio.begin()){
  //   Serial.println("Module isn't connected ...!!");
  //   while (1){}
  // }
  // radio.openWritingPipe(Address[0]); // Only write on a channel 
  //   // Open a channel to write on address"PKIOT0" 
  // radio.openReadingPipe(1,Address[1]); // Open a channel to read on address"PKIOT1" (Node 1)
  // radio.openReadingPipe(2,Address[2]); // Open a channel to read on address"PKIOT2" (Node 2)


  // radio.setPALevel(RF24_PA_MIN); //Cài bộ khuyết địa công suất ở mức MIN
  // radio.setChannel(80);
  // radio.setDataRate(RF24_250KBPS);    

  // if (!radio.available())
  // {
  //   Serial.println("Chưa kết nối được với Node con...!!");
  //   Serial.println("CHỜ KẾT NỐI.......");
  // }

  /////////////////
  Arduino_softSerial.begin(9600);
  
  pinMode(servo_checkin, OUTPUT);
  servo.write(0);

  /* setup for LCD*/
  lcd.begin(); //Khởi tạo màn hình LCD
  lcd.backlight(); //Bật đèn màn hình lCD  

  /* setup for RFID*/
  SPI.begin();    
  mfrc522.PCD_Init();
  
  
}

void loop() {

  // radio.stopListening();
  // /////

  // /////
  // radio.write(&send_data , sizeof(send_data));
  // delay(10);

  // radio.startListening();
  // while (!radio.available());
  // radio.read(&receive_data , sizeof(receive_data));
  // //////

  // //////
  // delay(10);

  send_data = "01,01";
  Arduino_softSerial.println(send_data);
  Serial.println("Gui du lieu toi ESP8266");  
  Serial.println(send_data);
  send_data = "";
  
  
  /* Received data from ESP8266 */
  while(Arduino_softSerial.available()) {
    receive_data = Arduino_softSerial.readStringUntil('\n');
    receive_data.remove(receive_data.length() - 1, 1);
    Serial.println("Du lieu nhan tu ESP8266"); 
    Serial.println(receive_data);
    receive_data = "";    
  }
  delay(1000);

  /*
  lcd.setCursor(0, 0); //Chọn vị trí đặt con trỏ. 0 đầu là chọn cột (từ 0 - 16), 0 kế là chọn hàng (có 2 hàng 0 và 1)
  lcd.print("Green Technology"); //in lên lcd chữ tỏng dấu ""
  delay(5000); // chờ 5giay
  lcd.clear(); //Xóa LCD
  //lcd.noDisplay(); //Tắt LCD
  delay(1000);
  
  //lcd.display(); //Mở LCD
  lcd.setCursor(1, 1); // Chọn cột 1 và hàng 1
  lcd.print("I LOVE ARDUINO");
  delay(5000);
  //lcd.noDisplay();
  lcd.clear();
  delay(1000);

  //lcd.display();
  lcd.setCursor(0, 0);
  lcd.print("Green Technology");
  lcd.setCursor(1, 1);
  lcd.print("I LOVE ARDUINO");
  delay(5000);
  //lcd.noDisplay();
  lcd.clear();
  delay(1000); 
  */

  Serial.println( hc.dist() ); //return current distance (cm) in serial
  delay(60);                   // we suggest to use over 60ms measurement cycle, in order to prevent trigger signal to the echo signal.

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
  
  mfrc522.PICC_HaltA();  
  mfrc522.PCD_StopCrypto1();

}
