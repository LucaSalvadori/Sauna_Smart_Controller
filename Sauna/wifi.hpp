#ifndef _WIFI_
#define _WIFI_

#include "wifiPass.hpp"
#include "shared.hpp"
#include <WiFi.h>

external const char* ssid;
external const char* password;

WiFiServer server(80);

bool initWifi();

#endif