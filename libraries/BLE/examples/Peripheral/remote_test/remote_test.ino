/* Copyright (c) Sandeep Mistry. All rights reserved.
   Licensed under the MIT license. See LICENSE file in the project root for full license information.
   Modified by Chiara Ruggeri <chiara@arduino.org>
*/


#include <BLEPeripheral.h>
#include <BLEUtil.h>

BLEPeripheral                    blePeripheral                            = BLEPeripheral();

// create remote services
BLERemoteService                 remoteService                            = BLERemoteService("fffffffffffffffffffffffffffffff0");

// create remote characteristics
BLERemoteCharacteristic          remoteCharacteristic1                    = BLERemoteCharacteristic("fffffffffffffffffffffffffffffff1", BLERead);
BLERemoteCharacteristic          remoteCharacteristic2                    = BLERemoteCharacteristic("fffffffffffffffffffffffffffffff2", BLERead);
BLERemoteCharacteristic          remoteCharacteristic3                    = BLERemoteCharacteristic("fffffffffffffffffffffffffffffff3", BLEWrite | BLEWriteWithoutResponse);
BLERemoteCharacteristic          remoteCharacteristic4                    = BLERemoteCharacteristic("fffffffffffffffffffffffffffffff4", BLENotify);


void setup() {
  Serial.begin(9600);

  //initialize BLE led
  pinMode(BLE_LED, OUTPUT);

  blePeripheral.setLocalName("remote-test");

  // set device name and appearance
  blePeripheral.setDeviceName("Remote Test");
  blePeripheral.setAppearance(0x0080);

  blePeripheral.addRemoteAttribute(remoteService);
  blePeripheral.addRemoteAttribute(remoteCharacteristic1);
  blePeripheral.addRemoteAttribute(remoteCharacteristic2);
  blePeripheral.addRemoteAttribute(remoteCharacteristic3);
  blePeripheral.addRemoteAttribute(remoteCharacteristic4);

  // assign event handlers for connected, disconnected to peripheral
  blePeripheral.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  blePeripheral.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);
  blePeripheral.setEventHandler(BLERemoteServicesDiscovered, blePeripheralRemoteServicesDiscoveredHandler);

  // assign event handlers for characteristic
  remoteCharacteristic1.setEventHandler(BLEValueUpdated, bleRemoteCharacteristicValueUpdatedHandle);
  remoteCharacteristic2.setEventHandler(BLEValueUpdated, bleRemoteCharacteristicValueUpdatedHandle);
  remoteCharacteristic4.setEventHandler(BLEValueUpdated, bleRemoteCharacteristicValueUpdatedHandle);


  // begin initialization
  blePeripheral.begin();

  Serial.println(F("BLE Peripheral - remote test"));
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

//  if (remoteCharacteristic1.canRead()) {
//    Serial.println(F("Read 1"));
//    remoteCharacteristic1.read();
//  }
//  
//  if (remoteCharacteristic2.canRead()) {
//    Serial.println(F("Read 2"));
//    remoteCharacteristic2.read();
//  }
//
//  if (remoteCharacteristic3.canWrite()) {
//    Serial.println(F("Write 3"));
//    
//    unsigned long writeValue = 42;
//    
//    remoteCharacteristic3.write((const unsigned char*)&writeValue, sizeof(writeValue));
//  }
//  
//  if (remoteCharacteristic4.canSubscribe()) {
//    Serial.println(F("Subscribe 4"));
//        
//    remoteCharacteristic4.subscribe();
//  }
}

void bleRemoteCharacteristicValueUpdatedHandle(BLECentral& central, BLERemoteCharacteristic& characteristic) {
  Serial.print(F("Remote characteristic value: "));

  BLEUtil::printBuffer(characteristic.value(), characteristic.valueLength());
}

