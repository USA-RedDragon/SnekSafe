#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include <stdint.h>

#include <ESPAsyncWebServer.h>
#include <ESP32Time.h>
#include <WiFiClientSecure.h>

#include "captive_portal.h"

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

extern float humidityHistory[999];
extern float temperatureHistory[999];
extern unsigned long humidityTimeHistory[999];
extern unsigned long temperatureTimeHistory[999];
extern int humidityHistoryIndex;
extern int temperatureHistoryIndex;

extern AsyncEventSource events;
extern CaptivePortal captivePortal;

extern const uint8_t rootca_crt_bundle_start[] asm("_binary_certs_x509_crt_bundle_bin_start");

extern WiFiClientSecure httpsClient;

#endif
