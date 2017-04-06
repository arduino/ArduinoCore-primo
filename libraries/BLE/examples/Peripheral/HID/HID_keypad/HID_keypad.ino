/* Copyright (c) Sandeep Mistry. All rights reserved.
   Licensed under the MIT license. See LICENSE file in the project root for full license information.
   Modified by Chiara Ruggeri <chiara@arduino.org>
   
   This example shows the use of the HID BLE library.
   Connect the keypad to the board using the following configuration:
   rows: A0, A1, A2, A3
   columns: 7, 6, 5
   Connect the board to the phone through the phone's bluetooth settings,
   open a text field in the phone and try to press a key in the keypad.
   In this example BLE_LED shows the status of the board. It will blink every 200 ms when the board is advertising.
   It will be on when the board is connected to a central. It will be off when the board is disconnected.
*/

#include <BLEHIDPeripheral.h>
#include <BLEKeyboard.h>

// From: http://playground.arduino.cc/Code/Keypad
#include <Keypad.h>

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = { A0, A1, A2, A3 }; //connect to the row pinouts of the keypad
byte colPins[COLS] = { 7, 6, 5 }; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

BLEHIDPeripheral bleHIDPeripheral = BLEHIDPeripheral();
BLEKeyboard bleKeyboard;

void setup() {
  Serial.begin(9600);

  //initialize BLE led
  pinMode(BLE_LED, OUTPUT);
  
  Serial.println(F("BLE HID Peripheral - clearing bond data"));
    
  // clear bond store data
  bleHIDPeripheral.clearBondStoreData();  
  
  bleHIDPeripheral.setReportIdOffset(1);
  
  bleHIDPeripheral.setLocalName("HID Keypad");
  bleHIDPeripheral.addHID(bleKeyboard);

  bleHIDPeripheral.begin();

  Serial.println(F("BLE HID Keypad"));
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
      char c = keypad.getKey();
  
      if (c) {
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