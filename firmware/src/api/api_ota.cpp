#include <Update.h>

#include "api.h"

void api_ota_setup(AsyncWebServer* server, settings_t* settings) {
    server->onFileUpload([] (AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
        if (request->method() != HTTP_POST) {
            request->send(405, "text/plain", "Method Not Allowed");
            return;
        }
        bool flash = false;
        bool frontend = false;
        if (request->url().endsWith("/api/v1/upload/firmware")) {
            flash = true;
        } else if (request->url().endsWith("/api/v1/upload/frontend")) {
            flash = true;
            frontend = true;
        } else {
            request->send(404);
            return;
        }
        if (flash) {
            if (!index) {
                Serial.printf("Upload Start: %s\n", filename.c_str());
                if (!Update.begin(UPDATE_SIZE_UNKNOWN, frontend ? U_SPIFFS : U_FLASH)) {
                    Update.printError(Serial);
                    String error = Update.errorString();
                    char error_buffer[error.length() + 15 + 1];
                    sprintf(error_buffer, "Update Failed: %s", error.c_str());
                    request->send(500, "text/plain", error_buffer);
                    return;
                }
            }
            if (Update.write(data, len) != len) {
                Update.printError(Serial);
                String error = Update.errorString();
                char error_buffer[error.length() + 15 + 1];
                sprintf(error_buffer, "Update Failed: %s", error.c_str());
                request->send(500, "text/plain", error_buffer);
                return;
            } else {
                float progress = ((index + len) / (float)request->contentLength()) * 100;
                Serial.printf("Upload Progress: %f%%\n", progress);
            }
            if (final) {
                Serial.printf("Upload End: %s (%u)\n", filename.c_str(), index + len);
                if (Update.end(true)) { //true to set the size to the current progress
                    Serial.printf("Update Success: %u\nRebooting...\n", index + len);
                    request->send(200);
                    delay(100);
                    ESP.restart();
                    return;
                } else {
                    Update.printError(Serial);
                    String error = Update.errorString();
                    char error_buffer[error.length() + 15 + 1];
                    sprintf(error_buffer, "Update Failed: %s", error.c_str());
                    request->send(500, "text/plain", error_buffer);
                }
            }
        }
    });
}