#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include <ESP32Time.h>

extern bool heatState;
extern bool lightState;
extern bool wifi_changed;
extern ESP32Time rtc;
extern double temperature;
extern float humidity;
extern unsigned long lastUpdate;
extern double heaterPulseWidth;

#endif