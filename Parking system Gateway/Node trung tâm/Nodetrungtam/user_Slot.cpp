#include "user_Slot.h"

Servo servo;

LiquidCrystal_I2C lcd(0x3F,16,2);

int status_slot[N0_NODE_CAR * N0_SLOT_IN_NODE];

void open_barrier(){
  servo.write(90);
}

void close_barrier(){
  servo.write(0);
}

void displayLCD(){
  // lcd.setCursor(0, 0); //Chọn vị trí đặt con trỏ. 0 đầu là chọn cột (từ 0 - 16), 0 kế là chọn hàng (có 2 hàng 0 và 1)
  // lcd.print("1.FULL"); //in lên lcd chữ tỏng dấu ""
  // lcd.print("1.EMPTY"); 
  // lcd.print("1.RESERV");
  String display = "";
  for (int i = 0 ; i < 4; i++){
    display += String(i) + "-";
      switch(i){
        case 0: // slot 1
        lcd.setCursor(0, 0);   
        break;   
        case 1: // slot 2
        lcd.setCursor(7, 0);   
        break;  
        case 2: // slot 3
        lcd.setCursor(0, 1);   
        break;               
        case 4: // slot 4
        lcd.setCursor(7, 1);   
        break;                      
      }
      
      switch(status_slot[i]){
        case 0: // red 
        display += "FULL";
        break;   
        case 1: // green
        display += "EMPTY"; 
        break;  
        case 2: // Yellow
        display += "RESERV";  
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