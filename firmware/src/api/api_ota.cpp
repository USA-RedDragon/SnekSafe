#include <AsyncJson.h>
#include <Update.h>

#include "api.h"
#include "ota.h"

void api_ota_setup(AsyncWebServer* server, settings_t* settings) {
    AsyncCallbackJsonWebHandler* handler = new AsyncCallbackJsonWebHandler("/api/v1/ota/web", [](AsyncWebServerRequest *request, JsonVariant &json) {
        if (request->method() != HTTP_POST) {
            request->send(405, "text/plain", "Method Not Allowed");
            return;
        }
        AsyncJsonResponse* response = new AsyncJsonResponse();
        const JsonObject& root = response->getRoot();
        const JsonObject& body = json.as<JsonObject>();

        bool restart = true;
        if (request->hasParam("restart")) {
            restart = request->getParam("restart")->value().equals("true");
        }

        if (!body.containsKey("binary_url")) {
            root["status"] = "error";
            root["message"] = "Must provide binary_url";
            response->setCode(400);
            response->setLength();
            request->send(response);
            return;
        }

        if (!body.containsKey("type")) {
            root["status"] = "error";
            root["message"] = "Type must be one of firmware or frontend";
            response->setCode(400);
            response->setLength();
            request->send(response);
            return;
        }

        const char* binary_url = body["binary_url"];
        const char* type = body["type"];

        if (strcmp(type, "firmware") != 0 && strcmp(type, "frontend") != 0) {
            root["status"] = "error";
            root["message"] = "Type must be one of firmware or frontend";
            response->setCode(400);
            response->setLength();
            request->send(response);
            return;
        }

        BinaryType binary_type = TYPE_FIRMWARE;
        if (strcmp(type, "frontend") == 0) {
            binary_type = TYPE_FRONTEND;
        }

        root["status"] = "ok";
        root["message"] = "Update started";
        response->setCode(200);

        firmwareUpdateURL = binary_url;
        firmwareUpdateFlag = true;
        firmwareUpdateReboot = restart;

        response->setLength();
        request->send(response);
    });
    server->addHandler(handler);

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