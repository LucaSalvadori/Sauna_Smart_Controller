#include "heater.hpp"

bool initHeater() {

  pinMode(RELAY_1, OUTPUT);
  pinMode(RELAY_2, OUTPUT);
  pinMode(RELAY_3, OUTPUT);


  
  sensors.begin();
  I2C_2.begin(4, 17);
  sht31 = Adafruit_SHT31(&I2C_2);
  
  if (! sht31.begin(0x44)) {   // Set to 0x45 for alternate i2c addr
    Serial.println("Couldn't find SHT31");
  }
  
  return true;
}

void heaterControll() {
  //read temperature (!! is very slow)
 //  sensors.requestTemperatures(); // Send the command to get temperatures

//   float tempC = sensors.getTempCByIndex(0);

  // Check if reading was successful
//     if(tempC != DEVICE_DISCONNECTED_C)
//     {
// //      Serial.print("Temperature for the device 1 (index 0) is: ");
// //      Serial.println(tempC);f
//       Serial.print(F("Temp ext *C = ")); Serial.print(tempC); Serial.print(F("\t\t"));
//       //tmp_int = tempC;
//     }
//     else
//     {
//       Serial.println(F("Error: Could not read ext temperature data"));
//     }



  float t = sht31.readTemperature();
  float h = sht31.readHumidity();

  if (! isnan(t)) {  // check if 'is not a number'
    tmp_int = t;
    Serial.print(F("Temp int *C = ")); Serial.print(t); Serial.print(F("\t\t"));
  } else { 
    Serial.println(F("Failed to read int temperature"));
  }
  
  if (! isnan(h)) {  // check if 'is not a number'
    hum = h;
    Serial.print(F("Hum. % = ")); Serial.println(h);
  } else { 
    Serial.println(F("Failed to read humidity"));
  }
  

  //controll
  if(programm == STANDBY){
    power = OFF;
  }else if (power == OFF) {
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
    switch (power) {
      case TREE:{
        digitalWrite(RELAY_3, 1);
        digitalWrite(RELAY_2, 1);
        digitalWrite(RELAY_1, 1);
      }break;
      case TWO: {
        digitalWrite(RELAY_3, 1);
        digitalWrite(RELAY_2, 0);
        digitalWrite(RELAY_1, 1);
      }break;
      case ONE: {
        digitalWrite(RELAY_3, 0);
        digitalWrite(RELAY_2, 1);
        digitalWrite(RELAY_1, 0);
      }break;
      case OFF:{
        digitalWrite(RELAY_3, 0);
        digitalWrite(RELAY_2, 0);
        digitalWrite(RELAY_1, 0);
      }break;
    }

}
