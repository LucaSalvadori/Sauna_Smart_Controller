#include "draw.hpp"

bool initDispaly() {
  
  I2C_1.begin(21, 22);
  display = Adafruit_SSD1306(S_W, S_H, &I2C_1, OLED_RESET);
  
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
        display.setCursor(4, S_H - 8);
        display.print((int) tmp_off);
        display.print(F(" c"));
      } break;
    case PROGRAMM: {
        display.print(F("Programma"));
        display.setCursor(4, S_H - 8);
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
        display.setCursor(4, S_H - 8);
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
        display.setCursor(4, S_H - 8);
        display.print((wifi_on) ? F("ON") : F("OFF"));
      } break;
    case WEB_SERVER: {
        display.print(F("Web Server"));
        display.setCursor(4, S_H - 8);
        display.print((web_server_on) ? F("ON") : F("OFF"));
      } break;
    case ERRORS: {
        display.print(F("Error"));
      } break;
  }

  if (editSetting) {
    display.setCursor(S_W - 25, S_H - 8);
    display.print(F("#"));
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

  
  bool tmp_or_hum = (millis() >> 12) % 2 == 0 ;
  int integer;
  int decimal;
  if(tmp_or_hum){
    integer = tmp_int;
    decimal = (tmp_int - integer) * 10;
  }else{
    integer = hum;
    decimal = (hum - integer) * 10;
  }
  

  display.print(integer);
  display.setFont(&FONT_12);
  display.setTextSize(1);
  //display.setCursor(90,S_H-5);
  display.print(".");
  display.print(decimal);

  display.print(tmp_or_hum ? F(" c") : F(" %"));


  display.display(); // Show the display buffer on the screen
}
