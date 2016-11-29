/* showPasskeyBond.ino

   Written by Chiara Ruggeri (chiara@arduino.org)

   This example shows how to enable bonding features on BLEPeripheral module.
   To know all the possible bonding types please refer to the documentation.
  
   This example code is in the public domain.
*/

#include <BLEPeripheral.h>
#include <LowPower.h>

// create peripheral instance
BLEPeripheral           blePeripheral        = BLEPeripheral();

// create service
BLEService              dummyService           = BLEService("19b10000e8f2537e4f6cd104768a1214");

// create characteristic
BLECharCharacteristic   dummyCharacteristic = BLECharCharacteristic("19b10001e8f2537e4f6cd104768a1214", BLERead | BLEWrite);

void setup() {
  Serial.begin(9600);

  // set advertised local name and service UUID
  blePeripheral.setLocalName("BONDExample");
  blePeripheral.setAdvertisedServiceUuid(dummyService.uuid());

  // add service and characteristic
  blePeripheral.addAttribute(dummyService);
  blePeripheral.addAttribute(dummyCharacteristic);

  //enable bonding and set the type
  blePeripheral.enableBond(LESC_DISPLAY_PASSKEY);

  // assign event handlers for connected, disconnected, passkey received and bond to peripheral
  blePeripheral.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  blePeripheral.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);
  blePeripheral.setEventHandler(BLEPasskeyReceived, showPasskey);
  blePeripheral.setEventHandler(BLEBonded, bond);

  // begin initialization
  blePeripheral.begin();

  Serial.println(F("BLE Bonding example"));
}

void loop() {
  // put the board in low power mode
  LowPower.standby();
}

void showPasskey(BLECentral& central) {
  // passkey generated event handler
  Serial.print("Please type this passkey on the other device = ");
  Serial.println(blePeripheral.getPasskey());
}

void bond(BLECentral& central) {
  // central bonded event handler
  Serial.println("Bonded");
}

void blePeripheralConnectHandler(BLECentral& central) {
  // central connected event handler
  Serial.print(F("Connected event, central: "));
  Serial.println(central.address());
}

void blePeripheralDisconnectHandler(BLECentral& central) {
  // central disconnected event handler
  Serial.print(F("Disconnected event, central: "));
  Serial.println(central.address());
}
