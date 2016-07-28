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

  pinMode(13, OUTPUT);
  pinMode(USER2_BUTTON, INPUT);
  //attach an interrupt on button USER2 to wake up the board when a low level is detected
  attachInterrupt(USER2_BUTTON, myFunction, LOW); 

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

  //enter in standby mode
  LowPower.standby(0, NULL); //Low power mode - default
  //  LowPower.standby(0, NULL, CONST_LATENCY); //constant latency mode
	
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
 // for interrupt external to LowPower library you need
  // to set to true the event variable in order to exit
  // form standby mode
  event=true;
}
