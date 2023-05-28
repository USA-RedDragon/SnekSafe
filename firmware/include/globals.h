#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include <ESP32Time.h>

extern bool lightState;
extern bool wifi_changed;
extern ESP32Time rtc;
extern double temperature;
extern float humidity;
extern unsigned long lastUpdate;
extern double heaterPulseWidth;

const int HEATER_PIN = 18;
const int LIGHT_PIN = 19;
const int BOOT0_PIN = 0;

#endif