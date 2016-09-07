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



