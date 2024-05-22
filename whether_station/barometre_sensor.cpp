#include "barometre_sensor.h"
#include <Wire.h>
#include <math.h>
#include <stdint.h>
#include <Arduino.h>

double tmp_sc = 0;

void bin(unsigned n) {
  if (n > 1)
    bin(n / 2);

  Serial.print(n % 2);
}

void print_byte(int64_t n) {
  bin(n);
  Serial.println();
}

// Function to set the kth bit of n
int setBit(int n, int k)
{
    return (n | (1 << (k)));
}

int bitExtracted(int64_t number, int k, int p)
{
    return (((1 << k) - 1) & (number >> (p)));
}
  
// Function to clear the kth bit of n
int clearBit(int n, int k) {
    return (n & (~(1 << (k))));
}

int to_signed(int value, int power) {
  if (value > pow(2, power - 1) - 1) {
    return value - pow(2, power);
  }
  return value;
}

void write(int data, int add) {
  Wire.beginTransmission(SLAVE_ADD);
  Wire.write(add);
  Wire.write(data);
  Wire.endTransmission(true);
}

int64_t read(int size, int add) {
  Wire.beginTransmission(SLAVE_ADD);
  Wire.write(add);
  Wire.endTransmission(true);

  int64_t buffer = 0;

  Wire.requestFrom(SLAVE_ADD, size);
  while(Wire.available()) {
    int c = Wire.read();
    buffer = (buffer << 8) | c;
  }

  return buffer;
}

void temp_init() {
  int coef = read(1, COEF_SRCE);
  int tmp_ext = read(1, TMP_CFG);

  if (coef >= 128) {
    tmp_ext = setBit(tmp_ext, 7);
  } else {
    tmp_ext = clearBit(tmp_ext, 7);
  }

  // sampling rate (8 sample per sec)
  tmp_ext = setBit(tmp_ext, 4);
  tmp_ext = setBit(tmp_ext, 5);

  write(tmp_ext, TMP_CFG);
}

void prs_init() {
  int prs_ext = read(1, PRS_CFG);

  // sampling rate (8 sample per sec)
  prs_ext = setBit(prs_ext, 4);
  prs_ext = setBit(prs_ext, 5);
  // oversampling rate (8)
  prs_ext = setBit(prs_ext, 0);
  prs_ext = setBit(prs_ext, 1);

  write(prs_ext, PRS_CFG);
}


void barometre_init() {
    Wire.begin();
    // set Backgroud mode
    int config = read(1, MEAS_CFG);

    config = setBit(config, 0);
    config = setBit(config, 1);  
    config = setBit(config, 2);  

    write(config, MEAS_CFG);

    // set precision
    temp_init();
    prs_init();
}

double get_temperature() {
    int tmp_raw = read(3, TMP_B2);

    int coef = read(3, C0);
    int c0 = to_signed(coef >> 12, 12);

    int c1 = to_signed(bitExtracted(coef, 12, 0), 12);

    tmp_sc = (double)tmp_raw/KT;

    return ((double)c0*0.5) + (double)c1*tmp_sc;
}

double get_pressure() {
  int prs_raw = read(3, PRS_B2);

  int64_t c0010 = read(5, C00);
  int c00 = to_signed(c0010 >> 20, 20);
  int c10 = bitExtracted(c0010, 20, 0);

  if (c10 > pow(2, 19) - 1) {
    c10 = c10 - pow(2, 20);
  }
  int c20 = to_signed(read(2, C20), 16);
  int c30 = to_signed(read(2, C30), 16);
  int c01 = to_signed(read(2, C01), 16);
  int c11 = to_signed(read(2, C11), 16);
  int c21 = to_signed(read(2, C21), 16);

  double prs_rawsc = (double)prs_raw/KP;

  return c00 + prs_rawsc * (c10 + prs_rawsc * (double)(c20 + prs_rawsc * c30)) + tmp_sc * c01 + tmp_sc * prs_rawsc * (double)(c11 + prs_rawsc * c21);
}