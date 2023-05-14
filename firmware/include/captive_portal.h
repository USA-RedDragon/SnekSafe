#ifndef __CAPTIVE_PORTAL_H__
#define __CAPTIVE_PORTAL_H__

#include <stdint.h>
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include "settings.h"

class CaptivePortal {
public:
    CaptivePortal(AsyncWebServer* webServer);
    void setup(settings_t* settings);
    void loop();
private:
    AsyncWebServer* webServer;
    DNSServer* dnsServer;
    void setupKnownEndpoints();
};

#endif
