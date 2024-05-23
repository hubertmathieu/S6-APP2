// Hubert Mathieu, math0701
// Antoine Hébert, heba0801
// May 2024

#include "ble_service.h"
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

BLEServer* server;
BLECharacteristic* char_hum;
BLECharacteristic* char_temp;
BLECharacteristic* char_prs;
BLECharacteristic* char_lum;
BLECharacteristic* char_wd_sp;
BLECharacteristic* char_wd_dir;
BLECharacteristic* char_rain;
BLEDescriptor *pDescr;
BLE2902 *desc_hum;
BLE2902 *desc_temp;
BLE2902 *desc_pres;
BLE2902 *desc_lum;
BLE2902 *desc_wd_sp;
BLE2902 *desc_wd_dir;
BLE2902 *desc_rain;

// keep track of client connection
bool deviceConnected = false;
bool oldDeviceConnected = false;

// overide server functions
class ServerCallback: public BLEServerCallbacks {
    void onConnect(BLEServer* server) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* server) {
      deviceConnected = false;
    }
};

// creating ble server
void init_ble() {
  // device global parameters
  BLEDevice::init("ESP32");

  server = BLEDevice::createServer();
  server->setCallbacks(new ServerCallback());

  BLEService *service = server->createService(BLEUUID(SERVICE_UUID), 40);

  // existing characteritics for the weather station
  char_hum = service->createCharacteristic(HUM_UUID, BLECharacteristic::PROPERTY_NOTIFY);
  char_temp = service->createCharacteristic(TEMP_UUID, BLECharacteristic::PROPERTY_NOTIFY);
  char_prs  = service->createCharacteristic(PRS_UUID, BLECharacteristic::PROPERTY_NOTIFY);
  char_lum  = service->createCharacteristic(LUM_UUID, BLECharacteristic::PROPERTY_NOTIFY);
  char_wd_sp   = service->createCharacteristic(WS_UUID, BLECharacteristic::PROPERTY_NOTIFY);
  char_wd_dir   = service->createCharacteristic(WD_UUID, BLECharacteristic::PROPERTY_NOTIFY);
  char_rain = service->createCharacteristic(RAIN_UUID, BLECharacteristic::PROPERTY_NOTIFY);                 
  
  // creating descriptors for each char.
  desc_hum = new BLE2902();
  desc_hum->setNotifications(true);
  char_hum->addDescriptor(desc_hum);

  desc_temp = new BLE2902();
  desc_temp->setNotifications(true);
  char_temp->addDescriptor(desc_temp);

  desc_pres = new BLE2902();
  desc_pres->setNotifications(true);
  char_prs->addDescriptor(desc_pres);

  desc_lum = new BLE2902();
  desc_lum->setNotifications(true);
  char_lum->addDescriptor(desc_lum);

  desc_wd_sp = new BLE2902();
  desc_wd_sp->setNotifications(true);
  char_wd_sp->addDescriptor(desc_wd_sp);

  desc_wd_dir = new BLE2902();
  desc_wd_dir->setNotifications(true);
  char_wd_dir->addDescriptor(desc_wd_dir);

  desc_rain = new BLE2902();
  desc_rain->setNotifications(true);
  char_rain->addDescriptor(desc_rain);

  service->start();

  // display on network
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);
  BLEDevice::startAdvertising();
  Serial.println("Waiting for a client connection...");
}

void send_data_to_base_station(float humidity, float luminosity, float pressure, float temperature, char* wind_direction, float wind_speed, float rain_accumulation) {
  // if not connected => advertise on the network for connection
  if (!deviceConnected && oldDeviceConnected) {
    delay(500);
    server->startAdvertising();
    Serial.println("start advertising");
    oldDeviceConnected = deviceConnected;
  }

  if (deviceConnected && !oldDeviceConnected) {
    oldDeviceConnected = deviceConnected;
  }

  if (deviceConnected) {
    char_hum->setValue(humidity);
    char_prs->setValue(pressure);
    char_lum->setValue(luminosity);
    char_wd_sp->setValue(wind_speed);
    char_wd_dir->setValue(wind_direction);
    char_rain->setValue(rain_accumulation);
    char_temp->setValue(temperature);

    // the client char. callbacks will be called on notify
    char_hum->notify();
    char_temp->notify();
    char_prs->notify();
    char_lum->notify();
    char_wd_sp->notify();
    char_wd_dir->notify();
    char_rain ->notify();
  }
}