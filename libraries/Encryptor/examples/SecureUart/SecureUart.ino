/*
  SecureUart.ino
  
  Written by Chiara Ruggeri (chiara@arduino.org)
  
  This example for the Arduino Primo board shows how to use
  Encryptor library.
  It opens a software UART on pins 9 and 10, encrypts all the
  characters available on the Serial port and sends them
  through the software UART. Each caracter available on
  software UART is decrypted and printed on the Serial port.
 
  This example code is in the public domain.
 
*/ 

#include <Encryptor.h>
#include <SoftwareSerial.h>

#define BUFFER_LENGTH 10

SoftwareSerial mySerial(9, 10); // RX, TX

//The key is maximum 16 bytes long
char key[16]={'a', ' ', 'v', 'e', 'r', 'y ', ' ', 'd', 'u', 'm', 'm', 'y', ' ', 'k', 'e', 'y'};
//Create buffers long enough to hold all the data
char ciphertext[BUFFER_LENGTH];
char cleartext[BUFFER_LENGTH];

void setup() {
  // Open Serial and SoftwareSerial ports
  Serial.begin(9600);
  mySerial.begin(9600);
}

void loop() {
  if (mySerial.available()){
    //if a data on SoftwareSerial is available, decrypt it before printing
    ciphertext[0] = mySerial.read();
    Encryptor.decrypt(key, ciphertext, cleartext, BUFFER_LENGTH);
    Serial.write(cleartext);
    }

  if (Serial.available()){
    //if a data on Serial is available, encrypt it before sending
    cleartext[0] = Serial.read();
    Encryptor.encrypt(key, cleartext, ciphertext, BUFFER_LENGTH);
    mySerial.write(ciphertext);
    }
}
