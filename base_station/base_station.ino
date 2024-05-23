// Hubert Mathieu, math0701
// Antoine Hébert, heba0801
// May 2024

#include "BLEDevice.h"

// Define UUIDs:
static BLEUUID service("66fa6fda-2808-4f1a-8f75-aaf2ef2e8aa3");
static BLEUUID humidityCharac("76ca544c-b8c2-404b-a88a-d6aa312d4d9d");
static BLEUUID temperatureCharac("341019a0-c3d6-40ec-baee-aeb8127031c8");
static BLEUUID pressureCharac("2a7f3b8d-3583-4e14-b770-79ddaf2c1674");
static BLEUUID luminosityCharac("004f0e25-5c29-4d34-ad9c-ca73bade9bdc");
static BLEUUID windSpeedCharac("a6ed8fd2-9c68-4c2a-8de5-ac0aa9b3446e");
static BLEUUID windDirectionCharac("e9a27706-1d6d-42ed-a2e2-0119d49a1b50");
static BLEUUID rainCharac("1c95d5e3-d8f7-413a-bf3d-7a2e5d7be87e");

// keep track of the connection to server
static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;

// Define pointer for the BLE connection
static BLEAdvertisedDevice* myDevice;
// char. from the server that needs to be get
BLERemoteCharacteristic* char_hum;
BLERemoteCharacteristic* char_temp;
BLERemoteCharacteristic* char_prs;
BLERemoteCharacteristic* char_lum;
BLERemoteCharacteristic* char_wd_sp;
BLERemoteCharacteristic* char_wd_dir;
BLERemoteCharacteristic* char_rain;

// overide connection functions from client
class ClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
  }

  void onDisconnect(BLEClient* pclient) {
    connected = false;
    Serial.println("onDisconnect");
  }
};

// overide onResult function from client
class DeviceCallback: public BLEAdvertisedDeviceCallbacks {
  // Called for each device that is advertising
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());

    // if it matches our service we connect to it
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(service)) {
      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;
    }
  }
};

// called when a specific char. is notified
static void notifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
    // convert received bytes to float
    uint8_t b[length];
    for(int i = 1; i < length; i++) {
      b[i] = pData[i];
    }
    float data;
    memcpy(&data, &b, sizeof(data));

    std::string uuid = pBLERemoteCharacteristic->getUUID().toString();

    if (uuid == humidityCharac.toString())
      Serial.printf("Humidity : %f %%\n", data);
    else if (uuid == temperatureCharac.toString())
      Serial.printf("Temperature : %f Celsius\n", data);
    else if (uuid == pressureCharac.toString())
      Serial.printf("Pressure : %f PA\n", data);
    else if (uuid == luminosityCharac.toString())
      Serial.printf("Luminosity : %f lux\n", data);
    else if (uuid == windSpeedCharac.toString())
      Serial.printf("Wind speed : %f km/h\n", data);
    else if (uuid == windDirectionCharac.toString())
      Serial.printf("Wind direction : %s\n", data);
    else if (uuid == rainCharac.toString())
      Serial.printf("Rain accumulation : %f mm\n", data);
}

bool tryServerConnection() {
  Serial.print("Forming a connection to ");
  Serial.println(myDevice->getAddress().toString().c_str());
  
  BLEClient*  pClient  = BLEDevice::createClient();
  Serial.println(" - Created client");

  pClient->setClientCallbacks(new ClientCallback());

  pClient->connect(myDevice);
  Serial.println(" - Connected to server");

  BLERemoteService* pRemoteService = pClient->getService(service);
  if (pRemoteService == nullptr) {
    Serial.print("Failed to find our service UUID: ");
    Serial.println(service.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found our service");

  connected = true;
  char_hum = pRemoteService->getCharacteristic(humidityCharac);
  char_temp = pRemoteService->getCharacteristic(temperatureCharac);
  char_prs = pRemoteService->getCharacteristic(pressureCharac);
  char_lum = pRemoteService->getCharacteristic(luminosityCharac);
  char_wd_sp = pRemoteService->getCharacteristic(windSpeedCharac);
  char_wd_dir = pRemoteService->getCharacteristic(windDirectionCharac);
  char_rain = pRemoteService->getCharacteristic(rainCharac);
  if(tryCharacteristicConnection(pRemoteService, char_hum) == false)
    connected = false;
  else if(tryCharacteristicConnection(pRemoteService, char_temp) == false)
    connected = false;
  // else if(tryCharacteristicConnection(pRemoteService, char_prs) == false)
  //   connected = false;
  else if(tryCharacteristicConnection(pRemoteService, char_lum) == false)
    connected = false;
  else if(tryCharacteristicConnection(pRemoteService, char_wd_sp) == false)
    connected = false;
  // else if(tryCharacteristicConnection(pRemoteService, char_wd_dir) == false)
  //   connected = false;
  else if(tryCharacteristicConnection(pRemoteService, char_rain) == false)
    connected = false;

  if(connected == false) {
    pClient-> disconnect();
    Serial.println("At least one characteristic UUID not found");
    return false;
  }

  return true;
}

// the code will fail if the connection can't be done
bool tryCharacteristicConnection(BLERemoteService* pRemoteService, BLERemoteCharacteristic* l_BLERemoteChar) {
  if (l_BLERemoteChar == nullptr) {
    Serial.print("Failed to find one of the characteristics");
    Serial.print(l_BLERemoteChar->getUUID().toString().c_str());
    return false;
  }
  Serial.println(" - Found characteristic: " + String(l_BLERemoteChar->getUUID().toString().c_str()));

  // in our case the char. needs to be notifiable
  if(l_BLERemoteChar->canNotify())
    l_BLERemoteChar->registerForNotify(notifyCallback);

  return true;
}

void setup() {
  Serial.begin(115200);
  BLEDevice::init("");

  // global client settings
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new DeviceCallback());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
}

void loop() {
  if (doConnect == true) {
    if (tryServerConnection()) {
      Serial.println("We are now connected to the BLE Server.");
    } else {
      Serial.println("We have failed to connect to the server; there is nothin more we will do.");
    }
    doConnect = false;
  }

  if(doScan && !connected){
    BLEDevice::getScan()->start(0);
  }

  delay(1000);
}