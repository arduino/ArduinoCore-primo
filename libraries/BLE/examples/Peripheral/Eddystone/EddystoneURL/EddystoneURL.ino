/* Copyright (c) Sandeep Mistry. All rights reserved.
   Licensed under the MIT license. See LICENSE file in the project root for full license information.
   Modified by Chiara Ruggeri <chiara@arduino.org>
   
   With this example the board implements the broadcaster role.
   Once the beacon is set, the board will advertise the URL continuously.
*/

#include <EddystoneBeacon.h>
#include <LowPower.h>

EddystoneBeacon eddystoneBeacon = EddystoneBeacon();

void setup() {
  Serial.begin(9600);

  delay(1000);

  eddystoneBeacon.begin(-18, "http://www.arduino.org"); // power, URI

  Serial.println(F("Eddystone URL Beacon"));
}

void loop() {
  // Put the board in low power mode
  LowPower.standby();
}
