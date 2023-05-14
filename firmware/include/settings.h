#ifndef __SETTINGS_H__
#define __SETTINGS_H__
#include <stdint.h>

typedef struct
{
    uint32_t structSize;

    char captivePortalSSID[32+1]; // 32 chars + null terminator
    char captivePortalPassword[63+1]; // 63 chars + null terminator
    uint8_t captivePortalChannel; // 1-13, wifi channel

    // Put all new settings before this
    uint16_t crc;
}
__attribute__((packed)) settings_t;

const settings_t default_settings =
{
    sizeof(settings_t), // structSize
    "SnekSafe", // captivePortalSSID
    "snekysnek", // captivePortalPassword
    6, // captivePortalChannel
    0 // crc
};

void settings_read(settings_t* dest);
void settings_write(settings_t* src);
bool settings_check_crc(settings_t* settings);

#endif
