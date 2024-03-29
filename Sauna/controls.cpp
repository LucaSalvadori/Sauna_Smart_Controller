#include "controls.hpp"

void initControls() {
  pinMode(ROTARY_PINA, INPUT_PULLUP);
  pinMode(ROTARY_PINB, INPUT_PULLUP);
  pinMode(ROTARY_PINSW, INPUT_PULLUP);

  attachInterrupt(ROTARY_PINA, isrAB, CHANGE);
  attachInterrupt(ROTARY_PINB, isrAB, CHANGE);
  attachInterrupt(ROTARY_PINSW, isrSW, CHANGE);

  input_queue = xQueueCreate(input_queue_len, sizeof(Control));
  ISR_Semaphore = xSemaphoreCreateBinary();
  xSemaphoreGive(ISR_Semaphore);
}

bool input_read() {
  bool S_update = false;
  while (xQueueReceive(input_queue, (void *)&cBuff, 0) == pdTRUE) { //empty queue
    navigate(cBuff);
    resetTimeout();
    S_update = true;
  }
  long int millisTime = millis();
  if (xSemaphoreTake(ISR_Semaphore, 10) == pdTRUE) {
    bool timeout = (millisTimeoutTime < millisTime && !switchPressed); 
    bool longPress = (switchPressed && millisLastSwPress + 1000 < millisTime);
    xSemaphoreGive(ISR_Semaphore);
    if (longPress && !consumedLongPress) {
      navigate(LONG_CLICK);
      resetTimeout();
      S_update = true;
      consumedLongPress = true;
    }
    if (timeout) {
      navigate(TIME_OUT);
      resetTimeout();
      S_update = true;
    }
  }
  return S_update;
}

void resetTimeout(){
  millisTimeoutTime = millis() + timeoutTime;
}



void IRAM_ATTR isrSW() {
  //Serial.print("S");
  long int encoderMillis =  millis();
  BaseType_t task_woken = pdFALSE;
  if (xSemaphoreTakeFromISR(ISR_Semaphore, &task_woken) == pdTRUE) { //!!
    if (millisLastEncoderChange + 20 <  encoderMillis && digitalRead(ROTARY_PINSW) == switchPressed) { // lo stato è cambiato e è passato abbastanza tempo
      if (switchPressed) {
        switchPressed = false;
        if (millisLastSwPress + 1000 > encoderMillis) { //ignore long press
         // swNTimesPressed++;
          if (xQueueIsQueueFullFromISR( input_queue ) == pdFALSE) {
            cBuff = CLICK;
            xQueueSendToBackFromISR(input_queue, (void *)&cBuff, &task_woken);
          }
        }else{
          consumedLongPress = false;
        }
      } else {
        switchPressed = true;
        millisLastSwPress = encoderMillis;
      }
      millisLastEncoderChange = encoderMillis;
    }
    xSemaphoreGiveFromISR(ISR_Semaphore, &task_woken); //!!
  }
}

void IRAM_ATTR isrAB() {
  BaseType_t task_woken = pdFALSE;
  uint8_t s = stateEncoder & 3;
  if (xSemaphoreTakeFromISR(ISR_Semaphore, &task_woken) == pdTRUE) {
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
        {
          rotValueEncoder += 2;
        } break;
      default:
        {
          rotValueEncoder -= 2;
        } break;
    }
    stateEncoder = (s >> 2);
    if (rotValueEncoder - lastRotValueEncoder >= 2) {
      if (xQueueIsQueueFullFromISR( input_queue ) == pdFALSE) {
        cBuff = CLK;
        xQueueSendToBackFromISR(input_queue, (void *)&cBuff, &task_woken);
      }
      lastRotValueEncoder = rotValueEncoder; //??
    } else if (rotValueEncoder - lastRotValueEncoder <= -2) {
      if (xQueueIsQueueFullFromISR( input_queue) == pdFALSE ) {
        cBuff = ACLK;
        xQueueSendToBackFromISR(input_queue, (void *)&cBuff, &task_woken);
      }
      lastRotValueEncoder = rotValueEncoder; //??
    }

    xSemaphoreGiveFromISR(ISR_Semaphore, &task_woken); //!!
  }
}

void navigate(Control cont) {
  while (xSemaphoreTake(shared_Semaphore, 10) == pdFALSE){Serial.print(F(" BC"));vTaskDelay(40);};

  Serial.println(cont);

  switch (page) {
    case INFO: {
        switch (cont) {
          case CLICK: {
              page = SETTING;
              setting = PROGRAM;
              editSetting = false;
              timeoutTime =  NORMAL_TIMEOUT;
            } break;
          case CLK:
          case ACLK: {
              page = SETTING;
              setting = TEMPERATURE;
              timeoutTime =  SHORT_TIMEOUT;
            } break;
          case LONG_CLICK: {
              if(program != STANDBY){
                program = STANDBY;
              }else{
                program = ON;
              }
              
            } break; //!!OFF
          case TIME_OUT: {} break;
        }
      } break;
    case SETTING: {
        if (cont == TIME_OUT || cont == LONG_CLICK) {
          timeoutTime =  NORMAL_TIMEOUT;
          page = INFO;
          xSemaphoreGive(shared_Semaphore);
          return;
        }
        if (setting == TEMPERATURE) {
          switch (cont) {
            case CLK: {
                tmp_off++;
              } break;
            case ACLK: {
                tmp_off--;
              } break;
            case CLICK: {
                page = INFO;
                timeoutTime =  NORMAL_TIMEOUT;
              } break;
          }
          xSemaphoreGive(shared_Semaphore);
          return;
        }

        if (cont == CLICK) {
          editSetting = !editSetting;
          xSemaphoreGive(shared_Semaphore);
          return;
        }
        switch (setting) {
          case PROGRAM: {
              if (editSetting) {
                switch (program) {
                  case STANDBY: {
                      switch (cont) {
                        case CLK: {
                            program = ON;
                          } break;
                        case ACLK: {
                            program = ON_LOW_POW;
                          }  break;
                      }
                    } break;
                  case ON: {
                      switch (cont) {
                        case CLK: {
                            program = ON_LOW_POW;
                          }  break;
                        case ACLK: {
                            program = STANDBY;
                          }  break;
                      }
                    } break;
                  case ON_LOW_POW: {
                      switch (cont) {
                        case CLK: {
                            program = STANDBY;
                          }  break;
                        case ACLK: {
                            program = ON;
                          }  break;
                      }
                    } break;
                  case ERROR_PROGRAM: { //!!
                      switch (cont) {
                        case CLK: {
                            program = STANDBY;
                          }  break;
                        case ACLK: {
                            program = ON_LOW_POW;
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
                      setting = PROGRAM;
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
                    setting = PROGRAM;
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
  xSemaphoreGive(shared_Semaphore);
}
