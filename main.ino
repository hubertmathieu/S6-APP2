#include <Wire.h>
#include <math.h>
#include <stdint.h>
#include "barometre_sensor.h"
#include "humidity_sensor.h"
#include "light_sensor.h"
#include "rain_sensor.h"
#include "wind_sensor.h"

void setup() {
    barometre_init();
    attachInterrupt(WIND_SPEED_PIN, increment_wind_tick, RISING);
    attachInterrupt(RAIN_PIN, increment_rain_tick, RISING);

    Serial.begin(9600);
    delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
  get_humidity();
  get_luminosity();
  get_pressure();
  get_temperature();
  get_wind_direction();
  get_wind_speed();
  get_rain_accumulation();

  delay(1000);
}