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

bool initDispaly() {
  return display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
}

void draw() {
  switch (page) {
    case INFO: {
        DrawInfo();
      } break;
    case SETTING: {
        DrawSetting();
      } break;
    case ERROR_PAGE: {
        // da implm
      } break;
  }
}

void DrawSetting() {
  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);
  display.setFont(&FONT_12);
  display.setTextSize(1);
  display.setCursor(4, 24);

  switch (setting) {
    case TEMPERATURE: {
        display.print(F("Temp"));
        display.setCursor(4, S_H - 5);
        display.print(tmp_off);
        display.print(F("°C"));
      } break;
    case PROGRAMM: {
        display.print(F("Programma"));
        display.setCursor(4, S_H - 5);
        switch (programm) {
          case STANDBY: {
              display.print(F("Standby"));
            } break;
          case ON: {
              display.print(F("ON"));
            } break;
          case ON_LOW_POW: {
              display.print(F("ECO"));
            } break;
          case ERROR_PROGRAMM: {
              display.print(F("ERROR"));
            } break;
        }
      } break;
    case MAX_POW: {
        display.print(F("Lim Pot"));
        display.setCursor(4, S_H - 5);
        switch (pow_max) {
          case OFF: {
              display.print(F("0 KW"));
            } break;
          case ONE: {
              display.print(F("1.5 KW"));
            } break;
          case TWO: {
              display.print(F("3  KW"));
            } break;
          case TREE: {
              display.print(F("4.5 KW"));
            } break;
        }
      } break;
    case WIFI: {
        display.print(F("WiFi"));
        display.setCursor(4, S_H - 5);
        display.print((wifi_on) ? F("ON") : F("OFF"));
      } break;
    case WEB_SERVER: {
        display.print(F("Web Server"));
        display.setCursor(4, S_H - 5);
        display.print((web_server_on) ? F("ON") : F("OFF"));
      } break;
    case ERRORS: {
        display.print(F("Error"));
      } break;
  }

  if (editSetting) {
    display.print(F(" #"));
  }

  display.display(); // Show the display buffer on the screen
}

void DrawInfo() {

  display.clearDisplay();

  for (int i = 0; i < 3; i++) {
    display.fillRoundRect(
      S_W - 6, (i * (S_H / 3)),
      6, ((S_H / 3) - 5),
      2,
      SSD1306_WHITE
    );
  }
  // draw current power line
  for (int i = 0; i < ((power - 3) * -1); i++) {
    display.fillRoundRect(
      S_W - 5, (i * (S_H / 3)) + 1,
      4, ((S_H / 3) - 5) - 2,
      2,
      SSD1306_BLACK
    );
  }

  //draw max power triangle
  int yPosT = 0;
  switch (pow_max) {
    case OFF : yPosT = S_H - 4; break;
    case ONE : yPosT = 41; break;
    case TWO : yPosT = 20; break;
    case TREE : yPosT = 3; break;
  }

  display.fillTriangle(
    110  , yPosT - 3,
    110  , yPosT + 3,
    117  , yPosT,
    SSD1306_WHITE
  );

  //draw current temperature
  display.setFont(&FONT_24);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, S_H - 5);
  int tmp_integer = tmp_int;
  int tmp_decimal = (tmp_int - tmp_integer) * 10;
  display.print(tmp_integer);
  display.setFont(&FONT_12);
  display.setTextSize(1);
  //display.setCursor(90,S_H-5);
  display.print(".");
  display.print(tmp_decimal);
  display.print(F("°C"));


  display.display(); // Show the display buffer on the screen
}
#endif
