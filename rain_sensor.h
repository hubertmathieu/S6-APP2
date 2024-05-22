#ifndef RAIN_SENSOR_H
#define RAIN_SENSOR_H

const float RAIN_MM = 0.2794;
const int RAIN_PIN = 23;

float get_rain_accumulation();
void increment_rain_tick();

#endif