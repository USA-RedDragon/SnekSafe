#include <AsyncJson.h>

#include "api.h"
#include "globals.h"

void api_toggles_setup(AsyncWebServer* server, settings_t* settings) {
    server->on("/api/v1/toggle/light", HTTP_POST, [] (AsyncWebServerRequest *request) {
        AsyncJsonResponse* response = new AsyncJsonResponse();
        const JsonObject& doc = response->getRoot();

        lightState = !lightState;

        doc["status"] = lightState ? "on" : "off";

        response->setLength();
        request->send(response);
    });

    server->on("/api/v1/toggle/heat", HTTP_POST, [] (AsyncWebServerRequest *request) {
        AsyncJsonResponse* response = new AsyncJsonResponse();
        const JsonObject& doc = response->getRoot();

        if (heaterPulseWidth > 0) {
            heaterPulseWidth = 0;
        } else {
            heaterPulseWidth = 255;
        }

        doc["status"] = heaterPulseWidth > 0 ? "on" : "off";

        response->setLength();
        request->send(response);
    });
}
