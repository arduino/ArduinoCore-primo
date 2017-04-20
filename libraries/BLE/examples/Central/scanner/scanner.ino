/*
  scanner.ino
  
  Written by Chiara Ruggeri (chiara@arduino.org)
  
  This example for the Arduino Primo board implements the
  Observer role of the BLE protocol.
  Once scan parameters are set, the sketch continuously
  listen for advertising packets.
  
  This example code is in the public domain.
  
*/

#include "BLECentralRole.h"
// download ArduinoLowPower library from library manager to enter in low power mode
#include "ArduinoLowPower.h"

// create central instance
BLECentralRole bleCentral = BLECentralRole();

void setup() {
  Serial.begin(9600);
  
  // assign event handler for scanReceived event
  bleCentral.setEventHandler(BLEScanReceived, receiveAdvPck);
  
  // set scan parameters
  // interval and window in 0.625 ms increments 
  bleCentral.setScanInterval(3200); // 2 sec
  bleCentral.setScanWindow(800);  // 0.5 sec
  // timeout in seconds. 0 disables timeout
  bleCentral.setScanTimeout(0);
  // active scan true to ask for scan response packet
  bleCentral.setActiveScan(true);
  // begin initialization and start scanning
  bleCentral.begin();
}

void loop() {
  // since we want to realize a low power application we don't handle the
  // BLE_LED in order to save power but put the board in low power mode instead.
  LowPower.sleep();
}

void receiveAdvPck(BLEPeripheralPeer& peer){
  // print the advertising packet received
  peer.printAdvertisement();
}