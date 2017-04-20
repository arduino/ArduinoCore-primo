/* Copyright (c) Sandeep Mistry. All rights reserved.
   Licensed under the MIT license. See LICENSE file in the project root for full license information.
   Modified by Chiara Ruggeri <chiara@arduino.org>
   
   With this example the board implements the broadcaster role.
   Once the beacon is set, the board will advertise the URL continuously.
*/

#include <EddystoneBeacon.h>
// download ArduinoLowPower library from library manager to enter in low power mode
#include "ArduinoLowPower.h"

EddystoneBeacon eddystoneBeacon = EddystoneBeacon();

void setup() {
  Serial.begin(9600);

  delay(1000);

  eddystoneBeacon.begin(-18, "http://www.arduino.org"); // power, URI

  Serial.println(F("Eddystone URL Beacon"));
}

void loop() {
  // since we want to realize a low power application we don't handle the
  // BLE_LED in order to save power but put the board in low power mode instead
  LowPower.sleep();
}