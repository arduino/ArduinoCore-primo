/*
  CompareVoltages.ino
  
  Written by Chiara Ruggeri (chiara@arduino.org)
  
  This example for the Arduino Primo board shows how to use
  Compare library.
  It declare a Comparator object specifying the analog pin to
  monitor, the reference voltage and the detect mode (if the
  voltage go DOWN, UP or CROSS the reference). Then begin the
  module and start the comparation with compare() function.
  When the target event occurs the function passed as parameter
  of compare function will be called.
  For a list of all voltage references look at the documentation.
  
  This example code is in the public domain.
  
*/

#include <Comparator.h>

//initialize a comparator object by monitoring analog pin A0
//and compare it with pin AREF. We look for the case in which
//A0 goes down the voltage on AREF pin
Comparator comp(A0, AREF, DOWN); 


void setup() {
 Serial.begin(9600);
 comp.begin();
 comp.compare(myFunction);
}


void loop() {
}

void myFunction(){
  Serial.println("WARNING!\r\nInput voltage is under reference voltage!");
} 
