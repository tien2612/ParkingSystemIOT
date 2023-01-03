#include "HardwareSerial.h"
#include "user_Slot.h"

Servo servo;

LiquidCrystal_I2C lcd(0x3F,16,2);

bool flag_closed_barrier = 0;

int status_slot[N0_NODE_CAR * N0_SLOT_IN_NODE] = {0};

void open_barrier(){
  servo.write(90);
}

void close_barrier(){
  servo.write(0);
}

void setupLCD(void){
  lcd.begin(); //Khởi tạo màn hình LCD
  lcd.backlight(); //Bật đèn màn hình lCD 
  displayLCD();   
  Serial.println("Set up LCD");
}

void displayLCD(){
  // lcd.setCursor(0, 0); //Chọn vị trí đặt con trỏ. 0 đầu là chọn cột (từ 0 - 16), 0 kế là chọn hàng (có 2 hàng 0 và 1)
  // lcd.print("1.FULL"); //in lên lcd chữ tỏng dấu ""
  // lcd.print("1.EMPTY"); 
  // lcd.print("1.RESERV");
  String display = "";
  for (int i = 1 ; i < 5; i++){
    display += String(i) + ":";
      switch(i){
        case 1: // slot 1
        lcd.setCursor(0, 0);   
        break;   
        case 2: // slot 2
        lcd.setCursor(8, 0);   
        break;  
        case 3: // slot 3
        lcd.setCursor(0, 1);   
        break;               
        case 4: // slot 4
        lcd.setCursor(8, 1);   
        break;                      
      }
      
      switch(status_slot[i-1]){
        case 0: // red  // "1-FULL  "
        display += "FULL  ";
        break;   
        case 1: // green "2-EMPTY "
        display += "EMPTY "; 
        break;  
        case 2: // Yellow //3-RESER ""
        display += "RESER ";  
        break;                                    
      }
      lcd.print(display); 
      display = "";
  }
  // delay(5000); // chờ 5giay
  // lcd.clear(); //Xóa LCD
  // //lcd.noDisplay(); //Tắt LCD
  // delay(1000);
  
  // //lcd.display(); //Mở LCD
  // lcd.setCursor(1, 1); // Chọn cột 1 và hàng 1
  // lcd.print("I LOVE ARDUINO");
  // delay(5000);
  // //lcd.noDisplay();
  // lcd.clear();
  // delay(1000);

  // //lcd.display();
  // lcd.setCursor(0, 0);
  // lcd.print("Green Technology");
  // lcd.setCursor(1, 1);
  // lcd.print("I LOVE ARDUINO");
  // delay(5000);
  // //lcd.noDisplay();
  // lcd.clear();
  // delay(1000); 
}

void updateDisplayLCD(int slot){ 
    String display = "";
    display += String(slot) + ":";
      switch(slot){
        case 1: // slot 1
        lcd.setCursor(0, 0);   
        break;   
        case 2: // slot 2
        lcd.setCursor(8, 0);   
        break;  
        case 3: // slot 3
        lcd.setCursor(0, 1);   
        break;               
        case 4: // slot 4
        lcd.setCursor(8, 1);   
        break;                      
      }
      switch(status_slot[slot-1]){
        case 0: // red  // "1-FULL  "
        display += "FULL  ";
        break;   
        case 1: // green "2-EMPTY "
        display += "EMPTY "; 
        break;  
        case 2: // Yellow //3-RESER ""
        display += "RESER ";  
        break;                                    
      }
      lcd.print(display); 
      display = "";
}