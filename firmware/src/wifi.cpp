#include <Arduino.h>

#ifdef ARDUINO_ARCH_ESP32
#include <WiFi.h>
#include <ESPmDNS.h>
#include <esp_wifi.h> //Used for mpdu_rx_disable android workaround
#else
#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
#endif

#include "wifi.h"

bool wifi_connect(settings_t* settings) {
    WiFi.begin(settings->wifiSSID, settings->wifiPassword);
    if (WiFi.waitForConnectResult(10000) != WL_CONNECTED) {
        return false;
    } else {
        WiFi.setAutoConnect(true);
        WiFi.setAutoReconnect(true);
        Serial.printf("WiFi Connected. IP=%s\n", WiFi.localIP().toString().c_str());
#ifdef ARDUINO_ARCH_ESP32
        if (!MDNS.begin(settings->mdnsName)) {
#else
        if (!MDNS.begin(settings->mdnsName, WiFi.localIP())) {
#endif
            Serial.println("Error setting up mDNS responder");
        } else {
            if (MDNS.addService("http", "tcp", 80)) {
                Serial.println("mDNS responder started on http://" + String(settings->mdnsName) + ".local");
            }
        }
        return true;
    }
}
