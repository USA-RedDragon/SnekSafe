#include <AsyncJson.h>

#include "api.h"
#include "api/api_wifi.h"
#include "api/api_settings.h"
#include "globals.h"

void api_setup(AsyncWebServer* server, settings_t* settings) {
    server->on("/ping", HTTP_GET, [] (AsyncWebServerRequest *request) {
        request->send(200, "text/plain", String(millis()));
    });

    server->on("/api/v1/version", HTTP_GET, [] (AsyncWebServerRequest *request) {
        // 8 commit hash characters + 1 for null terminator
        // Extra for semver
        char commitHash[25];
        sprintf(commitHash, "v1.0.0-%s", GIT_COMMIT);
        request->send(200, "text/plain", commitHash);
    });

    server->on("/api/v1/state", HTTP_GET, [] (AsyncWebServerRequest *request) {
        AsyncJsonResponse* response = new AsyncJsonResponse();
        const JsonObject& doc = response->getRoot();

        doc["temperature"] = temperature;
        doc["humidity"] = humidity;
        doc["lastUpdate"] = lastUpdate;
        doc["heat"] = heatState;
        doc["light"] = lightState;
        doc["heaterPulseWidth"] = heaterPulseWidth;

        response->setLength();
        request->send(response);
    });

    server->on("/api/v1/heap", HTTP_GET, [] (AsyncWebServerRequest *request) {
        auto heap_size = ESP.getHeapSize();
        auto free_heap_size = ESP.getFreeHeap();
        AsyncJsonResponse* response = new AsyncJsonResponse();
        const JsonObject& doc = response->getRoot();
        doc["heapSize"] = heap_size;
        doc["freeHeapSize"] = free_heap_size;
        response->setLength();
        request->send(response);
    });

    api_wifi_setup(server, settings);
    api_settings_setup(server, settings);
}
