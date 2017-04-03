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

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(USER1_BUTTON, INPUT);
  //attach an interrupt on button USER1 to wake up the board when a low level is detected
  attachInterrupt(USER1_BUTTON, myFunction, LOW); 

}


void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(200);
  digitalWrite(LED_BUILTIN, LOW);
  delay(200);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
  Serial.println("Ready to go to bed...");

  //enter in standby mode
  LowPower.standby();
	
  Serial.println("Good morning to all!");
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);	
}	


void myFunction(){
  // for interrupt external to LowPower library you need
  // to call wakeUpNow function in order to exit from
  // standby mode
  LowPower.wakeUpNow();
}
