#include <Wire.h>
#include "CIR.h"

CIR Primo_CIR;

void setup() {
  Serial.begin(115200);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Primo_CIR.enableReciver();
  digitalWrite(13, HIGH);
}

void loop() {
  if (Primo_CIR.getReciverStatus())
  PrintCirReceiverResult();
}

void PrintCirReceiverResult() {
  digitalWrite(13, HIGH);
  Serial.print("Data Code:" );
  Serial.println(Primo_CIR.getDecode(),HEX);
  Serial.print('\n');
  digitalWrite(13, LOW);
}