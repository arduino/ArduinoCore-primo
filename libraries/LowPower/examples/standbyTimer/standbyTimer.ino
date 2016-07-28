/*
  standbyTimer.ino
  
  Written by Chiara Ruggeri (chiara@arduino.org)
  
  This example for the Arduino Primo board shows how to use
  low power library to enter in standby mode and save power.
  This mode ensure the fastest response time. If you need
  a deeper power saving mode use powerOFF function instead.
  
  The function LowPower.standby() use an RTC to wake up the
  board after five second and call the function passed as second
  parameter when the count expires. You can choose between two
  different sub power mode. With "constant latency" mode you'll
  get the minimum response time; "low power" mode will ensure
  the minimum power consumption for standby mode.
  
  
  This example code is in the public domain.
  
*/

#include <LowPower.h>

void setup() {
  Serial.begin(9600);
  pinMode(13, OUTPUT);
}


void loop() {
  digitalWrite(13, HIGH);
  delay(200);
  digitalWrite(13, LOW);
  delay(200);
  digitalWrite(13, HIGH);
  delay(100);
  digitalWrite(13, LOW);
  delay(100);
  Serial.println("Ready to go to bed...");

  //set the RTC to wake up the board after five second and enter in standby mode
  LowPower.standby(5, myFunction); //Low power mode - default
  //  LowPower.standby(5, myFunction, CONST_LATENCY);  //constant latency mode

  Serial.println("Good morning to all!");
  digitalWrite(13, HIGH);
  delay(1000);
  digitalWrite(13, LOW);
  delay(1000);
  digitalWrite(13, HIGH);
  delay(500);
  digitalWrite(13, LOW);
  delay(500);	

} 


void myFunction(){
  // pass NULL as second argument of standby function if you
  // don't want to manage the Interrupt Request
}

