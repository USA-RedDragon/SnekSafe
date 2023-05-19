#ifndef __WIFI_H__
#define __WIFI_H__

#include "settings.h"

bool wifi_connect(settings_t* settings);
void wifi_update_time();
void wifi_update_timezone(settings_t* settings);

extern bool wifi_changed;

#endif