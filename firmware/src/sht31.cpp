#include <Arduino.h>
#include <Adafruit_SHT31.h>

Adafruit_SHT31 sht31 = Adafruit_SHT31();

void sht31_setup() {
    if (!sht31.begin(0x44)) {
        Serial.println("Couldn't find SHT31");
        ESP.restart();
        return;
    }

    Serial.print("Heater Enabled State: ");
    if (sht31.isHeaterEnabled()) {
        Serial.println("ENABLED");
    } else {
        Serial.println("DISABLED");
    }
}

bool sht31_read(float* temperature, float* humidity) {
    float t = sht31.readTemperature();
    float h = sht31.readHumidity();

    bool ret = false;

    if (!isnan(t)) {
        // convert to fahrenheit
        t = (t * 1.8) + 32;
        ret = true;
        *temperature = t;
    } else {
        Serial.println("Failed to read temperature");
    }

    if (!isnan(h)) {
        *humidity = h;
    } else {
        Serial.println("Failed to read humidity");
        ret = false;
    }

    return ret;
}

void sht31_set_heater(bool enabled) {
    if (enabled) {
        sht31.heater(true);
    } else {
        sht31.heater(false);
    }
}

bool sht31_get_heater() {
    return sht31.isHeaterEnabled();
}
