#include <AsyncJson.h>

#include "api.h"
#include "globals.h"
#include "pid.hpp"
#include "mywifi.h"

void api_settings_setup(AsyncWebServer* server, settings_t* settings) {
    server->on("/api/v1/settings", HTTP_GET, [settings] (AsyncWebServerRequest *request) {
        AsyncJsonResponse* response = new AsyncJsonResponse();
        const JsonObject& doc = response->getRoot();
        doc["captivePortalSSID"] = settings->captivePortalSSID;
        doc["captivePortalChannel"] = settings->captivePortalChannel;
        doc["temperatureSetpoint"] = settings->temperatureSetpoint;
        doc["lightOnHour"] = settings->lightOnHour;
        doc["lightOnMinute"] = settings->lightOnMinute;
        doc["lightOffHour"] = settings->lightOffHour;
        doc["lightOffMinute"] = settings->lightOffMinute;
        doc["wifiSSID"] = settings->wifiSSID;
        doc["mdnsName"] = settings->mdnsName;
        doc["pGain"] = settings->pGain;
        doc["iGain"] = settings->iGain;
        doc["dGain"] = settings->dGain;
        doc["iMax"] = settings->iMax;
        doc["iMin"] = settings->iMin;
        response->setLength();
        request->send(response);
    });

    server->on("/api/v1/settings/reset", HTTP_POST, [settings] (AsyncWebServerRequest *request) {
        AsyncJsonResponse* response = new AsyncJsonResponse();
        const JsonObject& doc = response->getRoot();
        
        strncpy(settings->captivePortalSSID, default_settings.captivePortalSSID, sizeof(settings->captivePortalSSID));
        strncpy(settings->captivePortalPassword, default_settings.captivePortalPassword, sizeof(settings->captivePortalPassword));
        settings->captivePortalChannel = default_settings.captivePortalChannel;
        settings->temperatureSetpoint = default_settings.temperatureSetpoint;
        settings->lightOnHour = default_settings.lightOnHour;
        settings->lightOnMinute = default_settings.lightOnMinute;
        settings->lightOffHour = default_settings.lightOffHour;
        settings->lightOffMinute = default_settings.lightOffMinute;
        strncpy(settings->wifiSSID, default_settings.wifiSSID, sizeof(settings->wifiSSID));
        strncpy(settings->wifiPassword, default_settings.wifiPassword, sizeof(settings->wifiPassword));
        strncpy(settings->mdnsName, default_settings.mdnsName, sizeof(settings->mdnsName));
        settings->pGain = default_settings.pGain;
        settings->iGain = default_settings.iGain;
        settings->dGain = default_settings.dGain;
        settings->iMax = default_settings.iMax;
        settings->iMin = default_settings.iMin;

        settings_write(settings);

        doc["status"] = "success";
        doc["message"] = "Settings reset to defaults";
        response->setLength();
        request->send(response);
        WiFi.disconnect(false, true);
        wifi_changed = true;
    });

    AsyncCallbackJsonWebHandler* handler = new AsyncCallbackJsonWebHandler("/api/v1/settings", [settings](AsyncWebServerRequest *request, JsonVariant &json) {
        if (request->method() != HTTP_PATCH) {
            request->send(405, "text/plain", "Method Not Allowed");
            return;
        }
        AsyncJsonResponse* response = new AsyncJsonResponse();
        const JsonObject& root = response->getRoot();
        const JsonObject& body = json.as<JsonObject>();

        // Settings that are present in the body are ones that we want to update.
        // If they are not present, we want to leave them alone.
        if (body.containsKey("captivePortalSSID")) {
            const char* captivePortalSSID = body["captivePortalSSID"];
            auto captivePortalSSID_len = strlen(captivePortalSSID);
            if (captivePortalSSID == NULL || captivePortalSSID_len == 0) {
                root["status"] = "error";
                root["message"] = "Missing captivePortalSSID";
                response->setCode(400);
                response->setLength();
                request->send(response);
                return;
            } else if (captivePortalSSID_len > 32) {
                root["status"] = "error";
                root["message"] = "captivePortalSSID too long";
                response->setCode(400);
                response->setLength();
                request->send(response);
                return;
            } else if (captivePortalSSID_len < 2) {
                root["status"] = "error";
                root["message"] = "captivePortalSSID too short";
                response->setCode(400);
                response->setLength();
                request->send(response);
                return;
            }
            strncpy(settings->captivePortalSSID, captivePortalSSID, captivePortalSSID_len);
            settings->captivePortalSSID[captivePortalSSID_len] = '\0';
        }

        if (body.containsKey("captivePortalChannel")) {
            int captivePortalChannel = body["captivePortalChannel"];
            if (captivePortalChannel < 1 || captivePortalChannel > 11) {
                root["status"] = "error";
                root["message"] = "captivePortalChannel out of range";
                response->setCode(400);
                response->setLength();
                request->send(response);
                return;
            }
            settings->captivePortalChannel = captivePortalChannel;
        }

        if (body.containsKey("temperatureSetpoint")) {
            int temperatureSetpoint = body["temperatureSetpoint"];
            if (temperatureSetpoint < 70 || temperatureSetpoint > 100) {
                root["status"] = "error";
                root["message"] = "temperatureSetpoint out of range";
                response->setCode(400);
                response->setLength();
                request->send(response);
                return;
            }
            settings->temperatureSetpoint = temperatureSetpoint;
        }

        if (body.containsKey("lightOnHour")) {
            int lightOnHour = body["lightOnHour"];
            if (lightOnHour < 0 || lightOnHour > 23) {
                root["status"] = "error";
                root["message"] = "lightOnHour out of range";
                response->setCode(400);
                response->setLength();
                request->send(response);
                return;
            }
            settings->lightOnHour = lightOnHour;
        }

        if (body.containsKey("lightOffHour")) {
            int lightOffHour = body["lightOffHour"];
            if (lightOffHour < 0 || lightOffHour > 23) {
                root["status"] = "error";
                root["message"] = "lightOffHour out of range";
                response->setCode(400);
                response->setLength();
                request->send(response);
                return;
            }
            settings->lightOffHour = lightOffHour;
        }

        if (body.containsKey("lightOnMinute")) {
            int lightOnMinute = body["lightOnMinute"];
            if (lightOnMinute < 0 || lightOnMinute > 59) {
                root["status"] = "error";
                root["message"] = "lightOnMinute out of range";
                response->setCode(400);
                response->setLength();
                request->send(response);
                return;
            }
            settings->lightOnMinute = lightOnMinute;
        }

        if (body.containsKey("lightOffMinute")) {
            int lightOffMinute = body["lightOffMinute"];
            if (lightOffMinute < 0 || lightOffMinute > 59) {
                root["status"] = "error";
                root["message"] = "lightOffMinute out of range";
                response->setCode(400);
                response->setLength();
                request->send(response);
                return;
            }
            settings->lightOffMinute = lightOffMinute;
        }

        bool changePortalPassword = false;
        if (body.containsKey("captivePortalPassword")) {
            const char* captivePortalPassword = body["captivePortalPassword"];
            auto captivePortalPassword_len = strlen(captivePortalPassword);
            if (captivePortalPassword == NULL || captivePortalPassword_len == 0) {
                root["status"] = "error";
                root["message"] = "Missing captivePortalPassword";
                response->setCode(400);
                response->setLength();
                request->send(response);
                return;
            } else if (captivePortalPassword_len > 63) {
                root["status"] = "error";
                root["message"] = "captivePortalPassword too long";
                response->setCode(400);
                response->setLength();
                request->send(response);
                return;
            } else if (captivePortalPassword_len < 8) {
                root["status"] = "error";
                root["message"] = "captivePortalPassword too short";
                response->setCode(400);
                response->setLength();
                request->send(response);
                return;
            }
            strncpy(settings->captivePortalPassword, captivePortalPassword, captivePortalPassword_len);
            settings->captivePortalPassword[captivePortalPassword_len] = '\0';
            changePortalPassword = true;
        }

        if (body.containsKey("mdnsName")) {
            const char* mdnsName = body["mdnsName"];
            auto mdnsName_len = strlen(mdnsName);
            if (mdnsName == NULL || mdnsName_len == 0) {
                root["status"] = "error";
                root["message"] = "Missing mdnsName";
                response->setCode(400);
                response->setLength();
                request->send(response);
                return;
            } else if (mdnsName_len > 32) {
                root["status"] = "error";
                root["message"] = "mdnsName too long";
                response->setCode(400);
                response->setLength();
                request->send(response);
                return;
            } else if (mdnsName_len < 2) {
                root["status"] = "error";
                root["message"] = "mdnsName too short";
                response->setCode(400);
                response->setLength();
                request->send(response);
                return;
            }
            strncpy(settings->mdnsName, mdnsName, mdnsName_len);
            settings->mdnsName[mdnsName_len] = '\0';
        }

        if (body.containsKey("wifiSSID")) {
            const char* wifiSSID = body["wifiSSID"];
            auto wifiSSID_len = strlen(wifiSSID);
            if (wifiSSID == NULL || wifiSSID_len == 0) {
                root["status"] = "error";
                root["message"] = "Missing wifiSSID";
                response->setCode(400);
                response->setLength();
                request->send(response);
                return;
            } else if (wifiSSID_len > 32) {
                root["status"] = "error";
                root["message"] = "wifiSSID too long";
                response->setCode(400);
                response->setLength();
                request->send(response);
                return;
            } else if (wifiSSID_len < 2) {
                root["status"] = "error";
                root["message"] = "wifiSSID too short";
                response->setCode(400);
                response->setLength();
                request->send(response);
                return;
            }
            strncpy(settings->wifiSSID, wifiSSID, wifiSSID_len);
            settings->wifiSSID[wifiSSID_len] = '\0';
            wifi_changed = true;
        }

        if (body.containsKey("wifiPassword")) {
            const char* wifiPassword = body["wifiPassword"];
            auto wifiPassword_len = strlen(wifiPassword);
            if (wifiPassword == NULL || wifiPassword_len == 0) {
                root["status"] = "error";
                root["message"] = "Missing wifiPassword";
                response->setCode(400);
                response->setLength();
                request->send(response);
                return;
            } else if (wifiPassword_len > 63) {
                root["status"] = "error";
                root["message"] = "wifiPassword too long";
                response->setCode(400);
                response->setLength();
                request->send(response);
                return;
            } else if (wifiPassword_len < 8) {
                root["status"] = "error";
                root["message"] = "wifiPassword too short";
                response->setCode(400);
                response->setLength();
                request->send(response);
                return;
            }
            strncpy(settings->wifiPassword, wifiPassword, wifiPassword_len);
            settings->wifiPassword[wifiPassword_len] = '\0';
            wifi_changed = true;
        }

        if (body.containsKey("pGain")) {
            double pGain = body["pGain"];
            if (pGain < 0) {
                root["status"] = "error";
                root["message"] = "pGain out of range";
                response->setCode(400);
                response->setLength();
                request->send(response);
                return;
            }
            settings->pGain = pGain;
            pidController.end();
            pidController.begin();
        }

        if (body.containsKey("iGain")) {
            double iGain = body["iGain"];
            if (iGain < 0) {
                root["status"] = "error";
                root["message"] = "iGain out of range";
                response->setCode(400);
                response->setLength();
                request->send(response);
                return;
            }
            settings->iGain = iGain;
            pidController.end();
            pidController.begin();
        }

        if (body.containsKey("dGain")) {
            double dGain = body["dGain"];
            if (dGain < 0) {
                root["status"] = "error";
                root["message"] = "dGain out of range";
                response->setCode(400);
                response->setLength();
                request->send(response);
                return;
            }
            settings->dGain = dGain;
            pidController.end();
            pidController.begin();
        }

        if (body.containsKey("iMax")) {
            double iMax = body["iMax"];
            if (iMax < 0) {
                root["status"] = "error";
                root["message"] = "iMax out of range";
                response->setCode(400);
                response->setLength();
                request->send(response);
                return;
            }
            settings->iMax = iMax;
            pidController.setIMax(iMax);
        }

        if (body.containsKey("iMin")) {
            double iMin = body["iMin"];
            if (iMin < 0) {
                root["status"] = "error";
                root["message"] = "iMin out of range";
                response->setCode(400);
                response->setLength();
                request->send(response);
                return;
            }
            settings->iMin = iMin;
            pidController.setIMin(iMin);
        }

        root["status"] = "success";
        root["message"] = "Settings updated";

        settings_write(settings);

        response->setLength();
        request->send(response);
        if (changePortalPassword) {
            captivePortal.changePassword(settings);
        }
    });
    server->addHandler(handler);
}