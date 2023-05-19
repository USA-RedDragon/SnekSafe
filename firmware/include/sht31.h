#ifndef __SHT31_H__
#define __SHT31_H__

void sht31_setup();
void sht31_read(float* temperature, float* humidity);
void sht31_set_heater(bool enabled);

#endif