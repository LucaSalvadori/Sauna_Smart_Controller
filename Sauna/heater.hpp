#ifndef _HEATER_
#define _HEATER_
#include "shared.hpp"

#define ONE_WIRE_BUS 16
#define RELAY_1 19
#define RELAY_2 2
#define RELAY_3 27

#include <OneWire.h>
#include <DallasTemperature.h>

#include <Wire.h>
#include "Adafruit_SHT31.h"

static OneWire oneWire(ONE_WIRE_BUS);// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
static DallasTemperature sensors(&oneWire);// Pass our oneWire reference to Dallas Temperature.

static TwoWire I2C_2 = TwoWire(1);
static Adafruit_SHT31 sht31;

bool initHeater();
void heaterControl();
void TaskHeater(void *pvParameters);

#endif
