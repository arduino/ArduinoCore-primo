/*
  standbyTimer.ino
  
  Written by Chiara Ruggeri (chiara@arduino.org)
  
  This example for the Arduino Primo board shows how to use
  low power library to enter in standby mode and save power.
  This mode ensure the fastest response time. If you need
  a deeper power saving mode use powerOFF function instead.
  
  The function LowPower.standby() use an RTC to wake up the
  board every second and call the function passed as second
  parameter every time an event on timer occurs. You can choose
  between two different sub power mode. With "constant latency"
  mode you'll get the minimum response time; "low power" mode
  will ensure the minimum power consumption for standby mode.
  
  
  This example code is in the public domain.
  
*/

#include <LowPower.h>

void setup() {
  Serial.begin(9600);
  //set the RTC to count every second and enter in standby mode
  LowPower.standby(1, myFunction); //Low power mode - default
//  LowPower.standby(1, myFunction, CONST_LATENCY);  //constant latency mode
}


void loop() {} //loop is never executed in this example


void myFunction(){
   Serial.println("Hey there, I'm still alive!");
}

