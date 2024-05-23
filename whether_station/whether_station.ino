// Hubert Mathieu, math0701
// Antoine Hébert, heba0801
// May 2024

#include "barometre_sensor.h"
#include "humidity_sensor.h"
#include "light_sensor.h"
#include "rain_sensor.h"
#include "wind_sensor.h"
#include "ble_service.h"

void setup() {
  barometre_init();
  // keep track of tick counts for anenometer
  attachInterrupt(WIND_SPEED_PIN, increment_wind_tick, RISING);
  attachInterrupt(RAIN_PIN, increment_rain_tick, RISING);

  Serial.begin(9600);
  delay(1000);
  
  // ble set up for creating server device
  init_ble();
}

void loop() {
  // Sensor values
  Serial.println("----------------------------------------------");
  float humidity = get_humidity();
  Serial.printf("Humidity : %f %%\n", humidity);
  float luminosity = get_luminosity();
  Serial.printf("Luminosity : %f lux\n", luminosity);
  float pressure = get_pressure();
  Serial.printf("Pressure : %f PA\n", pressure);
  float temperature = get_temperature();
  Serial.printf("Temperature : %f Celsius\n", temperature);
  char* wind_direction = get_wind_direction();
  Serial.printf("Wind direction : %s\n", wind_direction);
  float wind_speed = get_wind_speed();
  Serial.printf("Wind speed : %f km/h\n", wind_speed);
  float rain_accumulation = get_rain_accumulation();
  Serial.printf("Rain accumulation : %f mm\n", rain_accumulation);
  Serial.println("----------------------------------------------");

  // ble com (server->client)
  send_data_to_base_station(humidity, luminosity, pressure, temperature, wind_direction, wind_speed, rain_accumulation);

  delay(1000);
}