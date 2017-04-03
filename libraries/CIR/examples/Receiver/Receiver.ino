/*
  Receiver.ino

  Written by Jeff Wu (jeff@arduino.org)

  This example for the Arduino Primo board shows how to use
  CIR library.
  CIR data is received by 38kHz IR receiver of the Primo board,
  It is based on the NEC protocol to decode signals from IR
  receiver to get hex code, you can see the hex code using serial
  montior when the Primo board receive CIR data.

  NEC Protocol:
  9ms leading pulse burst
  4.5ms space
  8-bit address for the receiving device
  8-bit logical inverse of the address
  8-bit command
  8-bit logical inverse of the command
  Logical 0 : 560us + 560us
  Logical 1 : 560us + 1690us

  This example code is in the public domain.

*/

#include <Wire.h>
#include "CIR.h"

CIR Primo_CIR;

void setup() {
  // start serial port at 115200 bps:
  Serial.begin(115200);
  // initialize the LED pin as an output:
  pinMode(LED_BUILTIN, OUTPUT);
  // turn LED off:
  digitalWrite(LED_BUILTIN, LOW);
  // enable IR ready to receiver:
  Primo_CIR.enableReceiver();
  // turn LED on:
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
  // check if IR receiver decodes signals and accord with NEC Protocl:
  if (Primo_CIR.getReceiverStatus())
  // print hex code:
  PrintCirReceiverResult();
}

void PrintCirReceiverResult() {
  // turn LED on:
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.print("Data Code:");
  // convert to hex code and print:
  Serial.println(Primo_CIR.getDecode(),HEX);
  // new line:
  Serial.print('\n');
  // turn LED off:
  digitalWrite(LED_BUILTIN, LOW);
}
