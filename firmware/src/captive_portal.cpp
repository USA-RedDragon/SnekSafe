

#ifdef ARDUINO_ARCH_ESP32
#include <WiFi.h>
#include <esp_wifi.h> //Used for mpdu_rx_disable android workaround
#else
#include <ESP8266WiFi.h>
#endif

#include "captive_portal.h"

#define MAX_AP_CLIENTS 4

const char* CAPTIVE_PORTAL_LINK_SETUP = "http://192.9.200.1/setup";
const char* CAPTIVE_PORTAL_LINK = "http://192.9.200.1";
const IPAddress CAPTIVE_PORTAL_IP = IPAddress(192, 9, 200, 1);
const IPAddress CAPTIVE_PORTAL_GATEWAY = IPAddress(255, 255, 255, 0);

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

void CaptivePortal::changePassword(settings_t* settings) {
    WiFi.softAPdisconnect(false);
    WiFi.softAPConfig(CAPTIVE_PORTAL_IP, CAPTIVE_PORTAL_IP, CAPTIVE_PORTAL_GATEWAY);
    WiFi.softAP(settings->captivePortalSSID, settings->captivePortalPassword, settings->captivePortalChannel, 0, MAX_AP_CLIENTS);
}

void CaptivePortal::setup(settings_t* settings) {
    Serial.println("Starting SoftAP");
    if (WiFi.getMode() == WIFI_STA) {
        WiFi.mode(WIFI_AP_STA);
    } else {
        WiFi.mode(WIFI_AP);
    }
    WiFi.softAPConfig(CAPTIVE_PORTAL_IP, CAPTIVE_PORTAL_IP, CAPTIVE_PORTAL_GATEWAY);
    WiFi.softAP(settings->captivePortalSSID, settings->captivePortalPassword, settings->captivePortalChannel, 0, MAX_AP_CLIENTS);
    this->dnsServer->setTTL(300);
    this->dnsServer->start(53, "*", WiFi.softAPIP());

    #ifdef ARDUINO_ARCH_ESP32
        ampdu_rx_disable();
    #endif

    this->setupKnownEndpoints();
}

void CaptivePortal::loop() {
    this->dnsServer->processNextRequest();
}

void CaptivePortal::setupKnownEndpoints() {
    for (int i = 0; i < knownEndpointsSize; i++) {
        this->webServer->on(knownEndpoints[i], HTTP_ANY, [] (AsyncWebServerRequest *request) {
            if (WiFi.status() != WL_CONNECTED) {
                request->redirect(CAPTIVE_PORTAL_LINK_SETUP);
            } else {
                request->redirect(CAPTIVE_PORTAL_LINK);
            }
            Serial.printf("%s %s sent redirect to %s\n", request->host().c_str(), request->url().c_str(), CAPTIVE_PORTAL_LINK);
        });
    }
}

void CaptivePortal::end() {
    dnsServer->stop();
}
