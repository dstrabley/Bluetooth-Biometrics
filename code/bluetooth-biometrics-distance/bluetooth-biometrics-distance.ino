#include "Arduino.h"
#include <60ghzbreathheart.h>
#include <ArduinoBLE.h>

// Define custom 128-bit UUIDs
BLEService heartRateService("19b10000-e8f2-537e-4f6c-d104768a1214");
BLEIntCharacteristic heartRateChar("19b10001-e8f2-537e-4f6c-d104768a1214", BLERead | BLENotify);
BLEIntCharacteristic breathRateChar("19b10002-e8f2-537e-4f6c-d104768a1214", BLERead | BLENotify); 
BLEIntCharacteristic distanceChar("19b10003-e8f2-537e-4f6c-d104768a1214", BLERead | BLENotify);

BreathHeart_60GHz radar = BreathHeart_60GHz(&Serial1);
bool isBreathHeart = true;  // Flag to switch between functions

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);

  while(!Serial);

  Serial.println("Ready");

  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");
    while (1);
  }

  BLE.setLocalName("Heart Rate Monitor");
  BLE.setAdvertisedService(heartRateService);
  heartRateService.addCharacteristic(heartRateChar);
  heartRateService.addCharacteristic(breathRateChar);
  heartRateService.addCharacteristic(distanceChar);
  BLE.addService(heartRateService);

  heartRateChar.writeValue(0);
  breathRateChar.writeValue(0);
  distanceChar.writeValue(0);

  BLE.advertise();
}

void loop() {
  BLEDevice central = BLE.central();
  
  if (central) {
    while (central.connected()) {
      if(isBreathHeart) {
        radar.Breath_Heart();
      } else {
        radar.HumanExis_Func();
      }
      
      isBreathHeart = !isBreathHeart;

      if(radar.sensor_report != 0x00){
        switch(radar.sensor_report){
          case HEARTRATEVAL:
           // Serial.println(radar.heart_rate, DEC);
            heartRateChar.writeValue(radar.heart_rate);
            break;
          case BREATHVAL:
            //Serial.println(radar.breath_rate, DEC);
            breathRateChar.writeValue(radar.breath_rate);
            break;
          case DISVAL:
            int distanceInches = int(radar.distance * 39.3701);
            distanceChar.writeValue(distanceInches);
            break;
        }
      }

      delay(200);
    }
  }
}
