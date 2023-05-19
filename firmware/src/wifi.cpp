#include <Arduino.h>

#ifdef ARDUINO_ARCH_ESP32
#include <WiFi.h>
#include <esp_wifi.h> //Used for mpdu_rx_disable android workaround
#else
#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
#endif

#include "wifi.h"

bool wifi_connect(settings_t* settings) {
    WiFi.begin(settings->wifiSSID, settings->wifiPassword);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        return false;
    } else {
        Serial.printf("WiFi Connected. IP=%s\n", WiFi.localIP().toString().c_str());
        WiFi.setAutoConnect(true);
        WiFi.setAutoReconnect(true);
        #ifdef ARDUINO_ARCH_ESP8266
        if (!MDNS.begin(settings->mdnsName, WiFi.localIP())) {
            Serial.println("Error setting up mDNS responder!");
        } else {
            Serial.println("mDNS responder started on http://" + String(settings->mdnsName) + ".local");
        }
        #endif

        return true;
    }
}