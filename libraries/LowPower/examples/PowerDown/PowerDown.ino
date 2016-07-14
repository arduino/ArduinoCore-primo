/*
  PowerDown.ino
  
  Written by Chiara Ruggeri (chiara@arduino.org)
  
  This example for the Arduino Primo board shows how to use
  low power library to enter in power off mode and save power.
  This mode ensure the deepest power saving mode. If you need
  a faster response from the board use Idle function instead.
  
  The functions WakeUpBy.. set the signals that will wake up 
  the board. Comment out this functions if you don't want to
  use one of them in order to get the minimum power consumption.
  The board will be reset when it wakes up from power off.
  You can use WhoIs() function to find out what signals woke up
  the board if you use more than one WakeUpBy.. function.
  
  This example code is in the public domain.
  
*/

#include <LowPower.h>

void setup() {
  Serial.begin(9600);

  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  delay(500);
  digitalWrite(13, LOW);
  delay(500);

  int reason=LowPower.WhoIs();
  if(reason==1) //GPIO caused the wake up
    doMyStuff();
  else
    if(reason==2) //NFC caused the wake up
      doMyStuffWithNFC();

  Serial.println("Hi all, I return to sleep");

  LowPower.WakeUpByGPIO(8, LOW);
  LowPower.WakeUpByNFC();
  LowPower.powerOFF();
}


void loop() {}

void doMyStuff(){
  //insert your code here
}

void doMyStuffWithNFC(){
  //insert your code here
}

