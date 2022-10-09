#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h> //!!
#include <Fonts/FreeSans24pt7b.h>
#include <Fonts/FreeSans18pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#define FONT_24 FreeSans24pt7b
#define FONT_18 FreeSans18pt7b
#define FONT_12 FreeSans12pt7b
//pins
#define RELAY_1 4
#define RELAY_2 0
#define RELAY_3 2
#define ONE_WIRE_BUS 15

#define ROTARY_PINA 19
#define ROTARY_PINB 18
#define ROTARY_PINSW 5

portMUX_TYPE gpioMux = portMUX_INITIALIZER_UNLOCKED;

//#include <OneWire.h>
//#include <DallasTemperature.h>
//OneWire oneWire(ONE_WIRE_BUS);// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
//DallasTemperature sensors(&oneWire);// Pass our oneWire reference to Dallas Temperature.

#define S_W 128 // OLED display width, in pixels
#define S_H 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C //0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(S_W, S_H, &Wire, OLED_RESET);

#define TEMP_DELTA 1.5 // temperature difference for heater activation
float tmp_int = 80.5; //-!-
float tmp_off = 70;   //-!-

enum Pow_level {OFF, ONE, TWO, TREE};
Pow_level power = ONE; //-!-
Pow_level pow_max = TWO; //-!-

enum Programm {STANDBY, ON, ON_LOW_POW, ERROR_PROGRAMM};
Programm programm = STANDBY; //-!-

enum Page {INFO, SETTING, ERROR_PAGE};
Page page = INFO; //-!-

enum Settings {TEMPERATURE, PROGRAMM, MAX_POW, WIFI, WEB_SERVER, ERRORS};
Settings setting = MAX_POW; //-!-
bool editSetting = false; //-!-?

enum Errors {ERR_b, ERR_a}; //tmp
//Errors error;

bool wifi_on = false; //-!-
bool web_server_on = false; //-!-

enum Controll {CLK, ACLK, CLICK, LONG_CLICK, TIME_OUT};

//encoder and switch
long int millisLastEncoderChange =  0;
long int millisLastSwPress =  0;
uint8_t stateEncoder = 0;
bool switchPressed = 0;
int rotValueEncoder = 0, swNTimesPressed = 0, lastRotValueEncoder = 0, lastSwNTimesPressed = 0;


//--Functions--
//void TaskDisplay( void *pvParameters );
void draw();
void DrawInfo();
void DrawSetting();
void navigate(Controll cont);
void IRAM_ATTR isr_rotary_encoder();
void IRAM_ATTR isrAB();
void IRAM_ATTR isrSWAll();
void input_read();

void setup() {

  // pinMode(ROTARY_ENCODER_CKL, INPUT);
  // pinMode(ROTARY_ENCODER_DT, INPUT);
  // pinMode(ROTARY_ENCODER_SW, INPUT);
  // attachInterrupt(ROTARY_ENCODER_CKL, isr_rotary_encoder, CHANGE);

  pinMode(ROTARY_PINA, INPUT_PULLUP);
  pinMode(ROTARY_PINB, INPUT_PULLUP);
  pinMode(ROTARY_PINSW, INPUT_PULLUP);

  attachInterrupt(ROTARY_PINA, isrAB, CHANGE);
  attachInterrupt(ROTARY_PINB, isrAB, CHANGE);
  attachInterrupt(ROTARY_PINSW, isrSWAll, CHANGE);

  Serial.begin(115200);
  Serial.println(F("Hello"));
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }



  //   xTaskCreatePinnedToCore(
  //      TaskBlink
  //      ,  "TaskDisplay"   // A name just for humans
  //      ,  1024  // This stack size can be checked & adjusted by reading the Stack Highwater
  //      ,  NULL
  //      ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
  //      ,  NULL
  //      ,  ARDUINO_RUNNING_CORE
  //   );
}

void loop() {
  int i = 0;

  while (true) {
    if (i % 10 == 0) {
      Serial.print("isrSWAll ");
      Serial.print(swNTimesPressed);
      Serial.print(" Encoder ");
      Serial.println(rotValueEncoder);
    }


    input_read();
    draw();
    delay(100); // Pause for 2 seconds
  }
}

