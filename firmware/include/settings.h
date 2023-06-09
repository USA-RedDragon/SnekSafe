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

typedef struct
{
    uint32_t structSize;
    uint16_t crc;
    // Put all new settings after this

    char captivePortalSSID[32+1]; // 32 chars + null terminator
    char captivePortalPassword[63+1]; // 63 chars + null terminator
    uint8_t captivePortalChannel; // 1-13, wifi channel
    double temperatureSetpoint; // temperature setpoint in degrees F
    uint8_t lightOnHour; // 0-23, hour of day to turn on light
    uint8_t lightOnMinute; // 0-59, time of day to turn on light
    uint8_t lightOffHour; // 0-23, hour of day to turn off light
    uint8_t lightOffMinute; // 0-59, time of day to turn off light
    char wifiSSID[32+1]; // 32 chars + null terminator
    char wifiPassword[63+1]; // 63 chars + null terminator
    char mdnsName[32+1]; // 32 chars + null terminator
    double pGain; // PID proportional gain
    double iGain; // PID integral gain
    double dGain; // PID derivative gain
    double iMax; // PID integral max
    double iMin; // PID integral min
}
__attribute__((packed)) settings_t;

const settings_t default_settings =
{
    sizeof(settings_t), // structSize
    0, // crc
    "SnekSafe", // captivePortalSSID
    "", // captivePortalPassword
    6, // captivePortalChannel
    82, // temperatureSetpoint
    15, // lightOnHour
    0, // lightOnMinute
    3, // lightOffHour
    0, // lightOffMinute
    "", // wifiSSID
    "", // wifiPassword
    "snek", // mdnsName,
    1, // pGain
    0, // iGain
    0, // dGain
    100, // iMax
    1, // iMin
};

void settings_read(settings_t* dest);
void settings_write(settings_t* src);
bool settings_check_crc(settings_t* settings);

#endif
