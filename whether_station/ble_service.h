// Hubert Mathieu, math0701
// Antoine Hébert, heba0801
// May 2024

#ifndef BLE_SERVICE_H
#define BLE_SERVICE_H

#define SERVICE_UUID        "66fa6fda-2808-4f1a-8f75-aaf2ef2e8aa3"
#define HUM_UUID            "76ca544c-b8c2-404b-a88a-d6aa312d4d9d"
#define TEMP_UUID           "341019a0-c3d6-40ec-baee-aeb8127031c8"
#define PRS_UUID            "2a7f3b8d-3583-4e14-b770-79ddaf2c1674"
#define LUM_UUID            "004f0e25-5c29-4d34-ad9c-ca73bade9bdc"
#define WS_UUID             "a6ed8fd2-9c68-4c2a-8de5-ac0aa9b3446e"
#define WD_UUID             "e9a27706-1d6d-42ed-a2e2-0119d49a1b50"
#define RAIN_UUID           "1c95d5e3-d8f7-413a-bf3d-7a2e5d7be87e"

void init_ble();
void send_data_to_base_station(float humidity, float luminosity, float pressure, float temperature, char* wind_direction, float wind_speed, float rain_accumulation);

#endif