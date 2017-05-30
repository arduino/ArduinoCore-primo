/* Connection
MASTER           SLAVE
MISO      ->     MOSI
MOSI      ->     MISO
SCK       ->     SCK
D10(SS)   ->     D10(SS)
GND       ->     GND
*/

#include <SPI.h>

#ifdef ARDUINO_NRF52_PRIMO_CORE
  // Arduino Primo Core doesn't have fixed SPI pins. You can choose whatever pins you prefer. 
  #define MISO 4
  #define MOSI 5
  #define SCK  6
  #define SS   7

  SPIClass SPI(MISO, MOSI, SCK);
#endif //ARDUINO_NRF52_PRIMO_CORE

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


