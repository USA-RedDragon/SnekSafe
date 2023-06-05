#include <Arduino.h>

#include <WiFi.h>
#include <WiFiUdp.h>
#include <ESPmDNS.h>
#include <esp_wifi.h> //Used for mpdu_rx_disable android workaround

#include <NTPClient.h>

#include <ArduinoOTA.h>

#include "globals.h"
#include "mywifi.h"

WiFiUDP ntpUDP;
ESP32Time rtc(0);
NTPClient timeClient(ntpUDP);

bool wifi_connect(settings_t* settings) {
    WiFi.begin(settings->wifiSSID, settings->wifiPassword);
    if (WiFi.waitForConnectResult(10000) != WL_CONNECTED) {
        return false;
    } else {
        WiFi.setAutoConnect(true);
        WiFi.setAutoReconnect(true);
        Serial.printf("WiFi Connected. IP=%s\n", WiFi.localIP().toString().c_str());
        ArduinoOTA.begin();
        timeClient.begin();
        timeClient.setTimeOffset(0);
        if (!timeClient.update()) {
            if (timeClient.forceUpdate()) {
                rtc.setTime(timeClient.getEpochTime());
            }
        } else {
            rtc.setTime(timeClient.getEpochTime());
        }
        if (!MDNS.begin(settings->mdnsName)) {
            Serial.println("Error setting up mDNS responder");
        } else {
            if (MDNS.addService("http", "tcp", 80)) {
                Serial.println("mDNS responder started on http://" + String(settings->mdnsName) + ".local");
            }
#ifdef DEV
            MDNS.enableArduino(3232, false);
#endif
        }
        return true;
    }
}

void wifi_tick() {
    if (WiFi.status() == WL_CONNECTED) {
        ArduinoOTA.handle();
        if (timeClient.update()) {
            rtc.setTime(timeClient.getEpochTime());
        }
    }
}
