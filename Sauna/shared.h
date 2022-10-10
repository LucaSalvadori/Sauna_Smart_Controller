#ifndef _SHARED_
#define _SHARED_
#define TEMP_DELTA 1.5 // temperature difference for heater activation
float tmp_int = 80.5; //-!-
float tmp_off = 70;   //-!-

enum Pow_level {OFF, ONE, TWO, TREE};
Pow_level power = ONE; //-!-
Pow_level pow_max = TREE; //-!-

enum Programm {STANDBY, ON, ON_LOW_POW, ERROR_PROGRAMM};
Programm programm = STANDBY; //-!-

enum Page {INFO, SETTING, ERROR_PAGE};
Page page = INFO; //-!-

enum Settings {TEMPERATURE, PROGRAMM, MAX_POW, WIFI, WEB_SERVER, ERRORS};
Settings setting = PROGRAMM; //-!-
bool editSetting = false; //-!-?

enum Errors {ERR_b, ERR_a}; //tmp
//Errors error;

bool wifi_on = false; //-!-
bool web_server_on = false; //-!-
#endif
