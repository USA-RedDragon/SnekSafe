#include <Arduino.h>
#include <DFRobot_SHT3x.h>

DFRobot_SHT3x sht3x(&Wire,/*address=*/0x44);

void sht31_setup() {
    if (sht3x.begin() != 0) {
        Serial.println("Failed to initialize the temperature sensor");
        ESP.restart();
        return;
    }

    if(!sht3x.softReset()){
        Serial.println("Failed to reset the temperature sensor");
        ESP.restart();
        return;
    }

    if(!sht3x.startPeriodicMode(sht3x.eMeasureFreq_10Hz)){
        Serial.println("Failed to enter the periodic temperature sensing mode");
        ESP.restart();
        return;
    }
}

bool sht31_read(float* temperature, float* humidity) {
    auto data = sht3x.readTemperatureAndHumidity();
    if (data.ERR != 0) {
        Serial.printf("Failed to read temperature and humidity: %d\n", data.ERR);
        return false;
    }

    bool ret = false;

    if (!isnan(data.TemperatureF)) {
        ret = true;
        *temperature = data.TemperatureF;
    } else {
        Serial.println("Failed to read temperature");
    }

    if (!isnan(data.Humidity)) {
        *humidity = data.Humidity;
    } else {
        Serial.println("Failed to read humidity");
        ret = false;
    }

    return ret;
}

void sht31_set_heater(bool enabled) {
    if (enabled) {
        if (!sht3x.heaterEnable()) {
            Serial.println("Failed to enable sensor heater");
        }
    } else {
        if (!sht3x.heaterDisable()) {
            Serial.println("Failed to disable sensor heater");
        }
    }
}
