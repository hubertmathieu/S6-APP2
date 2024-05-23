// Hubert Mathieu, math0701
// Antoine Hébert, heba0801
// May 2024

#include "wind_sensor.h"
#include <Arduino.h>

int wind_ticks = 0;

// called by the interupt
void increment_wind_tick() {
    wind_ticks++; 
}

// function to get the wind speed of the last second
float get_wind_speed() {
    float wind_speed = WIND_SPEED_KMH * wind_ticks;
    wind_ticks = 0;
    return wind_speed;
}

// converting voltage to cardinal direction
char* get_wind_direction() {
    int analog_value = analogRead(WIND_DIR_PIN);
   
    switch(analog_value) {
      case 2225-50 ... 2225+50:
        return "Nord";
        break;
      case 3830-50 ... 3830+50:
        return "Nord-Est 30 deg";
        break;
      case 3472-50 ... 3472+50:
        return "Nord-Est 60 deg";
        break;
      case 2640-50 ... 2640+50:
        return "Est";
        break;
      case 986-50 ... 986+50:
       return "Nord-Ouest 300 deg";
        break;
      case 2359 ... 2359 + 1:
        return "Nord-Ouest 330 deg";
        break;
      case 200-1 ... 200+1:
        return "Nord-Ouest";
        break;
      case 2990-50 ... 2990+50:
        return "Sud-Est 120 deg";
        break;
      case 1459-50 ... 1459+50:
        return "Sud-Est 150 deg";
        break;
      case 1658-50 ... 1658+50:
        return "Sud";
        break;
      case 194-1 ... 194+1:
        return "Sud-Ouest 210 deg";
        break;
      case 563-50 ... 563+50:
        return "Sud-Ouest 240 deg";
        break;
      case 342-50 ... 342+50:
        return "Ouest";
        break;   
    }
}