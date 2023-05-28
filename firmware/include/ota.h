#ifndef __OTA_H__
#define __OTA_H__

#include <ESPAsyncWebServer.h>

#include "captive_portal.h"
#include "globals.h"
#include "pid.hpp"

void ota_setup(AsyncWebServer* server, CaptivePortal* captivePortal, PID* pidController);

#endif
