/* Connection
MASTER           SLAVE
MISO      ->     MOSI
MOSI      ->     MISO
SCK       ->     SCK
D10(SS)   ->     D10(SS)
GND       ->     GND
*/

#include <SPI.h>

#define DIM 11

void setup() {

  pinMode (SS, OUTPUT);
  digitalWrite(SS, HIGH);
  SPI.begin();
 
}

void loop() {

  String x ="hello world";
  digitalWrite(SS, LOW);
  for (int i=0;i<DIM;i++){
    SPI.transfer(x[i]);
    delay(1);
  } 
  digitalWrite(SS,HIGH);
  delay(1000);
}


