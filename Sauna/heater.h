#ifndef _HEATER_
#define _HEATER_
#include "shared.h"

#define ONE_WIRE_BUS 15
#define RELAY_1 4
#define RELAY_2 0
#define RELAY_3 2

//#include <OneWire.h>
//#include <DallasTemperature.h>
//OneWire oneWire(ONE_WIRE_BUS);// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
//DallasTemperature sensors(&oneWire);// Pass our oneWire reference to Dallas Temperature.

bool initHeater();
void heaterControll();

#endif
