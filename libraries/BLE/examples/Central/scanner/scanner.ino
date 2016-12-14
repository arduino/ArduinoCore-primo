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
BLECentralRole bleCentral;

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
  // put the board in standby mode to save power
  LowPower.standby();
}

void receiveAdvPck(BLENode& node){
  // print the advertising packet received
  node.printAdvertisement();
}
