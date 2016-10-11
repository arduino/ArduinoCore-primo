#include <EddystoneBeacon.h>

EddystoneBeacon eddystoneBeacon = EddystoneBeacon();

void setup() {
  Serial.begin(9600);

  delay(1000);

  eddystoneBeacon.begin(-18, "http://www.example.com"); // power, URI

  Serial.println(F("Eddystone URL Beacon"));
}

void loop() {
  eddystoneBeacon.loop();
}
