#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <FireTimer.h>

#ifdef ARDUINO_ARCH_ESP32
#include <WiFi.h>
#include <WiFiUdp.h>
#else
#include <ESP8266WiFi.h>
#endif

#include "api.h"
#include "captive_portal.h"
#include "frontend.h"
#include "globals.h"
#include "pid.hpp"
#include "settings.h"
#include "sht31.h"
#include "wifi.h"

const int HEATER_PIN = 18;
const int LIGHT_PIN = 19;

// Program-wide globals
bool heatState = false;
bool lightState = false;
unsigned long lastUpdate = 0;
bool wifi_changed = false;
double temperature = 0;
float humidity = 0;
double heaterPulseWidth = 0;

// File globals
settings_t settings;
AsyncWebServer server(80);
CaptivePortal captivePortal(&server);
float stagedHumidity = 0;
float stagedTemperature = 0;

PID pidController = PID(
  &(settings.pGain),
  &(settings.iGain),
  &(settings.dGain),
  &(settings.iMax),
  &(settings.iMin),
  &(settings.temperatureSetpoint),
  &temperature,
  &heaterPulseWidth
);

FireTimer timer3s;
FireTimer timer10s;
FireTimer timer30s;
FireTimer timer1m;

void setup() {
  pinMode(HEATER_PIN, OUTPUT);
  pinMode(LIGHT_PIN, OUTPUT);

  // Turn the heater off during boot for safety
  digitalWrite(HEATER_PIN, LOW);

  Serial.begin(9600);
#ifdef DEV
  Serial.setDebugOutput(true);
#endif

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

  // We don't accidentally want to start with the heater on
  sht31_set_heater(false);

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

  timer3s.begin(1000 * 3);
  timer10s.begin(1000 * 10);
  timer30s.begin(1000 * 30);
  timer1m.begin(1000 * 60);

  // Check time, see if light should be on
}

void loop() {
  captivePortal.loop();

  wifi_update_time();

  if (wifi_changed) {
    wifi_changed = false;
    wifi_connect(&settings);
  }

  // Collect temp readings every 3 seconds
  if(timer3s.fire()) {
    bool prevHeat = sht31_get_heater();
    sht31_set_heater(false);
    if (sht31_read(&stagedTemperature, &stagedHumidity)) {
      temperature = (float) stagedTemperature;
      lastUpdate = rtc.getEpoch();
      Serial.print("Temp *F = "); Serial.println(temperature);
      // we have a reading, if the pid controller is not started, start it
      if (!pidController.isStarted()) {
        Serial.println("Starting PID controller");
        pidController.begin();
      }
    } else {
      Serial.println("Failed to read temperature in 3s loop");
    }
    sht31_set_heater(prevHeat);
  }

  // Humidity readings are only updated every 10 seconds
  if(timer10s.fire()) {
    bool prevHeat = sht31_get_heater();
    sht31_set_heater(false);
    if (sht31_read(&stagedTemperature, &stagedHumidity)) {
      humidity = stagedHumidity;
      Serial.print("Hum. % = "); Serial.println(humidity);
    } else {
      Serial.println("Failed to read humidity in 10s loop");
    }
    sht31_set_heater(prevHeat);
  }

  // Call pidController.compute() every 30 seconds,
  // which updates the heaterPulseWidth value.
  if(timer30s.fire()) {
    pidController.compute();
    heatState = heaterPulseWidth > 0;
    Serial.println("");
    pidController.debug();
    Serial.println("");
  }

  if(timer1m.fire()) {
    bool prevHeat = sht31_get_heater();
    if (humidity > 95 && !prevHeat) {
      Serial.println("SHT31 Heater Enabled");
      sht31_set_heater(true);
    } else if (prevHeat) {
      Serial.println("SHT31 Heater Disabled");
      sht31_set_heater(false);
    }
  }
}
