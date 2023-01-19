#ifndef _SHARED_
#define _SHARED_

#include <Arduino.h> //!!


extern SemaphoreHandle_t shared_Semaphore;

#define TEMP_DELTA 1.5 // temperature difference for heater activation
extern float tmp_int; //-!- d h
extern float tmp_off;   //-!- c d h
extern float hum;

 enum Pow_level {OFF, ONE, TWO, TREE};
extern Pow_level power; //-!- d h
extern Pow_level pow_max; //-!- c d h

 enum Programm {STANDBY, ON, ON_LOW_POW, ERROR_PROGRAMM};
extern Programm programm; //-!- c d h

 enum Page {INFO, SETTING, ERROR_PAGE};
extern Page page; //-!- c d

 enum Settings {TEMPERATURE, PROGRAMM, MAX_POW, WIFI, WEB_SERVER, ERRORS};
extern Settings setting; //-!- c d
extern bool editSetting; //-!- c d

 enum Errors {ERR_b, ERR_a}; //tmp
//Errors error;

extern bool wifi_on; //-!- c d
extern bool web_server_on; //-!- c d
#endif
