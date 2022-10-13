#ifndef _CONTROLS_
#define _CONTROLS_

#include "shared.hpp"
#include <Arduino.h> //!!


//pins
#define ROTARY_PINA 19
#define ROTARY_PINB 18
#define ROTARY_PINSW 5

#define SHORT_TIMEOUT 3000
#define NORMAL_TIMEOUT 10000
#define LONG_TIMEOUT 30000


static const int input_queue_len = 10;     // Size of input_queue
static QueueHandle_t input_queue;

static SemaphoreHandle_t ISR_Semaphore;

enum Controll {CLK, ACLK, CLICK, LONG_CLICK, TIME_OUT};

//encoder and switch
static volatile long int millisLastEncoderChange =  0;
static volatile long int millisLastSwPress =  0;
static volatile uint8_t stateEncoder = 0;
static volatile bool switchPressed = 0;
static volatile int rotValueEncoder = 0, swNTimesPressed = 0, lastRotValueEncoder = 0, lastSwNTimesPressed = 0;
static Controll cBuff;
static long int timeoutTime =  NORMAL_TIMEOUT;
static long int millisTimeoutTime =  0;

bool input_read();
void initControls();
void navigate(Controll cont);
void IRAM_ATTR isr_rotary_encoder();
void IRAM_ATTR isrAB();
void IRAM_ATTR isrSW();
void resetTimeout();
#endif;