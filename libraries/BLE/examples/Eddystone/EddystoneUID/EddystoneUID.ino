#include <EddystoneBeacon.h>

EddystoneBeacon eddystoneBeacon = EddystoneBeacon();
BLEUuid         uid             = BLEUuid("01020304050607080910-AABBCCDDEEFF"); // <namespace id>-<instance id>

void setup() {
  Serial.begin(9600);

  eddystoneBeacon.begin(-18, uid); // power, UID

  Serial.println(F("Eddystone UID Beacon"));
}

void loop() {
  eddystoneBeacon.loop();
}