void input_read() { //to redo
  //critical
  portENTER_CRITICAL_ISR(&gpioMux);
  int rotDifference = rotValueEncoder - lastRotValueEncoder;
  int swdifference = swNTimesPressed - lastSwNTimesPressed;
  bool longPress = (switchPressed && millisLastSwPress + 1000 < millis());
  lastRotValueEncoder = rotValueEncoder;
  lastSwNTimesPressed = swNTimesPressed;
  portEXIT_CRITICAL_ISR(&gpioMux);
  //end critical

  for (int i = 0; i < abs(rotDifference / 2); i++) {
    navigate((rotDifference > 0) ? CLK : ACLK);
  }

  for (int i = 0; i < swdifference; i++) {
    navigate(CLICK);
  }

  if (longPress) {
    navigate(LONG_CLICK);
  }
}


void IRAM_ATTR isrSWAll() {
  long int encoderMillis =  millis();
  portENTER_CRITICAL_ISR(&gpioMux);
  if (millisLastEncoderChange + 20 <  encoderMillis && digitalRead(ROTARY_PINSW) == switchPressed) { // lo stato è cambiato e è passato abbastanza tempo
    if (switchPressed) {
      switchPressed = false;
      if (millisLastSwPress + 1000 > encoderMillis) { //ignore long press
        swNTimesPressed++;
      }
    } else {
      switchPressed = true;
      millisLastSwPress = encoderMillis;
    }
    millisLastEncoderChange = encoderMillis;
  }
  portEXIT_CRITICAL_ISR(&gpioMux);

}

void IRAM_ATTR isrAB() {
  uint8_t s = stateEncoder & 3;

  portENTER_CRITICAL_ISR(&gpioMux);
  if (digitalRead(ROTARY_PINA)) s |= 4;
  if (digitalRead(ROTARY_PINB)) s |= 8;
  switch (s) {
    case 0: case 5: case 10: case 15:
      break;
    case 1: case 7: case 8: case 14:
      rotValueEncoder++; break;
    case 2: case 4: case 11: case 13:
      rotValueEncoder--; break;
    case 3: case 12:
      rotValueEncoder += 2; break;
    default:
      rotValueEncoder -= 2; break;
  }
  stateEncoder = (s >> 2);
  portEXIT_CRITICAL_ISR(&gpioMux);

}



//void heaterControll(){
//  //read temperature
////  sensors.requestTemperatures(); // Send the command to get temperatures
//  Serial.println("DONE");
//  // After we got the temperatures, we can print them here.
//  // We use the function ByIndex, and as an example get the temperature from the first sensor only.
// // float tempC = sensors.getTempCByIndex(0);
//
//  // Check if reading was successful
//  if(tempC != DEVICE_DISCONNECTED_C)
//  {
//    Serial.print("Temperature for the device 1 (index 0) is: ");
//    Serial.println(tempC);
//
//    tmp_int = tempC;
//  }
//  else
//  {
//    Serial.println("Error: Could not read temperature data");
//  }
//
//  //controll
//  if(power == OFF){
//    if(tmp_int < tmp_off - TEMP_DELTA){
//      power = pow_max;
//    }
//  }else{
//    if(tmp_int >= tmp_off){
//      power = OFF;
//    }else{
//      power = pow_max;
//    }
//  }
//
//  //relay update
//  switch (power) {
//    case TREE:{
//      digitalWrite(RELAY_3, 1);
//      digitalWrite(RELAY_2, 1);
//      digitalWrite(RELAY_1, 1);
//    }break;
//    case TWO: {
//      digitalWrite(RELAY_3, 1);
//      digitalWrite(RELAY_2, 0);
//      digitalWrite(RELAY_1, 1);
//    }break;
//    case ONE: {
//      digitalWrite(RELAY_3, 0);
//      digitalWrite(RELAY_2, 1);
//      digitalWrite(RELAY_1, 0);
//    }break;
//    case OFF:{
//      digitalWrite(RELAY_3, 0);
//      digitalWrite(RELAY_2, 0);
//      digitalWrite(RELAY_1, 0);
//    }break;
//  }
//
//}

