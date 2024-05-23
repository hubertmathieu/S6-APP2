// Hubert Mathieu, math0701
// Antoine Hébert, heba0801
// May 2024

#include "rain_sensor.h"

int rain_ticks = 0;

// called by the interupt
void increment_rain_tick() {
    rain_ticks++; 
}

// get amount of rain for one second
float get_rain_accumulation() {
    float rain = RAIN_MM * rain_ticks;
    rain_ticks = 0;
    return rain;
}