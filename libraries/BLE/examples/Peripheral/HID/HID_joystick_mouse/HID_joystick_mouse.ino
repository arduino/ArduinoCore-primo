/* Copyright (c) Sandeep Mistry. All rights reserved.
   Licensed under the MIT license. See LICENSE file in the project root for full license information.
   Modified by Chiara Ruggeri <chiara@arduino.org>
   
   This example shows the use of the HID BLE library.
   Connect a joystick to the pins A0 and A1 of the board.
   Connect the board to the phone through the phone's bluetooth settings and try to
   move the joystick or press the USER1_BUTTON to press a key.
   In this example BLE_LED shows the status of the board. It will blink every 200 ms when the board is advertising.
   It will be on when the board is connected to a central. It will be off when the board is disconnected.
*/
   
#include <BLEHIDPeripheral.h>
#include <BLEMouse.h>

#define JOYSTICK_BUTTON_PIN USER1_BUTTON
#define JOYSTICK_X_AXIS_PIN A0
#define JOYSTICK_Y_AXIS_PIN A1
#define JOYSTICK_RANGE 24

// create peripheral instance
BLEHIDPeripheral bleHIDPeripheral = BLEHIDPeripheral();
BLEMouse bleMouse;

int buttonState;
int joystickXCenter;
int joystickYCenter;

void setup() {
  Serial.begin(9600);
  
  //initialize BLE led
  pinMode(BLE_LED, OUTPUT);
  
  pinMode(JOYSTICK_BUTTON_PIN, INPUT_PULLUP);
  buttonState = digitalRead(JOYSTICK_BUTTON_PIN);
  
  Serial.println(F("BLE HID Peripheral - clearing bond data"));
  bleHIDPeripheral.clearBondStoreData();

  bleHIDPeripheral.setReportIdOffset(1);
  
  bleHIDPeripheral.setLocalName("HID Mouse");
  bleHIDPeripheral.addHID(bleMouse);

  bleHIDPeripheral.begin();

  Serial.println(F("BLE HID Mouse"));
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
      int tempButtonState = digitalRead(JOYSTICK_BUTTON_PIN);
      if (tempButtonState != buttonState) {
        buttonState = tempButtonState;
        
        if (buttonState == LOW) {
          Serial.println(F("Mouse press"));
          bleMouse.press();
        } else {
          Serial.println(F("Mouse release"));
          bleMouse.release();
        }
      }

      int x = readJoystickAxis(JOYSTICK_X_AXIS_PIN);
      int y = readJoystickAxis(JOYSTICK_Y_AXIS_PIN);
      
      if (x || y) {
        bleMouse.move(x, y);
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

int readJoystickAxis(int pin) {
  int rawValue = analogRead(pin);
  delay(1);
  int mappedValue = map(rawValue, 0, 1023, 0, JOYSTICK_RANGE);
  int centeredValue = mappedValue - (JOYSTICK_RANGE / 2);
  
  return (centeredValue * -1); // reverse direction
}