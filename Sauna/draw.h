#ifndef _DRAW_
#define _DRAW_

#include "shared.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <Fonts/FreeSans24pt7b.h>
#include <Fonts/FreeSans18pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#define FONT_24 FreeSans24pt7b
#define FONT_18 FreeSans18pt7b
#define FONT_12 FreeSans12pt7b

#define S_W 128 // OLED display width, in pixels
#define S_H 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C //0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(S_W, S_H, &Wire, OLED_RESET);

void draw();
void DrawInfo();
void DrawSetting();

#endif
