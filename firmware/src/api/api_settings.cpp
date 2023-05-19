#include <AsyncJson.h>

#include "api.h"

void api_settings_setup(AsyncWebServer* server, settings_t* settings) {
    server->on("/api/v1/settings", HTTP_GET, [settings] (AsyncWebServerRequest *request) {
        AsyncJsonResponse* response = new AsyncJsonResponse();
        const JsonObject& doc = response->getRoot();
        doc["captivePortalSSID"] = settings->captivePortalSSID;
        doc["captivePortalChannel"] = settings->captivePortalChannel;
        doc["temperatureSetpoint"] = settings->temperatureSetpoint;
        doc["lightOnTime"] = settings->lightOnTime;
        doc["lightOffTime"] = settings->lightOffTime;
        doc["wifiSSID"] = settings->wifiSSID;
        doc["mdnsName"] = settings->mdnsName;
        response->setLength();
        request->send(response);
    });

    server->on("/api/v1/settings", HTTP_POST, [] (AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "ok");
    });
}