void navigate(Controll cont) {
  switch (page) {
    case INFO: {
        switch (cont) {
          case CLICK: {
              page = SETTING;
              setting = PROGRAMM;
              editSetting = false;
            } break;
          case CLK:
          case ACLK: {
              page = SETTING;
              setting = TEMPERATURE;
            } break;
          case LONG_CLICK: {} break; //!!OFF
          case TIME_OUT: {} break;
        }
      } break;
    case SETTING: {
        if (cont == TIME_OUT || cont == LONG_CLICK) {
          page = INFO;
          return;
        }
        if (setting == TEMPERATURE) {
          switch (cont) {
            case CLK: {
                tmp_off += 0.5;
              } break;
            case ACLK: {
                tmp_off -= 0.5;
              } break;
            case CLICK: {
                page = INFO;
              } break;
          }
          return;
        }

        if (cont == CLK) {
          editSetting = !editSetting;
          return;
        }
        switch (setting) {
          case PROGRAMM: {
              if (editSetting) {
                switch (programm) {
                  case STANDBY: {
                      switch (cont) {
                        case CLK: {programm = ON; }break;
                        case ACLK: { programm = ON_LOW_POW;}  break;
                      }
                    } break;
                  case ON: {
                      switch (cont) {
                        case CLK: { programm = ON_LOW_POW;}  break;
                        case ACLK: { programm = STANDBY;}  break;
                      }
                    } break;
                  case ON_LOW_POW: {
                      switch (cont) {
                        case CLK: { programm = STANDBY;}  break;
                        case ACLK: { programm = ON;}  break;
                      }
                    } break;
                  case ERROR_PROGRAMM: { //!!
                      switch (cont) {
                        case CLK: { programm = STANDBY;}  break;
                        case ACLK: { programm = ON_LOW_POW;}  break;
                      }
                    } break;
                }
              } else {
                switch (cont) {
                  case CLK: { setting = MAX_POW;}  break;
                  case ACLK: { setting = ERRORS;}  break;
                }
              }
            } break;
          case MAX_POW: {
              if (editSetting) {
                switch (pow_max) {
                  case OFF: {
                      if (cont == CLK) {
                        pow_max = ONE;
                      }
                    } break;
                  case ONE: {
                      switch (cont) {
                        case CLK: { pow_max = TWO;}  break;
                        case ACLK: { pow_max = TREE;}  break;
                      }
                    } break;
                  case TWO: {
                      switch (cont) {
                        case CLK: { pow_max = TREE;}  break;
                        case ACLK: { pow_max = ONE;}  break;
                      }
                    } break;
                  case TREE: {
                      if (cont == ACLK) {
                        pow_max = TWO;
                      }
                    } break;
                }

              } else {
                switch (cont) {
                  case CLK: { setting = WIFI;}  break;
                  case ACLK: { setting = PROGRAMM;}  break;
                }
              }

            } break;
          case WIFI: {
              if (editSetting) {
                wifi_on = !wifi_on;
              } else {
                switch (cont) {
                  case CLK: { setting = WEB_SERVER;}  break;
                  case ACLK: { setting = MAX_POW;}  break;
                }
              }
            } break;
          case WEB_SERVER: {
              if (editSetting) {
                web_server_on = !web_server_on;
              } else {
                switch (cont) {
                  case CLK: { setting = ERRORS;}  break;
                  case ACLK: { setting = WIFI;}  break;
                }
              }
            } break;
          case ERRORS: {
              switch (cont) {
                case CLK: { setting = PROGRAMM;}  break;
                case ACLK: { setting = WEB_SERVER;}  break;
              }
            } break;
        }
      } break;
    case ERROR_PAGE: {
        // da implm
      } break;
  }
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


  display.display(); // Show the display buffer on the screen
}
