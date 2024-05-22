#ifndef WIND_SENSOR_H
#define WIND_SENSOR_H

const float WIND_SPEED_KMH = 2.4;
const int WIND_SPEED_PIN = 27;
const int WIND_DIR_PIN = 35;

float get_wind_speed();
char* get_wind_direction();
void increment_wind_tick();

#endif