#include <Wire.h>
#include <math.h>
#include <stdint.h>

const int SLAVE_ADD = 0x77;
const int PRODUCT_ID = 0x0D;
const int PRS_B2 = 0x00;
const int PRS_B1 = 0x01;
const int PRS_B0 = 0x02;
const int TMP_B2 = 0x03;
const int TMP_B1 = 0x04;
const int TMP_B0 = 0x05;
const int PRS_CFG = 0x06;
const int TMP_CFG = 0x07;
const int MEAS_CFG = 0x08;
const int C0 = 0x10;
const int COEF_SRCE = 0x28;


const int C00 = 0x13;
const int C20 = 0x1c;
const int C30 = 0x20;
const int C01 = 0x18;
const int C11 = 0x1A;
const int C21 = 0x1E;

const double KT = 524288.0;
const double KP = 7864320.0;

double tmp_sc = 0;

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

void bin(unsigned n) {
  if (n > 1)
    bin(n / 2);

  Serial.print(n % 2);
}

void print_byte(int64_t n) {
  bin(n);
  Serial.println();
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


void init() {
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

void setup() {
  Wire.begin();
  Serial.begin(9600);

  delay(1000);

  init();   
}

int to_signed(int value, int power) {
  if (value > pow(2, power - 1) - 1) {
    return value - pow(2, power);
  }
  return value;
}

double read_temp() {
  int tmp_raw = read(3, TMP_B2);

  int coef = read(3, C0);
  int c0 = to_signed(coef >> 12, 12);

  int c1 = to_signed(bitExtracted(coef, 12, 0), 12);

  tmp_sc = (double)tmp_raw/KT;

  return ((double)c0*0.5) + (double)c1*tmp_sc;
}

double read_prs() {
  int prs_raw = read(3, PRS_B2);

  int64_t c0010 = read(5, C00);
  int c00 = to_signed(c0010 >> 20, 20);
  int c10 = bitExtracted(c0010, 20, 0);

  print_byte(c10);
  Serial.println(c10);

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

void loop() {
  int config = read(1, MEAS_CFG);
  int tmp_rdy = bitExtracted(config, 1, 5);
  int prs_rdy = bitExtracted(config, 1, 4);

  if (tmp_rdy == 1) {
    double tmp = read_temp();
    Serial.print("temperature : ");
    Serial.println(tmp);
  }

  if (prs_rdy == 1) {
    double prs = read_prs();
    Serial.print("pressure : ");
    Serial.println(prs);
  }

  delay(500);
}