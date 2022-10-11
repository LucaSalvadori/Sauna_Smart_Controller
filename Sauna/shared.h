#ifndef _SHARED_
#define _SHARED_

SemaphoreHandle_t shared_Semaphore;

#define TEMP_DELTA 1.5 // temperature difference for heater activation
float tmp_int = 80.5; //-!- d h
float tmp_off = 70;   //-!- c d h

enum Pow_level {OFF, ONE, TWO, TREE};
Pow_level power = ONE; //-!- d h
Pow_level pow_max = TREE; //-!- c d h

enum Programm {STANDBY, ON, ON_LOW_POW, ERROR_PROGRAMM};
Programm programm = STANDBY; //-!- c d h

enum Page {INFO, SETTING, ERROR_PAGE};
Page page = INFO; //-!- c d

enum Settings {TEMPERATURE, PROGRAMM, MAX_POW, WIFI, WEB_SERVER, ERRORS};
Settings setting = PROGRAMM; //-!- c d
bool editSetting = false; //-!- c d

enum Errors {ERR_b, ERR_a}; //tmp
//Errors error;

bool wifi_on = false; //-!- c d
bool web_server_on = false; //-!- c d
#endif
