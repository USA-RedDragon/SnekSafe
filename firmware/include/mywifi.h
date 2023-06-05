#ifndef __WIFI_H__
#define __WIFI_H__

#include "settings.h"

bool wifi_connect(settings_t* settings);
void wifi_tick();

#endif