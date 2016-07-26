/*
  standbyGPIO.ino
  
  Written by Chiara Ruggeri (chiara@arduino.org)
  
  This example for the Arduino Primo board shows how to use
  low power library to enter in standby mode and save power.
  This mode ensure the fastest response time. If you need
  a deeper power saving mode use powerOFF function instead.
  
  By passing 0 as value of LowPower.standby() function the board
  will enter in standby mode without set any timer. However in
  standby mode every kind of interrupt will wake up the board so
  you can manage any other type of interrupt if you don't
  want to use the timer or if you want to use more than an
  interrupt.
  You can choose between two different sub power mode.
  With "constant latency" mode you'll get the minimum response
  time; "low power" mode will ensure the minimum power
  consumption for standby mode.
  
  This example code is in the public domain.
  
*/

#include <LowPower.h>

void setup() {

  Serial.begin(9600);
  
  pinMode(8, INPUT);
  //attach an interrupt on pin 8 to wake up the board when an high level is detected
  attachInterrupt(8, myFunction, HIGH); 
  //enter in standby mode
  LowPower.standby(0, NULL); //Low power mode - default
//  LowPower.standby(0, NULL, CONST_LATENCY); //constant latency mode
}


void loop() {}	//loop is never executed in this example


void myFunction(){
   Serial.println("Hey there, I'm still alive!");
}
