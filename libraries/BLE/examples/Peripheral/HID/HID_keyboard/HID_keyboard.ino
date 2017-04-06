/* Copyright (c) Sandeep Mistry. All rights reserved.
   Licensed under the MIT license. See LICENSE file in the project root for full license information.
   Modified by Chiara Ruggeri <chiara@arduino.org>
   
   This example shows the use of the HID BLE library.
   Connect the board to the phone through the phone's bluetooth settings,
   open a text field in the phone and try to write something in the board serial terminal.
   In this example BLE_LED shows the status of the board. It will blink every 200 ms when the board is advertising.
   It will be on when the board is connected to a central. It will be off when the board is disconnected.
*/

#include <BLEHIDPeripheral.h>
#include <BLEKeyboard.h>


// create peripheral instance
BLEHIDPeripheral bleHIDPeripheral = BLEHIDPeripheral();
BLEKeyboard bleKeyboard;

void setup() {
  Serial.begin(9600);

  //initialize BLE led
  pinMode(BLE_LED, OUTPUT);

  // clear bond store data
  bleHIDPeripheral.clearBondStoreData();


  bleHIDPeripheral.setReportIdOffset(1);


  bleHIDPeripheral.setLocalName("HID Keyboard");
  bleHIDPeripheral.addHID(bleKeyboard);

  bleHIDPeripheral.begin();

  Serial.println(F("BLE HID Keyboard"));
}

void loop() {
  BLECentral central = bleHIDPeripheral.central();

  if (central) {
    // central connected to peripheral
    Serial.print(F("Connected to central: "));
    Serial.println(central.address());

	// turn on BLE_LED when connected
	digitalWrite(BLE_LED, HIGH);

    while (central.connected()) {
      if (Serial.available() > 0) {
        // read in character
        char c = Serial.read();

        Serial.print(F("c = "));
        Serial.println(c);

        bleKeyboard.print(c);
      }
    }

    // central disconnected
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }

  // here we are not connected. This means we are advertising
  digitalWrite(BLE_LED, HIGH);
  delay(200);
  digitalWrite(BLE_LED, LOW);
  delay(200);

}