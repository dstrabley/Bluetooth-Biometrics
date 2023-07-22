#include "Arduino.h"
#include <60ghzbreathheart.h>
#include <ArduinoBLE.h>

// Define custom 128-bit UUIDs
BLEService heartRateService("19b10000-e8f2-537e-4f6c-d104768a1214");
BLEIntCharacteristic heartRateChar("19b10001-e8f2-537e-4f6c-d104768a1214", BLERead | BLENotify);
BLEIntCharacteristic breathRateChar("19b10002-e8f2-537e-4f6c-d104768a1214", BLERead | BLENotify); // New Characteristic for breath rate

// Instantiate the heart rate monitor
BreathHeart_60GHz radar = BreathHeart_60GHz(&Serial1);

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);

  while(!Serial);   //When the serial port is opened, the program starts to execute.

  Serial.println("Ready");

  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");
    while (1);
  }

  BLE.setLocalName("Heart Rate Monitor");
  BLE.setAdvertisedService(heartRateService);
  heartRateService.addCharacteristic(heartRateChar);
  heartRateService.addCharacteristic(breathRateChar); // Added breath rate to the service
  BLE.addService(heartRateService);

  heartRateChar.writeValue(0);
  breathRateChar.writeValue(0); // Initialize breath rate

  BLE.advertise();
}

void loop() {
  BLEDevice central = BLE.central();
  if (central) {
    while (central.connected()) {
      radar.Breath_Heart();           //Breath and heartbeat information output
      if(radar.sensor_report != 0x00){
        switch(radar.sensor_report){
          case HEARTRATEVAL:
            Serial.println(radar.heart_rate, DEC);
            heartRateChar.writeValue(radar.heart_rate);
            break;
          case BREATHVAL: // Added case for breath rate
            Serial.println(radar.breath_rate, DEC);
            breathRateChar.writeValue(radar.breath_rate);
            break;
        }
      }
      delay(200);
    }
  } else {
    radar.Breath_Heart();           //Breath and heartbeat information output
    if(radar.sensor_report != 0x00){
      switch(radar.sensor_report){
        case HEARTRATEVAL:
          Serial.println(radar.heart_rate, DEC);
          heartRateChar.writeValue(radar.heart_rate);
          break;
        case BREATHVAL: // Added case for breath rate
          Serial.println(radar.breath_rate, DEC);
          breathRateChar.writeValue(radar.breath_rate);
          break;
      }
    }
    delay(200);                       //Add time delay to avoid program jam
  }
}