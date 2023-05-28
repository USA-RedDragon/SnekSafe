#include <AsyncJson.h>

#include "api.h"
#include "globals.h"

void api_history_setup(AsyncWebServer* server) {
    server->on("/api/v1/history/temperature", HTTP_GET, [] (AsyncWebServerRequest *request) {
        DynamicJsonDocument doc(sizeof(float) * temperatureHistoryIndex + sizeof(unsigned long) * temperatureHistoryIndex + JSON_OBJECT_SIZE(2) + JSON_ARRAY_SIZE(temperatureHistoryIndex) + JSON_ARRAY_SIZE(temperatureHistoryIndex));

        const JsonArray& data = doc.createNestedArray("data");
        int numEntries = temperatureHistoryIndex;
        for (int i = 0; i < numEntries; i++) {
            data.add(temperatureHistory[i]);
        }

        const JsonArray& timestamps = doc.createNestedArray("timestamps");
        for (int i = 0; i < numEntries; i++) {
            timestamps.add(temperatureTimeHistory[i]);
        }

        String json;
        serializeJson(doc, json);
        request->send(200, "application/json", json.c_str());
    });

    server->on("/api/v1/history/humidity", HTTP_GET, [] (AsyncWebServerRequest *request) {
        DynamicJsonDocument doc(sizeof(float) * humidityHistoryIndex + sizeof(unsigned long) * humidityHistoryIndex + JSON_OBJECT_SIZE(2) + JSON_ARRAY_SIZE(humidityHistoryIndex) + JSON_ARRAY_SIZE(humidityHistoryIndex));

        const JsonArray& data = doc.createNestedArray("data");
        int numEntries = humidityHistoryIndex;
        for (int i = 0; i < numEntries; i++) {
            data.add(humidityHistory[i]);
        }

        const JsonArray& timestamps = doc.createNestedArray("timestamps");
        for (int i = 0; i < numEntries; i++) {
            timestamps.add(humidityTimeHistory[i]);
        }

        String json;
        serializeJson(doc, json);
        request->send(200, "application/json", json);
    });
}