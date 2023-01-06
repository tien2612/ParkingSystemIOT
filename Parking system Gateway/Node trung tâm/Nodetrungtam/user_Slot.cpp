// #include "HardwareSerial.h"
#include "user_Slot.h"

Servo servo;

LiquidCrystal_I2C lcd(0x3F,16,2);

bool flag_closed_barrier = 1;
bool detected_car = 0;
bool get_in_car = 1;
extern bool is_swiped = 0;



int status_slot[N0_NODE_CAR * N0_SLOT_IN_NODE] = {SLOT_EMPTY,SLOT_EMPTY,SLOT_EMPTY,SLOT_EMPTY};

void open_barrier(){
  // Serial.println("Open barrier");
  servo.write(0);
}

void close_barrier(){
  // Serial.println("Close barrier");
  servo.write(90);
}

void setupLCD(void){
  lcd.begin(); //Khởi tạo màn hình LCD
  lcd.backlight(); //Bật đèn màn hình lCD 
  displayLCD();   
  // Serial.println("Set up LCD");
}

void displayLCD(){
  // lcd.setCursor(0, 0); //Chọn vị trí đặt con trỏ. 0 đầu là chọn cột (từ 0 - 16), 0 kế là chọn hàng (có 2 hàng 0 và 1)
  // lcd.print("1.FULL"); //in lên lcd chữ tỏng dấu ""
  // lcd.print("1.EMPTY"); 
  // lcd.print("1.RESERV");
  String display = "";
  for (int i = 0 ; i < 4; i++){
    display += String(i) + ":";
      switch(i){
        case 0: // slot 0
        lcd.setCursor(8, 0);   
        break;   
        case 1: // slot 1
        lcd.setCursor(0, 0);   
        break;  
        case 2: // slot 2
        lcd.setCursor(0, 1);   
        break;               
        case 3: // slot 3
        lcd.setCursor(8, 1);   
        break;                      
      }
      
      switch(status_slot[i]){
        case SLOT_FULL: // red  // "1-FULL  "
        display += "FULL  ";
        break;   
        case SLOT_EMPTY: // green "2-EMPTY "
        display += "EMPTY "; 
        break;  
        case SLOT_REVERSE: // Yellow //3-RESER ""
        display += "RESER ";  
        break;                                    
      }
      lcd.print(display); 
      // Serial.println(display);
      display = "";
  }
}

void updateDisplayLCD(int slot){ 
    String display = "";
    // slot += 1;
    // Serial.println("Update lcd");
    display += String(slot) + ":";
      switch(slot){
        case 0: // slot 1
        lcd.setCursor(8, 0);   
        break;   
        case 1: // slot 2
        lcd.setCursor(0, 0);   
        break;  
        case 2: // slot 3
        lcd.setCursor(0, 1);   
        break;               
        case 3: // slot 4
        lcd.setCursor(8, 1);   
        break;                      
      }
      switch(status_slot[slot]){
        case SLOT_FULL: // red  // "1-FULL  "
        display += "FULL  ";
        break;   
        case SLOT_EMPTY: // green "2-EMPTY "
        display += "EMPTY "; 
        break;  
        case SLOT_REVERSE: // Yellow //3-RESER ""
        display += "RESER ";  
        break;                                    
      }
      lcd.print(display); 
      display = "";
}