#include <BLEPeripheral.h>
#include <iBeacon.h>


iBeacon beacon;

void setup() {
  char* uuid                   = "a196c876-de8c-4c47-ab5a-d7afd5ae7127";
  unsigned short major         = 0;
  unsigned short minor         = 0;
  unsigned short measuredPower = -55;

  beacon.begin(uuid, major, minor, measuredPower);
}

void loop() {
  beacon.loop();
}
