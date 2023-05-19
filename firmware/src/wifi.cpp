#include <Arduino.h>

#ifdef ARDUINO_ARCH_ESP32
#include <WiFi.h>
#include <WiFiUdp.h>
#include <ESPmDNS.h>
#include <esp_wifi.h> //Used for mpdu_rx_disable android workaround
#else
#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
#endif

#include <NTPClient.h>

#include "wifi.h"

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

bool wifi_connect(settings_t* settings) {
    WiFi.begin(settings->wifiSSID, settings->wifiPassword);
    if (WiFi.waitForConnectResult(10000) != WL_CONNECTED) {
        return false;
    } else {
        WiFi.setAutoConnect(true);
        WiFi.setAutoReconnect(true);
        Serial.printf("WiFi Connected. IP=%s\n", WiFi.localIP().toString().c_str());
        timeClient.begin();
        timeClient.setTimeOffset(settings->timezoneOffset * 60);
        if (!timeClient.update()) {
            timeClient.forceUpdate();
        }
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

void wifi_update_time() {
    if (WiFi.status() == WL_CONNECTED) {
        timeClient.update();
    }
}

void wifi_update_timezone(settings_t* settings) {
    timeClient.setTimeOffset(settings->timezoneOffset * 60);
}