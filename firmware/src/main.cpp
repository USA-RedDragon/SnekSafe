#include <Arduino.h>
#ifdef ARDUINO_ARCH_ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include "FS.h"
#include <LittleFS.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>

#include "setup.h"

char ssid[] = "";
char pass[] = "";
int status = WL_IDLE_STATUS;

AsyncWebServer server(80);
DNSServer dnsServer;

void setup() {
  Serial.begin(9600);

  if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }
  
  Serial.println("Starting SoftAP for setup");
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(IPAddress(192, 168, 4, 1), IPAddress(192, 168, 4, 1), IPAddress(255, 255, 255, 0));
  WiFi.softAP("esp-captive");
  dnsServer.start(53, "*", WiFi.softAPIP());
  AsyncWebHandler* captiveSetup = new CaptiveSetupHandler();
  server.addHandler(captiveSetup).setFilter(ON_AP_FILTER);
  server.on("/captive", HTTP_GET, [] (AsyncWebServerRequest *request) {
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    response->print("<!DOCTYPE html><html><head><title>Captive Portal</title></head><body>");
    response->print("<p>This is out captive portal front page.</p>");
    response->printf("<p>You were trying to reach: http://%s%s</p>", request->host().c_str(), request->url().c_str());
    response->printf("<p>Try opening <a href='http://%s'>this link</a> instead</p>", WiFi.softAPIP().toString().c_str());
    response->print("</body></html>");
    request->send(response);
  });

  // Serial.println("Attempting to connect to WPA network...");
  // Serial.print("SSID: ");
  // Serial.println(ssid);

  // WiFi.mode(WIFI_STA);
  // status = WiFi.begin(ssid, pass);
  // if (WiFi.waitForConnectResult() != WL_CONNECTED) {
  //   Serial.println("Failed connecting to wifi");
  // }
  // Serial.print("Connected to wifi. My address:");
  // Serial.println(WiFi.localIP());

  // server.on("/data", HTTP_GET, [] (AsyncWebServerRequest *request) {
  //   request->send(200, "application/json", "{ \"status\": \"ok\" }");
  // });

  // server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html").setCacheControl("max-age=600");

  // server.onNotFound([](AsyncWebServerRequest *request){
  //   request->send(LittleFS, "/index.html");
  // });

  server.begin();
}

void loop() {
  int prevStatus = status;
  status = WiFi.status();

  if (status != prevStatus) {
    switch (status) {
      case WL_IDLE_STATUS:
        Serial.println("WLAN idle");
        break;
      case WL_NO_SSID_AVAIL:
        Serial.println("WLAN SSID not available");
        break;
      case WL_CONNECTED:
        Serial.println("WLAN connected");
        break;
      case WL_CONNECT_FAILED:
        Serial.println("WLAN failed to connnect");
        break;
#ifdef ARDUINO_ARCH_ESP8266
      case WL_WRONG_PASSWORD:
        Serial.println("WLAN wrong password");
        break;
#endif
      case WL_DISCONNECTED:
        Serial.println("WLAN disconnected");
        break;
    }
  }

  delay(500);
}
