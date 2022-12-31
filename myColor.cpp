#include "myColor.h"

/* Struct of array contains color of led */
ledColor led_color[3];

int red = A0;
int green = A1;
int blue = A2;
int en1 = A3;
int en2 = A4;
//int en3 = 2;

int color_En1 = GREEN_COLOR;
int color_En2 = GREEN_COLOR;
//int color_En3 = YELLOW_COLOR;

/* Update color of ledRGB controlled by en */
void updateColorIndex(int &en, int color) {
  en = color;
  // int addr_temp = 0;
  // if (en == color_En1) addr_temp = address_color_of_slot[0];
  // else if (en == color_En2) addr_temp = address_color_of_slot[1];

  // EEPROM.write(addr_temp, color);
}
/* Initiate the corresponding three colors */
void initLED() {
  for (int i = 0; i < 3; i++) {
    led_color[RED_COLOR].color[0] = 1;
    led_color[RED_COLOR].color[1] = 0;
    led_color[RED_COLOR].color[2] = 0;

    led_color[GREEN_COLOR].color[0] = 0;
    led_color[GREEN_COLOR].color[1] = 1;
    led_color[GREEN_COLOR].color[2] = 0;

    led_color[YELLOW_COLOR].color[0] = 1;
    led_color[YELLOW_COLOR].color[1] = 1;
    led_color[YELLOW_COLOR].color[2] = 0;
  }
}
/* Make color for LED */
void writeLed(int color) {
    digitalWrite(red,   led_color[color].color[0]);
    digitalWrite(green, led_color[color].color[1]);
    digitalWrite(blue,  led_color[color].color[2]);
}

/* Make color for each index of LED */
void ledRGB(int index, int color_en1, int color_en2) {
  switch(index) {
    case 0:
      digitalWrite(en1, 0);
      digitalWrite(en2, 1);
     // digitalWrite(en3, 1);
      writeLed(color_en1);
      break;
    case 1:
      digitalWrite(en1, 1);
      digitalWrite(en2, 0);
      //digitalWrite(en3, 1);
      writeLed(color_en2);
      break;
    // case 2:
    //   digitalWrite(en1, 1);
    //   digitalWrite(en2, 1);
    //   digitalWrite(en3, 0);
    //   writeLed(color_en3);
    //   break;
    default:
      break;
  } 
}