/*
  SenseNFC.ino
  
  Written by Chiara Ruggeri (chiara@arduino.org)
  
  This example for the Arduino Primo board shows how to use
  PPI library.
  With this library is possible to bind an action to a particular
  event. In this way, when an event occurs, the related peripheral
  can take the specified action without the MCU intervention.
  For a complete list of all events and actions that is possible to
  use, please refer to the library documentation.
  
  This example code is in the public domain.
  
*/

#include <PPI.h>
// download ArduinoLowPower library from library manager to enter in low power mode
#include "ArduinoLowPower.h"

void setup() {

//Set the comparator to stop NFC sense when voltage
//on pin A0 goes below 1/8 of VDD
PPI.setInputPin(A0);
PPI.setCompReference(REF_VDD_1_8); 
//              event    , action
PPI.setShortcut(COMP_DOWN, NFC_STOP_SENSE);

//Start to sense NFC field each time an high level is detected
//on pin 4
PPI.setInputPin(4);
PPI.setShortcut(PIN_HIGH, NFC_START_SENSE);

//Toggle LED pin each time an NFC field is detected
PPI.setOutputPin(LED_BUILTIN);
PPI.setShortcut(NFC_FIELD_DETECTED, PIN_TOGGLE);
}


void loop() {
//Put mcu in sleep mode to save power. 
LowPower.sleep();
}