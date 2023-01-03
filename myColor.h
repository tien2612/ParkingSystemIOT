#ifndef myColor_h
#define myColor_h
#include "Arduino.h"
#include "eeprom.h"

#define RED_COLOR     0
#define YELLOW_COLOR  1
#define GREEN_COLOR   2

/* Define pinout */
extern int red ;
extern int green;
extern int blue;
extern int en1;
extern int en2;
extern int en3;

extern int color_En1;
extern int color_En2;
//extern int color_En3;

/* Index of led RGB */
extern int index;

struct ledColor {
  int color[3];
};

/* Update color of ledRGB controlled by en */
void updateColorIndex(int &en, int color);

/* Initiate the corresponding three colors */
void initLED();
/* Make color for LED */
void writeLed(int color);

/* Make color for each index of LED */
void ledRGB(int index, int color_en1, int color_en2);

/* Updating all led by switching sequentially */ 
void scan_led();
#endif
