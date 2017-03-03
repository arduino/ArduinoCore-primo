#include <Wire.h>
#include "CIR.h"

#define USER1_BUTTON 0xC1AA1107
#define USER2_BUTTON 0xC1AA68F0

CIR Primo_CIR;

void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Primo_CIR.enableTransmitter();
  pinMode(34, INPUT);
  digitalWrite(13, HIGH);
}

void loop() {
  if (!digitalRead(34))
  {
    digitalWrite(13, HIGH);
    Primo_CIR.sendEncode(USER1_BUTTON);
    digitalWrite(13, LOW);
  }
  else if (!digitalRead(44))
  {
    digitalWrite(13, HIGH);
    Primo_CIR.sendEncode(USER2_BUTTON);
    digitalWrite(13, LOW);
  }
}
