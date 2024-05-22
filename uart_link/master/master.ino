#include <HardwareSerial.h>

HardwareSerial MasterCOMPort(2);

const int RXPIN = 16;
const int TXPIN = 17;
const int BAUDRATE = 9600;
const int SER_BUF_SIZE = 1024; // bytes

void setup() {
  MasterCOMPort.setRxBufferSize(SER_BUF_SIZE);
  MasterCOMPort.begin(BAUDRATE, SERIAL_8N1, RXPIN, TXPIN);

  Serial.begin(9600);

  delay(500);
}

void loop() {
  MasterCOMPort.println("Request data");

  while (MasterCOMPort.available()) {
    Serial.print(MasterCOMPort.readString());
  }
  delay(1000);
}