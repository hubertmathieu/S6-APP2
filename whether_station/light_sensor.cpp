// Hubert Mathieu, math0701
// Antoine Hébert, heba0801
// May 2024

#include "light_sensor.h"
#include <Arduino.h>

/*
 * Read analog from light sensor and returns luminosity in lux
 * return float
*/
float get_luminosity() {
  int value = analogRead(LIGHT_SENSOR_PIN);
  float voltage = value * (3.3 / 4095.0);
  float current = voltage / 68.0;

  return (float)current * 100.0 / 0.2395;
}