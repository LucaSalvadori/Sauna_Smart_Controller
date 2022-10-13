#ifndef _HEATER_
#define _HEATER_
#include "shared.hpp"

#define ONE_WIRE_BUS 4
#define RELAY_1 23
#define RELAY_2 0
#define RELAY_3 2

#include <OneWire.h>
#include <DallasTemperature.h>
static OneWire oneWire(ONE_WIRE_BUS);// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
static DallasTemperature sensors(&oneWire);// Pass our oneWire reference to Dallas Temperature.

bool initHeater();
void heaterControll();

#endif
