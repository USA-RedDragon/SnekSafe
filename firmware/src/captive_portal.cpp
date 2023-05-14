

#ifdef ARDUINO_ARCH_ESP32
#include <WiFi.h>
#include <esp_wifi.h> //Used for mpdu_rx_disable android workaround
#else
#include <ESP8266WiFi.h>
#endif
#include <LittleFS.h>

#include "captive_portal.h"

#define MAX_AP_CLIENTS 4

uint8_t knownEndpointsSize = 9;
const char* knownEndpoints[] = {
    "/mobile/status.php",
    "/generate_204",
    "/gen_204",
    "/ncsi.txt",
    "/hotspot-detect.html",
    "/hotspotdetect.html",
    "/library/test/success.html",
    "/success.txt",
    "/kindle-wifi/wifiredirect.html",
};

#ifdef ARDUINO_ARCH_ESP32
void ampdu_rx_disable() {
  //ampdu_rx_disable android workaround see https://github.com/espressif/arduino-esp32/issues/4423
  esp_wifi_stop();
  esp_wifi_deinit();

  wifi_init_config_t my_config = WIFI_INIT_CONFIG_DEFAULT();   //We use the default config ...
  my_config.ampdu_rx_enable = false;                           //... and modify only what we want.

  esp_wifi_init(&my_config); //set the new config
  esp_wifi_start(); //Restart WiFi
  delay(100); //this is necessary don't ask me why
}
#endif

CaptivePortal::CaptivePortal(AsyncWebServer* webServer) {
    this->webServer = webServer;
    this->dnsServer = new DNSServer();
}

void CaptivePortal::setup(settings_t* settings) {
    Serial.println("Starting SoftAP");
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(IPAddress(192, 9, 200, 1), IPAddress(192, 9, 200, 1), IPAddress(255, 255, 255, 0));
    WiFi.softAP(settings->captivePortalSSID, settings->captivePortalPassword, settings->captivePortalChannel, 0, MAX_AP_CLIENTS);
    this->dnsServer->setTTL(300);
    this->dnsServer->start(53, "*", WiFi.softAPIP());

    #ifdef ARDUINO_ARCH_ESP32
        ampdu_rx_disable();
    #endif

    this->setupKnownEndpoints();

    this->webServer->serveStatic("/", LittleFS, "/");
    this->webServer->serveStatic("/assets", LittleFS, "/assets").setCacheControl("max-age=600");

    this->webServer->on("/", HTTP_GET, [] (AsyncWebServerRequest *request) {
        // If the user agent has "CaptiveNetworkSupport" in it, we should redirect to the captive portal
        if (request->header("User-Agent").indexOf("CaptiveNetworkSupport") >= 0) {
            request->redirect("http://192.9.200.1/");
        } else {
            request->send(LittleFS, "/index.html");
        }
    });

    this->webServer->onNotFound([](AsyncWebServerRequest *request){
        request->send(LittleFS, "/index.html");
    });
}

void CaptivePortal::loop() {
    this->dnsServer->processNextRequest();
}

void CaptivePortal::setupKnownEndpoints() {
    for (int i = 0; i < knownEndpointsSize; i++) {
        this->webServer->on(knownEndpoints[i], HTTP_ANY, [] (AsyncWebServerRequest *request) {
            request->redirect("http://192.9.200.1/");
            Serial.printf("%s %s sent redirect to http://192.9.200.1/\n", request->host().c_str(), request->url().c_str());
        });
    }
}