/*
  SleepMode.ino
  
  Written by Jeff Wu (jeff@arduino.org)
  
  This example for the Arduino Primo board shows how to use
  stm32pwr library.
  The STM32 has two functions for power savings. One is sleep mode, 
  and the other is standby mode. The USER1 BUTTON allows you setup 
  as an interrupt to trigger STM32 enter to sleep mode or wake up 
  from sleep mode. When the Primo board is powered by USB cable and 
  battery, and then disconnect the USB cable, the STM32 will enter 
  to standby mode or the Primo board is only powered by battery. 
  Waiting for Primo board reconnect to USB cable to wake up from 
  standby mode. The disableStandbyMode() function allows you to 
  disable STM32 enter to standby mode when the Primo disconnect 
  the USB cable.

  This example code is in the public domain.
  
*/

#include <stm32pwr.h>

void setup() {
  // start serial port at 9600 bps
  Serial.begin(9600);
  // setup USER1_BUTTON as an interrupt to trigger STM32 
  // enter to sleep mode or wake up from sleep mode
  stm32pwr.enableSleepMode();
  // disable STM32 enter to standby mode
  stm32pwr.disableStandbyMode() ;
}

void loop() {
  //Waiting for USER1 BUTTON press trigger STM32 enter to sleep mode
  Serial.println("Waiting for USER1 BUTTON press");
  delay(5000);
}