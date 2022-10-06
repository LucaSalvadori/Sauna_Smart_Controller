#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define S_W 128 // OLED display width, in pixels
#define S_H 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x78 //0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

Adafruit_SSD1306 display(S_W, S_H, &Wire, OLED_RESET);

float tmp_int = 80.5;
float tmp_off = 70;

 enum Pow_level {OFF, ONE, TWO, TREE};
Pow_level power = OFF;
Pow_level pow_max = TREE;

 enum Programm {STANDBY, ON, ON_LOW_POW, ERROR_PROGRAMM};
Programm programm = STANDBY;

 enum Page {INFO, SETTING, ERROR_PAGE};

Page page = INFO;

 enum Settings {PROGRAMM, MAX_POW, SOLAR_READ, WIFI, WEB_SERVER, ERRORS};
Settings setting = PROGRAMM;

 enum Errors {ERR_b, ERR_a}; //tmp
//Errors error;


void TaskDisplay( void *pvParameters );

void setup() {
  Serial.begin(115200);

   xTaskCreatePinnedToCore(
      TaskBlink
      ,  "TaskDisplay"   // A name just for humans
      ,  1024  // This stack size can be checked & adjusted by reading the Stack Highwater
      ,  NULL
      ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
      ,  NULL 
      ,  ARDUINO_RUNNING_CORE
   ); 
}

void loop() {
  // put your main code here, to run repeatedly:

}

void DrawInfo(){
  display.clearDisplay();
  
  // draw current power line
  for(int i=0; i<power;i++){
    
      display.drawLine(S_W-5, (i*19)+((i+1)*2), S_W-5, (i*22), SSD1306_WHITE); 
  }
  
  //draw max power triangle
  int yPosT = 0;
  switch (pow_max){
    case OFF : yPosT = S_H - 4; break;
    case ONE : yPosT = 41; break;
    case TWO : yPosT = 20; break;
    case TREE : yPosT = 3; break;
  }
  
  display.fillTriangle(
      110  , yPosT - 3,
      110  , yPosT + 3,
      117  , yPosT, 
      SSD1306_WHITE
  );

  //draw current temperature
  display.setTextSize(8);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,2);
  int tmp_integer = tmp_int;
  int tmp_decimal = (tmp_int - tmp_integer)*10;
  display.print(tmp_integer);
  display.setTextSize(4);
  display.print(tmp_decimal);

  
  display.display(); // Show the display buffer on the screen
}
