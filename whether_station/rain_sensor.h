// Hubert Mathieu, math0701
// Antoine Hébert, heba0801
// May 2024

#ifndef RAIN_SENSOR_H
#define RAIN_SENSOR_H

// amount of rain by tick
const float RAIN_MM = 0.2794;
const int RAIN_PIN = 23;

float get_rain_accumulation();
void increment_rain_tick();

#endif