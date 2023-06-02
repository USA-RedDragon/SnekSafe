#ifndef __OTA_H__
#define __OTA_H__

#include <ESPAsyncWebServer.h>

#include "captive_portal.h"
#include "globals.h"
#include "pid.hpp"

enum BinaryType {
    TYPE_FIRMWARE,
    TYPE_FRONTEND,
};

void ota_setup(AsyncWebServer* server, CaptivePortal* captivePortal, PID* pidController);
void ota_check_for_update();

#endif
