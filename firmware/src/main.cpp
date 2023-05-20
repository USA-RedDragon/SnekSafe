#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>

#ifdef ARDUINO_ARCH_ESP32
#include <WiFi.h>
#include <WiFiUdp.h>
#else
#include <ESP8266WiFi.h>
#endif

#include "api.h"
#include "captive_portal.h"
#include "frontend.h"
#include "pid.hpp"
#include "settings.h"
#include "sht31.h"
#include "wifi.h"

settings_t settings;

AsyncWebServer server(80);
CaptivePortal captivePortal(&server);

bool wifi_changed = false;

double temperature = 0;
double heaterPulseWidth = 0;
float humidity = 0;

PID pidController = PID(
  &settings.pGain,
  &settings.iGain,
  &settings.dGain,
  &settings.iMax,
  &settings.iMin,
  &settings.temperatureSetpoint,
  &temperature,
  &heaterPulseWidth
);

void setup() {
  Serial.begin(9600);
  // Serial.setDebugOutput(true);

  delay(100); // Wait for serial to initialize. Early messages may be lost otherwise.

  prefs.begin("settings");

  // settings_t default_settings2;
  // default_settings2 = default_settings;

  // settings_write(&default_settings2);
  settings_read(&settings);

  if (!LittleFS.begin()){
    Serial.println("Failed to initialize LittleFS");
    ESP.restart();
    return;
  }

  sht31_setup();

  captivePortal.setup(&settings);

  api_setup(&server, &settings);

  frontend_setup(&server);

#ifdef DEV
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "*");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "*");
#endif

  server.begin();

  if (!wifi_connect(&settings)) {
    Serial.println("Wifi not connected.");
  }

  pidController.begin();
}

void loop() {
  captivePortal.loop();

  wifi_update_time();

  if (wifi_changed) {
    wifi_changed = false;
    wifi_connect(&settings);
  }

  sht31_read(&temperature, &humidity);

  pidController.debug();

  delay(1);
}
