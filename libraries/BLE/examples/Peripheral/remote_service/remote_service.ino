/* Copyright (c) Sandeep Mistry. All rights reserved.
   Licensed under the MIT license. See LICENSE file in the project root for full license information.
   Modified by Chiara Ruggeri <chiara@arduino.org>
*/

#include <BLEPeripheral.h>


// create peripheral instance
BLEPeripheral                    blePeripheral                            = BLEPeripheral();

// create remote services
BLERemoteService                 remoteGenericAttributeService            = BLERemoteService("1800");

// create remote characteristics
BLERemoteCharacteristic          remoteDeviceNameCharacteristic           = BLERemoteCharacteristic("2a00", BLERead);


void setup() {
  Serial.begin(9600);

  //initialize BLE led
  pinMode(BLE_LED, OUTPUT);

  blePeripheral.setLocalName("remote-attributes");

  // set device name and appearance
  blePeripheral.setDeviceName("Remote Attributes");
  blePeripheral.setAppearance(0x0080);

  blePeripheral.addRemoteAttribute(remoteGenericAttributeService);
  blePeripheral.addRemoteAttribute(remoteDeviceNameCharacteristic);

  // assign event handlers for connected, disconnected to peripheral
  blePeripheral.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  blePeripheral.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);
  blePeripheral.setEventHandler(BLERemoteServicesDiscovered, blePeripheralRemoteServicesDiscoveredHandler);

  // assign event handlers for characteristic
  remoteDeviceNameCharacteristic.setEventHandler(BLEValueUpdated, bleRemoteDeviceNameCharacteristicValueUpdatedHandle);

  // begin initialization
  blePeripheral.begin();

  Serial.println(F("BLE Peripheral - remote attributes"));
}

void loop() {
  blePeripheral.poll();
  // blink if the board is advertising
  blinkOnAdv();
}

void blinkOnAdv(){
  // retrieve the peripheral status in order to blink only when advertising
  if(blePeripheral.status() == ADVERTISING){
    digitalWrite(BLE_LED, LOW);
    delay(200);
    digitalWrite(BLE_LED, HIGH);
    delay(200);
  }
}

void blePeripheralConnectHandler(BLECentral& central) {
  // central connected event handler
  Serial.print(F("Connected event, central: "));
  Serial.println(central.address());
  // turn BLE_LED on
  digitalWrite(BLE_LED, HIGH);
}

void blePeripheralDisconnectHandler(BLECentral& central) {
  // central disconnected event handler
  Serial.print(F("Disconnected event, central: "));
  Serial.println(central.address());
  // turn BLE_LED off
  digitalWrite(BLE_LED, LOW);
}

void blePeripheralRemoteServicesDiscoveredHandler(BLECentral& central) {
  // central remote services discovered event handler
  Serial.print(F("Remote services discovered event, central: "));
  Serial.println(central.address());

  if (remoteDeviceNameCharacteristic.canRead()) {
    remoteDeviceNameCharacteristic.read();
  }
}

void bleRemoteDeviceNameCharacteristicValueUpdatedHandle(BLECentral& central, BLERemoteCharacteristic& characteristic) {
  char remoteDeviceName[BLE_REMOTE_ATTRIBUTE_MAX_VALUE_LENGTH + 1];
  memset(remoteDeviceName, 0, sizeof(remoteDeviceName));
  memcpy(remoteDeviceName, remoteDeviceNameCharacteristic.value(), remoteDeviceNameCharacteristic.valueLength());

  Serial.print(F("Remote device name: "));
  Serial.println(remoteDeviceName);
}
