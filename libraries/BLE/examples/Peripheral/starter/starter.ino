/* Copyright (c) Sandeep Mistry. All rights reserved.
   Licensed under the MIT license. See LICENSE file in the project root for full license information.
   Modified by Chiara Ruggeri <chiara@arduino.org>

   This example shows how to use the BLEPeripheral library.
   The example also uses BLE_LED to show the status of the board. It will blink every 200 ms when the board is advertising.
   It will be on when the board is connected to a central. It will be off when the board is disconnected.
*/

#include <BLEPeripheral.h>

// create peripheral instance
BLEPeripheral blePeripheral = BLEPeripheral();

// uuid's can be:
//   16-bit: "ffff"
//  128-bit: "19b10010e8f2537e4f6cd104768a1214" (dashed format also supported)

// create one or more services
BLEService service = BLEService("fff0");

// create one or more characteristics
BLECharCharacteristic characteristic = BLECharCharacteristic("fff1", BLERead | BLEWrite);

// create one or more descriptors (optional)
BLEDescriptor descriptor = BLEDescriptor("2901", "value");

void setup() {
  Serial.begin(115200);

  //initialize BLE led
  pinMode(BLE_LED, OUTPUT);

  blePeripheral.setLocalName("local-name"); // optional
  blePeripheral.setAdvertisedServiceUuid(service.uuid()); // optional

  // add attributes (services, characteristics, descriptors) to peripheral
  blePeripheral.addAttribute(service);
  blePeripheral.addAttribute(characteristic);
  blePeripheral.addAttribute(descriptor);

  // set initial value
  characteristic.setValue(0);

  // begin initialization
  blePeripheral.begin();
}

void loop() {
  BLECentral central = blePeripheral.central();

  if (central) {
    // central connected to peripheral
    Serial.print(F("Connected to central: "));
    Serial.println(central.address());

    while (central.connected()) {
      // central still connected to peripheral
      if (characteristic.written()) {
        // central wrote new value to characteristic
        Serial.println(characteristic.value(), DEC);

        // set value on characteristic
        characteristic.setValue(5);
      }
    }

    // central disconnected
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
  
  // retrieve the peripheral status in order to blink only when advertising
  if(blePeripheral.status() == ADVERTISING){
    digitalWrite(BLE_LED, LOW);
    delay(200);
    digitalWrite(BLE_LED, HIGH);
    delay(200);
  }
  else // if we are not advertising, we are connected
	digitalWrite(BLE_LED, HIGH);
}