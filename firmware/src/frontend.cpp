#include <LittleFS.h>

#include "captive_portal.h"
#include "frontend.h"

void frontend_setup(AsyncWebServer* server) {
    server->serveStatic("/", LittleFS, "/");
    // 1y cache. The frontend is versioned, so we can cache it for a long time.
    server->serveStatic("/assets", LittleFS, "/assets").setCacheControl("max-age=31536000");

    server->on("/", HTTP_GET, [] (AsyncWebServerRequest *request) {
        // If the user agent has "CaptiveNetworkSupport" in it, we should redirect to the captive portal
        if (request->header("User-Agent").indexOf("CaptiveNetworkSupport") >= 0) {
            if (WiFi.status() != WL_CONNECTED) {
                request->redirect(CAPTIVE_PORTAL_LINK_SETUP);
            } else {
                request->redirect(CAPTIVE_PORTAL_LINK);
            }
        } else {
            request->send(LittleFS, "/index.html");
        }
    });

    server->onNotFound([](AsyncWebServerRequest *request){
        if (request->method() == HTTP_OPTIONS) {
            request->send(200);
        } else if (request->url().startsWith("/api")) {
            request->send(404);
        } else {
            request->send(LittleFS, "/index.html");
        }
    });
}
