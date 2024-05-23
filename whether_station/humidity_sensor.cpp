// Hubert Mathieu, math0701
// Antoine Hébert, heba0801
// May 2024

#include "humidity_sensor.h"
#include <Arduino.h>

float get_humidity() {
  int i, j;
  int duree[42];
  unsigned long pulse;
  byte data[5];
  float humidite;
  float temperature;

  pinMode(HUMIDITY_SENSOR_PIN, OUTPUT_OPEN_DRAIN);
  digitalWrite(HUMIDITY_SENSOR_PIN, HIGH);
  delay(250);
  digitalWrite(HUMIDITY_SENSOR_PIN, LOW);
  delay(20);
  digitalWrite(HUMIDITY_SENSOR_PIN, HIGH);
  delayMicroseconds(40);
  pinMode(HUMIDITY_SENSOR_PIN, INPUT_PULLUP);

  while (digitalRead(HUMIDITY_SENSOR_PIN) == HIGH);
  i = 0;

  do {
        pulse = pulseIn(HUMIDITY_SENSOR_PIN, HIGH);
        duree[i] = pulse;
        i++;
  } while (pulse != 0);

  if (i != 42) 
    Serial.printf(" Erreur timing \n"); 

  for (i=0; i<5; i++) {
    data[i] = 0;
    for (j = ((8*i)+1); j < ((8*i)+9); j++) {
      data[i] = data[i] * 2;
      if (duree[j] > 50) {
        data[i] = data[i] + 1;
      }
    }
  }

  if ( (data[0] + data[1] + data[2] + data[3]) != data[4] ) 
    Serial.println(" Erreur checksum");

  humidite = data[0] + (data[1] / 256.0);
  return humidite;
}