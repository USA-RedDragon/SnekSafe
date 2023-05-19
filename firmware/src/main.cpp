#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <EEPROM.h>

#ifdef ARDUINO_ARCH_ESP32
#include <WiFi.h>
#include <esp_wifi.h> //Used for mpdu_rx_disable android workaround
#else
#include <ESP8266WiFi.h>
#endif

#include "api.h"
#include "captive_portal.h"
#include "frontend.h"
#include "settings.h"
#include "wifi.h"

settings_t settings;

AsyncWebServer server(80);
CaptivePortal captivePortal(&server);
bool wifi_changed = false;

void setup() {
  Serial.begin(9600);
  // Serial.setDebugOutput(true);

  delay(100); // Wait for serial to initialize. Early messages may be lost otherwise.

  EEPROM.begin(sizeof(settings_t));

  // settings_t default_settings2;
  // default_settings2 = default_settings;

  // settings_write(&default_settings2);
  settings_read(&settings);

  if (!LittleFS.begin()){
    Serial.println("Failed to initialize LittleFS");
    ESP.restart();
    return;
  }

  captivePortal.setup(&settings);

  api_setup(&server, &settings);

  frontend_setup(&server);

  server.begin();

  if (!wifi_connect(&settings)) {
    Serial.println("Wifi not connected.");
  }
}

void loop() {
  captivePortal.loop();

  if (wifi_changed) {
    wifi_changed = false;
    wifi_connect(&settings);
  }

  delay(1);
}
