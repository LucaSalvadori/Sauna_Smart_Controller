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


bool initHeater() {
  return true;
}

void heaterControll() {
  //read temperature
  //  sensors.requestTemperatures(); // Send the command to get temperatures
  // After we got the temperatures, we can print them here.
  // We use the function ByIndex, and as an example get the temperature from the first sensor only.
  // float tempC = sensors.getTempCByIndex(0);

  // Check if reading was successful
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

  //controll
  if (power == OFF) {
    if (tmp_int < tmp_off - TEMP_DELTA) {
      power = pow_max;
    }
  } else {
    if (tmp_int >= tmp_off) {
      power = OFF;
    } else {
      power = pow_max;
    }
  }

  //relay update
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

}
#endif
