#include <ESPAsyncWebServer.h>
#ifdef ARDUINO_ARCH_ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif

#include "setup.h"

CaptiveSetupHandler::CaptiveSetupHandler() {
}

bool CaptiveSetupHandler::canHandle(AsyncWebServerRequest *request){
  //request->addInterestingHeader("ANY");
  return true;
}

void CaptiveSetupHandler::handleRequest(AsyncWebServerRequest *request) {
  request->redirect("/captive");
}

void CaptiveSetupHandler::handleHTTP(AsyncWebServerRequest *request) {
  AsyncResponseStream *response = request->beginResponseStream("text/html");
  response->print("<!DOCTYPE html><html><head><title>Captive Portal</title></head><body>");
  response->print("<p>This is out captive portal front page.</p>");
  response->printf("<p>You were trying to reach: http://%s%s</p>", request->host().c_str(), request->url().c_str());
  response->printf("<p>Try opening <a href='http://%s'>this link</a> instead</p>", WiFi.softAPIP().toString().c_str());
  response->print("</body></html>");
  request->send(response);
}
