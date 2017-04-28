/*
  Transmitter.ino

  Written by Jeff Wu (jeff@arduino.org)

  This example for the Arduino Primo board shows how to use
  cir library.
  The IR transmitter allows you to send hex code by infrared LED
  of the Primo board, the hex code is based on the NEC protocol
  that transmiited consits of the following, using buttons to send
  hex code when a button is pressed.

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
#include "cir.h"

// define hex code:
#define HEX_1 0xC1AA1107
#define HEX_2 0xC1AA68F0

cir primo_cir;

void setup() {
  // initialize the LED pin as an output:
  pinMode(LED_BUILTIN, OUTPUT);
  // turn LED off:
  digitalWrite(LED_BUILTIN, LOW);
  // enable IR ready to transmitter:
  primo_cir.enableTransmitter();
  // initialize the pushbutton pin as an input:
  pinMode(USER1_BUTTON, INPUT);
  pinMode(USER2_BUTTON, INPUT);
  // turn LED on:
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
  // check if the USER1 BUTTON is pressed:
  if (!digitalRead(USER1_BUTTON))
  {
    // turn LED on:
    digitalWrite(LED_BUILTIN, HIGH);

    // sned hex code:
    primo_cir.digitalWrite(HEX_1);
    // turn LED off:
    digitalWrite(LED_BUILTIN, LOW);
  }
  // check if the USER2 BUTTON is pressed:
  else if (!digitalRead(USER2_BUTTON))
  {
    // turn LED on:
    digitalWrite(LED_BUILTIN, HIGH);
    // sned hex code:
    primo_cir.digitalWrite(HEX_2);
    // turn LED off:
    digitalWrite(LED_BUILTIN, LOW);
  }
}
