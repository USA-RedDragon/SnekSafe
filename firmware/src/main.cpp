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

#include "captive_portal.h"
#include "frontend.h"
#include "settings.h"
#include "wifi.h"

settings_t settings;

AsyncWebServer server(80);
CaptivePortal captivePortal(&server);

void setup() {
  Serial.begin(9600);

  delay(100); // Wait for serial to initialize. Early messages may be lost otherwise.

  EEPROM.begin(sizeof(settings_t));

  settings_read(&settings);

  if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }

  WiFi.mode(WIFI_STA);

  captivePortal.setup(&settings);

  server.on("/data", HTTP_GET, [] (AsyncWebServerRequest *request) {
    request->send(200, "application/json", "{ \"status\": \"ok\" }");
  });

  server.on("/ping", HTTP_GET, [] (AsyncWebServerRequest *request) {
    request->send(200, "text/plain", String(millis()));
  });

  frontend_setup(&server);

  server.begin();

  wifi_connect(&settings);
}

void loop() {
  captivePortal.loop();

  if (WiFi.status() != WL_CONNECTED) {
    wifi_connect(&settings);
  }

  delay(1);
}
