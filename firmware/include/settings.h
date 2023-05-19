#ifndef __SETTINGS_H__
#define __SETTINGS_H__
#include <stdint.h>

#include <Preferences.h>

extern Preferences prefs;

// ONLY ADD NEW SETTINGS TO THE END OF THIS STRUCT BEFORE THE CRC
// OTHERWISE YOU WILL BREAK COMPATIBILITY WITH EXISTING SETTINGS
// DO NOT CHANGE THE ORDER OF EXISTING SETTINGS
// DO NOT CHANGE THE DATA TYPE OF EXISTING SETTINGS, INCLUDING ARRAY LENGTHS
// DO NOT REMOVE EXISTING SETTINGS, THIS IS NOT IMPLEMENTED YET

#define NUMBER_OF_SETTINGS 10
typedef struct
{
    uint32_t structSize;

    char captivePortalSSID[32+1]; // 32 chars + null terminator
    char captivePortalPassword[63+1]; // 63 chars + null terminator
    uint8_t captivePortalChannel; // 1-13, wifi channel
    uint8_t temperatureSetpoint; // 0-255, temperature setpoint in degrees F
    time_t lightOnTime; // time_t, time of day to turn on light
    time_t lightOffTime; // time_t, time of day to turn off light
    char wifiSSID[32+1]; // 32 chars + null terminator
    char wifiPassword[63+1]; // 63 chars + null terminator
    char mdnsName[32+1]; // 32 chars + null terminator

    // Put all new settings before this
    uint16_t crc;
}
__attribute__((packed)) settings_t;

const settings_t default_settings =
{
    sizeof(settings_t), // structSize
    "SnekSafe", // captivePortalSSID
    "", // captivePortalPassword
    6, // captivePortalChannel
    82, // temperatureSetpoint
    0, // lightOnTime
    0, // lightOffTime
    "", // wifiSSID
    "", // wifiPassword
    "snek", // mdnsName
    0 // crc
};

void settings_read(settings_t* dest);
void settings_write(settings_t* src);
bool settings_check_crc(settings_t* settings);

#endif
