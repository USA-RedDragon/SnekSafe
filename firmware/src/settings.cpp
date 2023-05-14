#include <Arduino.h>
#include <EEPROM.h>

#include "crc.h"
#include "settings.h"

void settings_read(settings_t* dest) {
    EEPROM.begin(sizeof(settings_t));

    settings_t read_settings;
    *dest = default_settings;

    EEPROM.get(0, read_settings);
    Serial.printf("EEPROM settings structSize: %d\n", read_settings.structSize);
    Serial.printf("Actual settings structSize: %d\n", sizeof(settings_t));
    Serial.printf("EEPROM settings crc: %04X\n", read_settings.crc);

    if (read_settings.structSize > sizeof(settings_t)) {
        Serial.println("EEPROM settings structure size is bigger than expected, using defaults");
        settings_write(dest);
    } else if (read_settings.structSize == sizeof(settings_t)) {
        // Settings found of the correct size
        Serial.println("EEPROM settings found of the correct size");

        if (settings_check_crc(&read_settings)) {
            Serial.println("EEPROM settings CRC matches");
        } else {
            Serial.println("EEPROM settings CRC mismatch, using defaults");
            settings_write(dest);
            return;
        }

        *dest = read_settings;
    } else if (read_settings.structSize < sizeof(settings_t)) {
        // Settings found of a bigger size
        // We have potentially added new fields, so memcpy the old settings into the settings struct
        Serial.println("EEPROM settings found of a bigger size");
        
        // Check the CRC of the existing settings
        if (settings_check_crc(&read_settings)) {
            Serial.println("EEPROM settings CRC matches");
        } else {
            Serial.println("EEPROM settings CRC mismatch, using defaults");
            settings_write(dest);
            return;
        }

        memcpy(dest, &read_settings, read_settings.structSize);
        settings_write(dest);
    } else {
        Serial.println("EEPROM settings not found, using defaults");
        settings_write(dest);
    }
}

void settings_write(settings_t* src) {
    Serial.println("Writing settings to EEPROM");
    src->structSize = sizeof(settings_t);
    src->crc = crc16((uint8_t*)src, src->structSize);
    Serial.printf("CRC: %04X\n", src->crc);
    EEPROM.put(0, *src);
    EEPROM.commit();
}

bool settings_check_crc(settings_t* settings) {
    bool ret = false;
    uint16_t read_crc = settings->crc;
    settings->crc = 0;
    if (crc16((uint8_t*)settings, settings->structSize) == read_crc) {
        ret = true;
    }
    settings->crc = read_crc;
    return ret;
}
