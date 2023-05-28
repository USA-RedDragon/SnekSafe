#ifndef __CAPTIVE_PORTAL_H__
#define __CAPTIVE_PORTAL_H__

#include <stdint.h>
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include "settings.h"

extern const char* CAPTIVE_PORTAL_LINK;
extern const char* CAPTIVE_PORTAL_LINK_SETUP;
extern const IPAddress CAPTIVE_PORTAL_IP;
extern const IPAddress CAPTIVE_PORTAL_GATEWAY;

class CaptivePortal {
public:
    CaptivePortal(AsyncWebServer* webServer);
    void setup(settings_t* settings);
    void loop();
    void end();
private:
    AsyncWebServer* webServer;
    DNSServer* dnsServer;
    void setupKnownEndpoints();
};

#endif
