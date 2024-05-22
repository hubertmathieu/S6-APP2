#ifndef BAROMETRE_SENSOR_H
#define BAROMETRE_SENSOR_H

// Config addresse
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

// Coefficient addresse
const int C00 = 0x13;
const int C20 = 0x1c;
const int C30 = 0x20;
const int C01 = 0x18;
const int C11 = 0x1A;
const int C21 = 0x1E;

// Constant for temperature and pressure calculation
const double KT = 524288.0;
const double KP = 7864320.0;

void barometre_init();
double get_temperature();
double get_pressure();

#endif


