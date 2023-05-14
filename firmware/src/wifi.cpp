#include <Arduino.h>

#ifdef ARDUINO_ARCH_ESP32
#include <WiFi.h>
#include <esp_wifi.h> //Used for mpdu_rx_disable android workaround
#else
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
        return true;
    }
}
