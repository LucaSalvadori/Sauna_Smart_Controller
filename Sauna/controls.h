#ifndef _CONTROLS_
#define _CONTROLS_

#include "shared.h"
#include <Arduino.h> //!!


//pins
#define ROTARY_PINA 19
#define ROTARY_PINB 18
#define ROTARY_PINSW 5

portMUX_TYPE gpioMux = portMUX_INITIALIZER_UNLOCKED;


enum Controll {CLK, ACLK, CLICK, LONG_CLICK, TIME_OUT};

//encoder and switch
long int millisLastEncoderChange =  0;
long int millisLastSwPress =  0;
uint8_t stateEncoder = 0;
bool switchPressed = 0;
int rotValueEncoder = 0, swNTimesPressed = 0, lastRotValueEncoder = 0, lastSwNTimesPressed = 0;


void initControls() {
  pinMode(ROTARY_PINA, INPUT_PULLUP);
  pinMode(ROTARY_PINB, INPUT_PULLUP);
  pinMode(ROTARY_PINSW, INPUT_PULLUP);

  attachInterrupt(ROTARY_PINA, isrAB, CHANGE);
  attachInterrupt(ROTARY_PINB, isrAB, CHANGE);
  attachInterrupt(ROTARY_PINSW, isrSWAll, CHANGE);
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

  for (int i = 0; i < abs((int) rotDifference / 2); i++) {
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

        if (cont == CLICK) {
          editSetting = !editSetting;
          return;
        }
        switch (setting) {
          case PROGRAMM: {
              if (editSetting) {
                switch (programm) {
                  case STANDBY: {
                      switch (cont) {
                        case CLK: {
                            programm = ON;
                          } break;
                        case ACLK: {
                            programm = ON_LOW_POW;
                          }  break;
                      }
                    } break;
                  case ON: {
                      switch (cont) {
                        case CLK: {
                            programm = ON_LOW_POW;
                          }  break;
                        case ACLK: {
                            programm = STANDBY;
                          }  break;
                      }
                    } break;
                  case ON_LOW_POW: {
                      switch (cont) {
                        case CLK: {
                            programm = STANDBY;
                          }  break;
                        case ACLK: {
                            programm = ON;
                          }  break;
                      }
                    } break;
                  case ERROR_PROGRAMM: { //!!
                      switch (cont) {
                        case CLK: {
                            programm = STANDBY;
                          }  break;
                        case ACLK: {
                            programm = ON_LOW_POW;
                          }  break;
                      }
                    } break;
                }
              } else {
                switch (cont) {
                  case CLK: {
                      setting = MAX_POW;
                    }  break;
                  case ACLK: {
                      setting = ERRORS;
                    }  break;
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
                      if (cont == CLK) {
                        pow_max = TWO;
                      }
                    } break;
                  case TWO: {
                      switch (cont) {
                        case CLK: {
                            pow_max = TREE;
                          }  break;
                        case ACLK: {
                            pow_max = ONE;
                          }  break;
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
                  case CLK: {
                      setting = WIFI;
                    }  break;
                  case ACLK: {
                      setting = PROGRAMM;
                    }  break;
                }
              }

            } break;
          case WIFI: {
              if (editSetting) {
                switch (cont) {
                  case CLK: {
                      wifi_on = true;
                    }  break;
                  case ACLK: {
                      wifi_on = false;
                    }  break;
                }
                //wifi_on = !wifi_on;
              } else {
                switch (cont) {
                  case CLK: {
                      setting = WEB_SERVER;
                    }  break;
                  case ACLK: {
                      setting = MAX_POW;
                    }  break;
                }
              }
            } break;
          case WEB_SERVER: {
              if (editSetting) {
                switch (cont) {
                  case CLK: {
                      web_server_on = true;
                    }  break;
                  case ACLK: {
                      web_server_on = false;
                    }  break;
                }
                // web_server_on = !web_server_on;
              } else {
                switch (cont) {
                  case CLK: {
                      setting = ERRORS;
                    }  break;
                  case ACLK: {
                      setting = WIFI;
                    }  break;
                }
              }
            } break;
          case ERRORS: {
              editSetting = false; //to redo
              switch (cont) {
                case CLK: {
                    setting = PROGRAMM;
                  }  break;
                case ACLK: {
                    setting = WEB_SERVER;
                  }  break;
              }
            } break;
        }
      } break;
    case ERROR_PAGE: {
        // da implm
      } break;
  }
}

#endif;
