#include <LittleFS.h>

#include "captive_portal.h"
#include "frontend.h"

void frontend_setup(AsyncWebServer* server) {
    server->serveStatic("/", LittleFS, "/");
    server->serveStatic("/assets", LittleFS, "/assets").setCacheControl("max-age=600");

    server->on("/", HTTP_GET, [] (AsyncWebServerRequest *request) {
        // If the user agent has "CaptiveNetworkSupport" in it, we should redirect to the captive portal
        if (request->header("User-Agent").indexOf("CaptiveNetworkSupport") >= 0) {
            request->redirect(CAPTIVE_PORTAL_LINK);
        } else {
            request->send(LittleFS, "/index.html");
        }
    });

    server->onNotFound([](AsyncWebServerRequest *request){
        request->send(LittleFS, "/index.html");
    });
}
