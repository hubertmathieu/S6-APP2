#include <HardwareSerial.h>

HardwareSerial SlaveCOMPort(1);

const int RXPIN = 16;
const int TXPIN = 17;
const int BAUDRATE = 9600;
const int SER_BUF_SIZE = 1024; // bytes

void setup() {
  SlaveCOMPort.setRxBufferSize(SER_BUF_SIZE);
  SlaveCOMPort.begin(BAUDRATE, SERIAL_8N1, RXPIN, TXPIN);

  Serial.begin(9600);
  
  delay(500);
}

void loop() {
  while (SlaveCOMPort.available()) {
    Serial.print(SlaveCOMPort.readString());
    SlaveCOMPort.println("Send data");
  }
  delay(1000);
}