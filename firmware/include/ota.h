#ifndef __OTA_H__
#define __OTA_H__

#include <ESPAsyncWebServer.h>

#include "captive_portal.h"
#include "globals.h"
#include "pid.hpp"

bool ota_download_and_apply_update(const char* url, BinaryType type, bool restart=true);
void ota_setup(AsyncWebServer* server, CaptivePortal* captivePortal, PID* pidController);
void ota_check_for_update();

#endif
