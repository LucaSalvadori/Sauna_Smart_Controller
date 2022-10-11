#include "shared.h"
#include "draw.h"
#include "controls.h"
#include "heater.h"

#include <SPI.h>
#include <Wire.h>

//--Functions--
//void TaskDisplay( void *pvParameters );
bool initDispaly();
void draw();
void DrawInfo();
void DrawSetting();

void initControls();
void navigate(Controll cont);
void IRAM_ATTR isr_rotary_encoder();
void IRAM_ATTR isrAB();
void IRAM_ATTR isrSW();
bool input_read();

bool initHeater();
void heaterControll();

void setup() {



  Serial.begin(115200);
  Serial.println(F("Hello"));

  shared_Semaphore = xSemaphoreCreateBinary();
  xSemaphoreGive(shared_Semaphore);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally

  initControls();
  if (!initDispaly()) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }


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
    if (i % 30 == 0) {
      Serial.print("SW ");
      Serial.print(swNTimesPressed);
      Serial.print(" Encoder ");
      Serial.println(rotValueEncoder);
    }
    
    if (input_read() || i % 20 == 0) {
      draw();
      heaterControll();
    }

    vTaskDelay(100/portTICK_PERIOD_MS);
  }
}
