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
#include "LowPower.h"

// create central instance
BLECentralRole central;

void setup() {
  Serial.begin(9600);
  
  // assign event handler for scanReceived event
  central.setEventHandler(BLEScanReceived, receiveAdvPck);
  
  // set scan parameters
  // interval and window in 0.625 ms increments 
  central.setScanInterval(1600); // 1 sec
  central.setScanWindow(800);  // 0.5 sec
  // timeout in seconds. 0 disables timeout
  central.setScanTimeout(0);
  // active scan true to ask for scan response packet
  central.setActiveScan(true);
  // start scanning
  central.startScan();
}

void loop() {
  // put the board in standby mode to save power
  LowPower.standby();
}

void receiveAdvPck(BLENode& node){
  // print the advertising packet received
  node.printAdvertisement();
}
