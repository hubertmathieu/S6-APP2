#include "rain_sensor.h"

int rain_ticks = 0;

void increment_rain_tick() {
    rain_ticks++; 
}

float get_rain_accumulation() {
    float rain = RAIN_MM * rain_ticks;
    rain_ticks = 0;
    return rain;
}