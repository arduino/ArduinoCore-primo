#include <Wire.h>
#include "CIR.h"

#define HEX_1 0xC1AA1107
#define HEX_2 0xC1AA68F0

CIR Primo_CIR;

void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Primo_CIR.enableTransmitter();
  pinMode(USER1_BUTTON, INPUT);
  pinMode(USER2_BUTTON, INPUT);
  digitalWrite(13, HIGH);
}

void loop() {
  if (!digitalRead(USER1_BUTTON))
  {
    digitalWrite(13, HIGH);
    Primo_CIR.sendEncode(HEX_1);
    digitalWrite(13, LOW);
  }
  else if (!digitalRead(USER2_BUTTON))
  {
    digitalWrite(13, HIGH);
    Primo_CIR.sendEncode(HEX_2);
    digitalWrite(13, LOW);
  }
}
