/*
  standbyGPIO.ino
  
  Written by Chiara Ruggeri (chiara@arduino.org)
  
  This example for the Arduino Primo board shows how to use
  low power library to enter in standby mode and save power.
  This mode ensure the fastest response time. If you need
  a deeper power saving mode use powerOFF function instead.
  
  By calling LowPower.standby() function, the board will enter
  in standby mode waiting for any interrupt. Every kind of
  interrupt will wake up the board and you can use more than
  an interrupt source at the same time.
  If you want to exit from standby mode you have to call 
  LowPower.wakeUpNow() function inside the interrupt callback.
  
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
  LowPower.standby();
	
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
  // to call wakeUpNow function in order to exit from
  // standby mode
  LowPower.wakeUpNow();
}
