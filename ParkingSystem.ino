#include <Arduino.h>

#define RED_COLOR     0
#define YELLOW_COLOR  1
#define GREEN_COLOR   2

/* Define pinout */
int red = 7;
int green = 6;
int blue = 5;
int en1 = 4;
int en2 = 3;
int en3 = 2;
/* Index of led RGB */
int i = 0; 
int color_En1 = RED_COLOR;
int color_En2 = GREEN_COLOR;
int color_En3 = YELLOW_COLOR;

/* Struct of array contains color of led */
struct ledColor {
  int color[3];
};

ledColor led_color[3];

/* Update color of ledRGB controlled by en */
void updateColorIndex(int &en, int color) {
  en = color;
}
/* Initiate the corresponding three colors */
void initLED() {
  for (int i = 0; i < 3; i++) {
    led_color[RED_COLOR].color[0] = 0;
    led_color[RED_COLOR].color[1] = 1;
    led_color[RED_COLOR].color[2] = 1;

    led_color[GREEN_COLOR].color[0] = 1;
    led_color[GREEN_COLOR].color[1] = 0;
    led_color[GREEN_COLOR].color[2] = 1;

    led_color[YELLOW_COLOR].color[0] = 0;
    led_color[YELLOW_COLOR].color[1] = 0;
    led_color[YELLOW_COLOR].color[2] = 1;
  }
}
/* Make color for LED */
void writeLed(int color) {
    digitalWrite(red,   led_color[color].color[0]);
    digitalWrite(green, led_color[color].color[1]);
    digitalWrite(blue,  led_color[color].color[2]);
}

/* Make color for each index of LED */
void ledRGB(int index, int color_en1, int color_en2, int color_en3) {
  switch(index) {
    case 0:
      digitalWrite(en1, 0);
      digitalWrite(en2, 1);
      digitalWrite(en3, 1);
      writeLed(color_en1);
      break;
    case 1:
      digitalWrite(en1, 1);
      digitalWrite(en2, 0);
      digitalWrite(en3, 1);
      writeLed(color_en2);
      break;
    case 2:
      digitalWrite(en1, 1);
      digitalWrite(en2, 1);
      digitalWrite(en3, 0);
      writeLed(color_en3);
      break;
  }
 
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(en1, OUTPUT);
  pinMode(en2, OUTPUT);
  pinMode(en3, OUTPUT);

  initLED();
}

void loop() {
  // put your main code here, to run repeatedly:
  ledRGB(i, color_En1, color_En2, color_En3);
  // update color of ledRGB controlled by en1
  updateColorIndex(color_En1, GREEN_COLOR);
  if (i >= 2) i = 0;
  else i++;
  delay(10);
}
