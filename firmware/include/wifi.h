#ifndef __WIFI_H__
#define __WIFI_H__

#include <ESP32Time.h>

#include "settings.h"

bool wifi_connect(settings_t* settings);
void wifi_update_time();

extern bool wifi_changed;
extern ESP32Time rtc;

#endif