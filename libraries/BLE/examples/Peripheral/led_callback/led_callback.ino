/* Copyright (c) Sandeep Mistry. All rights reserved.
   Licensed under the MIT license. See LICENSE file in the project root for full license information.
   Modified by Chiara Ruggeri <chiara@arduino.org>

   This example shows how to read/write a characteristic to turn a LED on or off.
   In this implementation, after the configuration is set, the board is put in low power mode
   and the BLE features continue to work through callbacks.
   Furthermore NFC capabilities was added to show how to set different NFC messages depending
   the characteristic value.
   You can use nRFConnect app to read/write the characteristic
   https://www.nordicsemi.com/eng/Products/Nordic-mobile-Apps/nRF-Connect-for-mobile-previously-called-nRF-Master-Control-Panel
 */

#include <BLEPeripheral.h>
#include <NFC.h>
// download ArduinoLowPower library from library manager to enter in low power mode
#include "ArduinoLowPower.h"

// LED pin
#define LED_PIN   LED_BUILTIN

// create peripheral instance
BLEPeripheral           blePeripheral        = BLEPeripheral();

// create service
BLEService              ledService           = BLEService("19b10000e8f2537e4f6cd104768a1214");

// create switch characteristic
BLECharCharacteristic   switchCharacteristic = BLECharCharacteristic("19b10001e8f2537e4f6cd104768a1214", BLERead | BLEWrite);

void setup() {
  Serial.begin(115200);

  // set LED pin to output mode
  pinMode(LED_PIN, OUTPUT);

  // set advertised local name and service UUID
  blePeripheral.setLocalName("LED");
  blePeripheral.setAdvertisedServiceUuid(ledService.uuid());

  // add service and characteristic
  blePeripheral.addAttribute(ledService);
  blePeripheral.addAttribute(switchCharacteristic);

  // assign event handlers for connected, disconnected to peripheral
  blePeripheral.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  blePeripheral.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);

  // assign event handlers for characteristic
  switchCharacteristic.setEventHandler(BLEWritten, switchCharacteristicWritten);

  // begin initialization
  blePeripheral.begin();
  
  // set the NFC message
  NFC.setTXTmessage("No client connected", "en");
  NFC.start();
  
  Serial.println(F("BLE LED Peripheral"));
}

void loop() {
  // since we want to realize a low power application we don't handle the
  // BLE_LED in order to save power but put the board in low power mode instead
  LowPower.sleep();
}

void blePeripheralConnectHandler(BLECentral& central) {
  // central connected event handler
  Serial.print(F("Connected event, central: "));
  Serial.println(central.address());
  // NFC needs to be stopped before to set another message
  NFC.stop();
  NFC.setTXTmessage("Client connected!", "en");
  NFC.start();
}

void blePeripheralDisconnectHandler(BLECentral& central) {
  // central disconnected event handler
  Serial.print(F("Disconnected event, central: "));
  Serial.println(central.address());
  // NFC needs to be stopped before to set another message
  NFC.stop();
  NFC.setTXTmessage("No client connected", "en");
  NFC.start();
}

void switchCharacteristicWritten(BLECentral& central, BLECharacteristic& characteristic) {
  // central wrote new value to characteristic, update LED
  Serial.print(F("Characteristic event, writen: "));

  if (switchCharacteristic.value()) {
    Serial.println(F("LED on"));
    digitalWrite(LED_PIN, HIGH);
    // NFC needs to be stopped before to set another message
    NFC.stop();
    NFC.setTXTmessage("LED is on", "en");
    NFC.start();
  } else {
    Serial.println(F("LED off"));
    digitalWrite(LED_PIN, LOW);
    // NFC needs to be stopped before to set another message
    NFC.stop();
    NFC.setTXTmessage("LED is off", "en");
    NFC.start();
  }
}