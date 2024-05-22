#include "ble_service.h"
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

BLEServer* pServer = NULL;
BLECharacteristic* pCharHUM = NULL;
BLECharacteristic* pCharTEMP    = NULL;
BLECharacteristic* pCharPRS     = NULL;
BLECharacteristic* pCharLUM     = NULL;
BLECharacteristic* pCharWS      = NULL;
BLECharacteristic* pCharWD      = NULL;
BLECharacteristic* pCharRain    = NULL;
BLEDescriptor *pDescr;
BLE2902 *pBLE2902;
BLE2902 *pBLE2902_2;
BLE2902 *pBLE2902_3;
BLE2902 *pBLE2902_4;
BLE2902 *pBLE2902_5;
BLE2902 *pBLE2902_6;
BLE2902 *pBLE2902_7;

bool deviceConnected = false;
bool oldDeviceConnected = false;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

void init_ble() {
  // Create the BLE Device
  BLEDevice::init("ESP32");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(BLEUUID(SERVICE_UUID), 40);

  // Create a BLE Characteristic
  pCharHUM = pService->createCharacteristic(
                      HUM_UUID,
                      BLECharacteristic::PROPERTY_NOTIFY
                    );
  pCharTEMP = pService->createCharacteristic(
                    TEMP_UUID,
                    BLECharacteristic::PROPERTY_NOTIFY
                  );
  pCharPRS  = pService->createCharacteristic(
                      PRS_UUID,
                      BLECharacteristic::PROPERTY_NOTIFY
                    );
  pCharLUM  = pService->createCharacteristic(
                      LUM_UUID,
                      BLECharacteristic::PROPERTY_NOTIFY
                    );
  pCharWS   = pService->createCharacteristic(
                      WS_UUID,
                      BLECharacteristic::PROPERTY_NOTIFY
                    );
  pCharWD   = pService->createCharacteristic(
                      WD_UUID,
                      BLECharacteristic::PROPERTY_NOTIFY
                    );
  pCharRain = pService->createCharacteristic(
                      RAIN_UUID,
                      BLECharacteristic::PROPERTY_NOTIFY
                    );                 

  // Create a BLE Descriptor
  
  // pDescr = new BLEDescriptor((uint16_t)0x2901);
  // pDescr->setValue("A very interesting variable");
  // pCharHUM->addDescriptor(pDescr);
  
  pBLE2902 = new BLE2902();
  pBLE2902->setNotifications(true);

  pBLE2902_2 = new BLE2902();
  pBLE2902_2->setNotifications(true);

  pBLE2902_3 = new BLE2902();
  pBLE2902_3->setNotifications(true);

  pBLE2902_4 = new BLE2902();
  pBLE2902_4->setNotifications(true);

  pBLE2902_5 = new BLE2902();
  pBLE2902_5->setNotifications(true);

  pBLE2902_6 = new BLE2902();
  pBLE2902_6->setNotifications(true);

  pBLE2902_7 = new BLE2902();
  pBLE2902_7->setNotifications(true);

  pCharHUM->addDescriptor(pBLE2902);
  pCharTEMP->addDescriptor(pBLE2902_2);
  pCharPRS  ->addDescriptor(pBLE2902_3);
  pCharLUM  ->addDescriptor(pBLE2902_4);
  pCharWS   ->addDescriptor(pBLE2902_5);
  pCharWD   ->addDescriptor(pBLE2902_6);
  pCharRain ->addDescriptor(pBLE2902_7);

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  Serial.println("Waiting a client connection to notify...");
}

void send_data_to_base_station(float humidity, float luminosity, float pressure, float temperature, char* wind_direction, float wind_speed, float rain_accumulation) {
    // notify changed value
    if (deviceConnected) {
        pCharHUM->setValue(humidity);
        pCharPRS  ->setValue(pressure);
        pCharLUM  ->setValue(luminosity);
        pCharWS   ->setValue(wind_speed);
        pCharWD   ->setValue(wind_direction);
        pCharRain ->setValue(rain_accumulation);
        pCharTEMP->setValue(temperature);

        pCharHUM->notify();
        pCharTEMP->notify();
        pCharPRS  ->notify();
        pCharLUM  ->notify();
        pCharWS   ->notify();
        pCharWD   ->notify();
        pCharRain ->notify();
    }
    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
}