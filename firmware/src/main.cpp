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

// File globals
settings_t settings;
AsyncWebServer server(80);
AsyncEventSource events("/events");
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

FireTimer timer1m;
FireTimer timer10s;
FireTimer timer10ms;
FireTimer timer100ms;
FireTimer timer500ms;

// Program-wide globals
bool lightState = false;
unsigned long lastUpdate = 0;
bool wifi_changed = false;
double temperature = 0;
float humidity = 0;
double heaterPulseWidth = 0;
CaptivePortal captivePortal(&server);

float humidityHistory[999] = {0};
float temperatureHistory[999] = {0};
unsigned long humidityTimeHistory[999] = {0};
unsigned long temperatureTimeHistory[999] = {0};
int humidityHistoryIndex = 0;
int temperatureHistoryIndex = 0;

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

  Serial.printf("\nSnekSafe v%s-%s\n", VERSION, GIT_COMMIT);

  prefs.begin("settings");
  settings_read(&settings);

  if (!LittleFS.begin()){
    Serial.println("Failed to initialize LittleFS");
    ESP.restart();
    return;
  }

  ota_setup(&server, &captivePortal, &pidController);

  sht31_setup();

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
    if (rtc.getEpoch() != 0) {
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

  timer1m.begin(1000 * 60);
  timer10s.begin(1000 * 10);
  timer500ms.begin(500);
  timer100ms.begin(100);
  timer10ms.begin(10);

  analogWriteFrequency(120);
}

void loop() {
  captivePortal.loop();

  analogWrite(HEATER_PIN, heaterPulseWidth);

  wifi_tick();

  if (wifi_changed) {
    wifi_changed = false;
    if (wifi_connect(&settings)) {
      // Only update the light if we have a valid time after 1/1/2023
      if (rtc.getEpoch() != 0) {
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
  }

  // Only update the light if we have a valid time after 1/1/2023
  if (rtc.getEpoch() != 0) {
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

  if(timer100ms.fire() && rtc.getEpoch() != 0) {
    if (sht31_read(&stagedTemperature, &humidity)) {
      temperature = (float) stagedTemperature;
      lastUpdate = rtc.getEpoch();
      if (!pidController.isStarted()) {
        Serial.println("Starting PID controller");
        pidController.begin();
        Serial.println("");
      }
    } else {
      Serial.println("Failed to read temperature in 100ms loop");
    }
  }

  if(timer500ms.fire() && pidController.isStarted()) {
    pidController.compute();
    Serial.print("Heater Pulse Width = "); Serial.println(heaterPulseWidth);
    pidController.debug();
    Serial.println("");
  }

  if(timer10s.fire() && pidController.isStarted()) {
    Serial.print("Temp *F = "); Serial.println(temperature);
    Serial.print("Humidity % = "); Serial.println(humidity);

    DynamicJsonDocument doc(1024);
    doc["temperature"] = temperature;
    doc["humidity"] = humidity;
    doc["lastUpdate"] = lastUpdate;
    doc["heat"] = heaterPulseWidth > 0;
    doc["light"] = lightState;
    doc["heaterPulseWidth"] = heaterPulseWidth;
    doc["temperatureSetpoint"] = settings.temperatureSetpoint;
    String json;
    serializeJson(doc, json);
    events.send(json.c_str(), "state", millis());

    if (temperatureHistoryIndex == 998) {
      // Shift the history arrays
      for (int i = 0; i < 998; i++) {
        temperatureHistory[i] = temperatureHistory[i + 1];
        temperatureTimeHistory[i] = temperatureTimeHistory[i + 1];
      }
      temperatureHistoryIndex = 998;
      temperatureHistory[temperatureHistoryIndex] = temperature;
      temperatureTimeHistory[temperatureHistoryIndex] = lastUpdate;
    } else {
      temperatureHistory[temperatureHistoryIndex] = temperature;
      temperatureTimeHistory[temperatureHistoryIndex] = lastUpdate;
      temperatureHistoryIndex++;
    }

    if (humidityHistoryIndex == 998) {
      // Shift the history arrays
      for (int i = 0; i < 998; i++) {
        humidityHistory[i] = humidityHistory[i + 1];
        humidityTimeHistory[i] = humidityTimeHistory[i + 1];
      }
      humidityHistoryIndex = 998;
      humidityHistory[humidityHistoryIndex] = humidity;
      humidityTimeHistory[humidityHistoryIndex] = lastUpdate;
    } else {
      humidityHistory[humidityHistoryIndex] = humidity;
      humidityTimeHistory[humidityHistoryIndex] = lastUpdate;
      humidityHistoryIndex++;
    }
  }

  if(timer1m.fire()) {
    static bool heaterOn = false;
    if (humidity > 95 && !heaterOn) {
      Serial.println("SHT31 Heater Enabled");
      sht31_set_heater(true);
      heaterOn = true;
    } else {
      sht31_set_heater(false);
      heaterOn = false;
    }
  }
}
