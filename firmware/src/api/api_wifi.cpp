#ifdef ARDUINO_ARCH_ESP32
#include <WiFi.h>
#include <esp_wifi.h> //Used for mpdu_rx_disable android workaround
#else
#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
#endif

#include <AsyncJson.h>

#include "api.h"
#include "wifi.h"

void api_wifi_setup(AsyncWebServer* server, settings_t* settings) {
    server->on("/api/v1/wifi/scan", HTTP_POST, [] (AsyncWebServerRequest *request) {
        AsyncJsonResponse* response = new AsyncJsonResponse();
        const JsonObject& root = response->getRoot();
        auto num_networks = WiFi.scanComplete();
        if (num_networks == -1) {
            root["status"] = "error";
            root["message"] = "Scan already in progress";
        } else if (num_networks == -2) {
            root["status"] = "success";
            root["message"] = "Scan started";
            WiFi.scanNetworks(true);
        } else if (num_networks >= 0) {
            root["status"] = "success";
            root["message"] = "Scan started";
            WiFi.scanDelete();
            WiFi.scanNetworks(true);
        }
        response->setLength();
        request->send(response);
    });

    server->on("/api/v1/wifi/scan/status", HTTP_GET, [] (AsyncWebServerRequest *request) {
        AsyncJsonResponse* response = new AsyncJsonResponse();
        const JsonObject& root = response->getRoot();
        auto num_networks = WiFi.scanComplete();
        if (num_networks == -1) {
            root["status"] = "error";
            root["message"] = "Scan still in progress";
        } else if (num_networks == -2) {
            root["status"] = "error";
            root["message"] = "Scan never called";
        } else if (num_networks >= 0) {
            root["status"] = "success";
            root["message"] = "Scan complete";
        }
        response->setLength();
        request->send(response);
    });

    server->on("/api/v1/wifi/scan/results", HTTP_GET, [] (AsyncWebServerRequest *request) {
        AsyncJsonResponse* response = new AsyncJsonResponse();
        const JsonObject& root = response->getRoot();
        auto num_networks = WiFi.scanComplete();
        if (num_networks == -1) {
            root["status"] = "error";
            root["message"] = "Scan still in progress";
        } else if (num_networks == -2) {
            root["status"] = "error";
            root["message"] = "Scan never called";
        } else if (num_networks >= 0) {
            const JsonArray& data = root.createNestedArray("data");
            for (int i = 0; i < num_networks; i++) {
                data.add(const_cast<char*>(WiFi.SSID(i).c_str()));
            }
        }
        response->setLength();
        request->send(response);
    });

    server->on("/api/v1/wifi/status", HTTP_GET, [] (AsyncWebServerRequest *request) {
        AsyncJsonResponse* response = new AsyncJsonResponse();
        const JsonObject& root = response->getRoot();
        switch (WiFi.getMode()) {
            case WIFI_OFF:
                root["mode"] = "off";
                break;
            case WIFI_STA:
                root["mode"] = "station";
                break;
            case WIFI_AP:
                root["mode"] = "access point";
                break;
            case WIFI_AP_STA:
                root["mode"] = "access point and station";
                break;
        }
        switch (WiFi.status()) {
            case WL_CONNECTED:
                root["status"] = "connected";
                root["ssid"] = WiFi.SSID();
                root["ip"] = WiFi.localIP().toString();
                break;
            case WL_IDLE_STATUS:
                root["status"] = "idle";
                break;
            case WL_NO_SSID_AVAIL:
                root["status"] = "no ssid available";
                break;
            case WL_SCAN_COMPLETED:
                root["status"] = "scan completed";
                break;
            case WL_CONNECT_FAILED:
                root["status"] = "connect failed";
                break;
            case WL_CONNECTION_LOST:
                root["status"] = "connection lost";
                break;
            case WL_DISCONNECTED:
                root["status"] = "disconnected";
                break;
        }
        response->setLength();
        request->send(response);
    });

    AsyncCallbackJsonWebHandler* handler = new AsyncCallbackJsonWebHandler("/api/v1/wifi/connect", [settings](AsyncWebServerRequest *request, JsonVariant &json) {
        if (request->method() != HTTP_POST) {
            request->send(405, "text/plain", "Method Not Allowed");
            return;
        }
        AsyncJsonResponse* response = new AsyncJsonResponse();
        const JsonObject& root = response->getRoot();
        const JsonObject& body = json.as<JsonObject>();

        const char* ssid = body["ssid"];
        auto ssid_len = strlen(ssid);
        if (ssid == NULL || ssid_len == 0) {
            root["status"] = "error";
            root["message"] = "Missing SSID";
            response->setCode(400);
            response->setLength();
            request->send(response);
            return;
        } else if (ssid_len > 32) {
            root["status"] = "error";
            root["message"] = "SSID too long";
            response->setCode(400);
            response->setLength();
            request->send(response);
            return;
        } else if (ssid_len < 2) {
            root["status"] = "error";
            root["message"] = "SSID too short";
            response->setCode(400);
            response->setLength();
            request->send(response);
            return;
        }

        const char* password = body["password"];
        auto password_len = strlen(password);
        if (ssid == NULL || password_len == 0) {
            root["status"] = "error";
            root["message"] = "Missing password";
            response->setCode(400);
            response->setLength();
            request->send(response);
            return;
        } else if (password_len > 63) {
            root["status"] = "error";
            root["message"] = "Password too long";
            response->setCode(400);
            response->setLength();
            request->send(response);
            return;
        } else if (password_len < 8) {
            root["status"] = "error";
            root["message"] = "Password too short";
            response->setCode(400);
            response->setLength();
            request->send(response);
            return;
        }

        Serial.printf("ssid=%s, password=%s\n", ssid, password);

        for (int i = 0; i < ssid_len; i++) {
            settings->wifiSSID[i] = ssid[i];
        }
        settings->wifiSSID[ssid_len] = '\0';

        for (int i = 0; i < password_len; i++) {
            settings->wifiPassword[i] = password[i];
        }
        settings->wifiPassword[password_len] = '\0';

        settings_write(settings);

        root["status"] = "ok";
        root["message"] = "WiFi settings saved";

        wifi_changed = true;

        response->setLength();
        request->send(response);
    });
    server->addHandler(handler);
}