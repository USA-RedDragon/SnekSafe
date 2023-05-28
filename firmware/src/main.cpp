#include <Arduino.h>
#include <ArduinoJson.h>
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
#include "ota.h"
#include "pid.hpp"
#include "settings.h"
#include "sht31.h"
#include "wifi.h"

// Program-wide globals
bool lightState = false;
unsigned long lastUpdate = 0;
bool wifi_changed = false;
double temperature = 0;
float humidity = 0;
double heaterPulseWidth = 0;

// File globals
settings_t settings;
AsyncWebServer server(80);
AsyncEventSource events("/events");
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
FireTimer timer10ms;

void setup() {
  pinMode(HEATER_PIN, OUTPUT);
  pinMode(LIGHT_PIN, OUTPUT);
  pinMode(BOOT0_PIN, INPUT_PULLUP);

  // Turn the heater off during boot for safety
  digitalWrite(HEATER_PIN, LOW);

  Serial.begin(9600);
#ifdef DEV
  Serial.setDebugOutput(true);
#endif

  delay(100); // Wait for serial to initialize. Early messages may be lost otherwise.

  Serial.printf("\nSnekSafe v1.0.0-%s\n", GIT_COMMIT);

  prefs.begin("settings");
  settings_read(&settings);

  if (!LittleFS.begin()){
    Serial.println("Failed to initialize LittleFS");
    ESP.restart();
    return;
  }

  ota_setup(&server, &captivePortal, &pidController);

  sht31_setup();

  // We don't accidentally want to start with the heater on
  sht31_set_heater(false);

  captivePortal.setup(&settings);

  api_setup(&server, &settings);

  frontend_setup(&server);

  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it gat is: %u\n", client->lastId());
    }
  });
  server.addHandler(&events);

#ifdef DEV
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "*");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "*");
#endif

  server.begin();

  if (!wifi_connect(&settings)) {
    Serial.println("Wifi not connected.");
  } else {
    // Only update the light if we have a valid time after 1/1/2023
    if (rtc.getEpoch() > 1672531200) {
      if (rtc.getMinute() >= settings.lightOnMinute && rtc.getHour(true) >= settings.lightOnHour
        && rtc.getMinute() <= settings.lightOffMinute && rtc.getHour(true) <= settings.lightOffHour) {
        // If we haven't already turned the light on during this minute, turn it on
        if (!lightState) {
          lightState = true;
        }
      } else {
        // If we haven't already turned the light off during this minute, turn it off
        if (lightState) {
          lightState = false;
        }
      }
    }
  }

  timer3s.begin(1000 * 3);
  timer10s.begin(1000 * 10);
  timer30s.begin(1000 * 30);
  timer1m.begin(1000 * 60);
  // 100Hz PWM
  timer10ms.begin(10);
}

void loop() {
  captivePortal.loop();

  wifi_tick();

  if (wifi_changed) {
    wifi_changed = false;
    if (wifi_connect(&settings)) {
      // Only update the light if we have a valid time after 1/1/2023
      if (rtc.getEpoch() > 1672531200) {
        if (rtc.getMinute() >= settings.lightOnMinute && rtc.getHour(true) >= settings.lightOnHour
          && rtc.getMinute() <= settings.lightOffMinute && rtc.getHour(true) <= settings.lightOffHour) {
          // If we haven't already turned the light on during this minute, turn it on
          if (!lightState) {
            lightState = true;
          }
        } else {
          // If we haven't already turned the light off during this minute, turn it off
          if (lightState) {
            lightState = false;
          }
        }
      }
    }
  }

  if (timer10ms.fire()) {
    if (digitalRead(BOOT0_PIN) == LOW) {
      // If the user is holding down the BOOT0 button, reset the settings to defaults
      Serial.println("Resetting settings to defaults");
      settings_t default_settings2;
      default_settings2 = default_settings;
      settings_write(&default_settings2);
      ESP.restart();
    }
    // Update the light state every 10ms
    analogWrite(HEATER_PIN, heaterPulseWidth);
  }

  // Only update the light if we have a valid time after 1/1/2023
  if (rtc.getEpoch() > 1672531200) {
    if (rtc.getMinute() == settings.lightOnMinute && rtc.getHour(true) == settings.lightOnHour) {
      // If we haven't already turned the light on during this minute, turn it on
      if (!lightState) {
        lightState = true;
        Serial.println("Light On");
      }
    }

    if (rtc.getMinute() == settings.lightOffMinute && rtc.getHour(true) == settings.lightOffHour) {
      // If we haven't already turned the light off during this minute, turn it off
      if (lightState) {
        lightState = false;
        Serial.println("Light Off");
      }
    }
  }

  digitalWrite(LIGHT_PIN, lightState);

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
        pidController.compute();
        Serial.print("Heater Pulse Width = "); Serial.println(heaterPulseWidth);
        pidController.debug();
        Serial.println("");
      }
    } else {
      Serial.println("Failed to read temperature in 3s loop");
    }
    DynamicJsonDocument doc(1024);
    doc["temperature"] = temperature;
    doc["lastUpdate"] = lastUpdate;
    doc["heat"] = heaterPulseWidth > 0;
    doc["light"] = lightState;
    doc["heaterPulseWidth"] = heaterPulseWidth;
    String json;
    serializeJson(doc, json);
    events.send(json.c_str(), "state", millis());
    sht31_set_heater(prevHeat);
  }

  // Humidity readings are only updated every 10 seconds
  if(timer10s.fire()) {
    bool prevHeat = sht31_get_heater();
    sht31_set_heater(false);
    if (sht31_read(&stagedTemperature, &stagedHumidity)) {
      humidity = stagedHumidity;
      Serial.print("Hum. % = "); Serial.println(humidity);
      DynamicJsonDocument doc(1024);
      doc["humidity"] = humidity;
      doc["lastUpdate"] = lastUpdate;
      doc["heat"] = heaterPulseWidth > 0;
      doc["light"] = lightState;
      doc["heaterPulseWidth"] = heaterPulseWidth;
      String json;
      serializeJson(doc, json);
      events.send(json.c_str(), "state", millis());
      sht31_set_heater(prevHeat);
    } else {
      Serial.println("Failed to read humidity in 10s loop");
    }
    sht31_set_heater(prevHeat);
  }

  // Call pidController.compute() every 30 seconds,
  // which updates the heaterPulseWidth value.
  if(timer30s.fire()) {
    pidController.compute();
    Serial.print("Heater Pulse Width = "); Serial.println(heaterPulseWidth);
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
