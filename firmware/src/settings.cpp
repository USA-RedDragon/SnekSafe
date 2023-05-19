#include <Arduino.h>

#include "crc.h"
#include "settings.h"

Preferences prefs;

void settings_read(settings_t* dest) {
    settings_t read_settings = default_settings;
    *dest = default_settings;

    auto settings_size = prefs.getBytesLength("settings");
    char buffer[sizeof(settings_t)];
    prefs.getBytes("settings", buffer, settings_size);
    memcpy(&read_settings, buffer, settings_size);

    Serial.printf("NVM settings structSize: %d\n", read_settings.structSize);
    Serial.printf("Actual settings structSize: %d\n", sizeof(settings_t));
    Serial.printf("NVM settings crc: %04X\n", read_settings.crc);
    Serial.printf("NVM settings wifi: %s\n", read_settings.wifiSSID);

    if (read_settings.structSize > sizeof(settings_t)) {
        Serial.println("NVM settings structure size is bigger than expected, using defaults");
        settings_write(dest);
    } else if (read_settings.structSize == sizeof(settings_t)) {
        // Settings found of the correct size
        Serial.println("NVM settings found of the correct size");

        if (settings_check_crc(&read_settings)) {
            Serial.println("NVM settings CRC matches");
        } else {
            Serial.println("NVM settings CRC mismatch, using defaults");
            settings_write(dest);
            return;
        }

        *dest = read_settings;
    } else if (read_settings.structSize < sizeof(settings_t)) {
        // Settings found of a smaller size
        // We have potentially added new fields, so memcpy the old settings into the settings struct
        Serial.println("NVM settings found of a bigger size");
        
        // Check the CRC of the existing settings
        if (settings_check_crc(&read_settings)) {
            Serial.println("NVM settings CRC matches");
        } else {
            Serial.println("NVM settings CRC mismatch, using defaults");
            settings_write(dest);
            return;
        }

        memcpy(dest, &read_settings, read_settings.structSize);
        settings_write(&read_settings);
    } else {
        Serial.println("NVM settings not found, using defaults");
        settings_write(dest);
    }
}

void settings_write(settings_t* src) {
    Serial.println("Writing settings to NVM");
    src->structSize = sizeof(settings_t);
    src->crc = 0;
    src->crc = crc16((uint8_t*)src, src->structSize);
    Serial.printf("CRC: %04X\n", src->crc);
    prefs.putBytes("settings", src, sizeof(settings_t));
}

bool settings_check_crc(settings_t* settings) {
    bool ret = false;
    uint16_t read_crc = settings->crc;
    settings->crc = 0;
    if (crc16((uint8_t*)settings, settings->structSize-sizeof(uint16_t)) == read_crc) {
        ret = true;
    }
    settings->crc = read_crc;
    return ret;
}
