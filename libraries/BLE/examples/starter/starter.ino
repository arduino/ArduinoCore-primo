#include <BLEPeripheral.h>

// create peripheral instance, see pinouts above
BLEPeripheral blePeripheral = BLEPeripheral();

// uuid's can be:
//   16-bit: "ffff"
//  128-bit: "19b10010e8f2537e4f6cd104768a1214" (dashed format also supported)

// create one or more services
BLEService service = BLEService("fff0");

// create one or more characteristics
BLECharCharacteristic characteristic = BLECharCharacteristic("fff1", BLERead | BLEWrite);

// create one or more descriptors (optional)
BLEDescriptor descriptor = BLEDescriptor("2901", "value");

void setup() {
  Serial.begin(115200);

  blePeripheral.setLocalName("local-name"); // optional
  blePeripheral.setAdvertisedServiceUuid(service.uuid()); // optional

  // add attributes (services, characteristics, descriptors) to peripheral
  blePeripheral.addAttribute(service);
  blePeripheral.addAttribute(characteristic);
  blePeripheral.addAttribute(descriptor);

  // set initial value
  characteristic.setValue(0);

  // begin initialization
  blePeripheral.begin();
}

void loop() {
  BLECentral central = blePeripheral.central();

  if (central) {
    // central connected to peripheral
    Serial.print(F("Connected to central: "));
    Serial.println(central.address());

    while (central.connected()) {
      // central still connected to peripheral
      if (characteristic.written()) {
        // central wrote new value to characteristic
        Serial.println(characteristic.value(), DEC);

        // set value on characteristic
        characteristic.setValue(5);
      }
    }

    // central disconnected
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}