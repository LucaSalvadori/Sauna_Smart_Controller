//#include "shared.h"
#include "draw.hpp"
#include "controls.hpp"
#include "heater.hpp"

#include <SPI.h>
#include <Wire.h>


//--GLOBALS--
 SemaphoreHandle_t shared_Semaphore;
 float tmp_int = 0; //-!- d h
 float tmp_off = 70;   //-!- c d h
 float hum = 0;

// enum Pow_level {OFF, ONE, TWO, TREE};
 Pow_level power = ONE; //-!- d h
 Pow_level pow_max = TREE; //-!- c d h

 //enum Program {STANDBY, ON, ON_LOW_POW, ERROR_PROGRAM};
 Program program = STANDBY; //-!- c d h

 //enum Page {INFO, SETTING, ERROR_PAGE};
 Page page = INFO; //-!- c d

 //enum Settings {TEMPERATURE, PROGRAM, MAX_POW, WIFI, WEB_SERVER, ERRORS};
 Settings setting = PROGRAM; //-!- c d
 bool editSetting = false; //-!- c d

 //enum Errors {ERR_b, ERR_a}; //tmp
//Errors error;

 bool wifi_on = false; //-!- c d
 bool web_server_on = false; //-!- c d

//--Functions--


void setup() {
  
  Serial.begin(2000000);
  Serial.println(F("Smart Sauna : by Luca Salvadori"));

  shared_Semaphore = xSemaphoreCreateBinary();
  xSemaphoreGive(shared_Semaphore);

  initHeater();
  initControls();
  if (!initDisplay()) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
 // Serial.println(&page);
  

  //   xTaskCreatePinnedToCore(
  //      TaskBlink
  //      ,  "TaskDisplay"   // A name just for humans
  //      ,  1024  // This stack size can be checked & adjusted by reading the Stack Highwater
  //      ,  NULL
  //      ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
  //      ,  NULL
  //      ,  ARDUINO_RUNNING_CORE
  //   );
  
}

void loop() {
  for (int i = 0; true; i++) {
//    if (i % 30 == 0) {
 //     Serial.print("SW ------");
//      Serial.print(swNTimesPressed);
//      Serial.print(" Encoder ");
//      Serial.println(rotValueEncoder);
//    }

    if (input_read()){
      draw();
    }else if (i % 20 == 0) {
      draw();
      heaterControl();
    }
    
    vTaskDelay(10/portTICK_PERIOD_MS);
  }
}
