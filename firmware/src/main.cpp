#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <EEPROM.h>

#include "captive_portal.h"
#include "frontend.h"
#include "settings.h"

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

  captivePortal.setup(&settings);

  server.on("/data", HTTP_GET, [] (AsyncWebServerRequest *request) {
    request->send(200, "application/json", "{ \"status\": \"ok\" }");
  });

  server.on("/ping", HTTP_GET, [] (AsyncWebServerRequest *request) {
    request->send(200, "text/plain", String(millis()));
  });

  frontend_setup(&server);

  server.begin();
}

void loop() {
  captivePortal.loop();

  delay(1);
}
