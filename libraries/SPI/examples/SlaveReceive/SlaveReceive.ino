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
        
uint8_t data[DIM];
  
void setup() {

  Serial.begin(9600);
  pinMode (SS, INPUT);
  while(!Serial); 
  delay(1);
  SPI.beginSlave(); 
   
}

void loop() {
  while(digitalRead(SS)==LOW){
    for (int i=0;i<DIM;i++){
      data[i] = SPI.transfer(0x00);
      Serial.print((char)data[i]);
   }
    Serial.println("");
  }    
}



