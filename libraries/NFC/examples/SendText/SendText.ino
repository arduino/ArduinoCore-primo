/*
  SendText.ino
  
  Written by Chiara Ruggeri (chiara@arduino.org)
  
  This example for the Arduino Primo board shows how to use
  NFC library.
  It first initialize the NFC and then set a text message
  specifying the language code, so that when a device with
  NFC is near to the board the message "Hello World!" will be sent.

  This example code is in the public domain.
  
*/

#include <NFC.h>

static const uint8_t message[]={'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!'};
static const uint8_t language[] = {'e', 'n'}; //english

void setup() { 
  NFC.begin();
  NFC.setTXTmessage(message, language);
}


void loop() {
}


