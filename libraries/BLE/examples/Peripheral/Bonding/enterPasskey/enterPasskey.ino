/* enterPasskey.ino

   Written by Chiara Ruggeri (chiara@arduino.org)

   This example shows how to enable bonding features on BLEPeripheral module.
   To know all the possible bonding types please refer to the documentation.

   To delete bond information an interrupt has been attached to USER1_BUTTON (in Primo board).
   If you're using a Primo Core connect a button to the board and change USER1_BUTTON with the
   number of pin the button is connected to.
   
   Use the complementary example showPasskeyCentral.ino in File->Examples->BLE->Central->Bonding
   to test this feature.

   This example code is in the public domain.
*/

#include <BLEPeripheral.h>
// download ArduinoLowPower library from library manager to enter in low power mode
#include "ArduinoLowPower.h"


// create peripheral instance
BLEPeripheral           blePeripheral        = BLEPeripheral();

// create service
BLEService              dummyService           = BLEService("19b10000e8f2537e4f6cd104768a1214");

// create characteristic
BLECharCharacteristic   dummyCharacteristic = BLECharCharacteristic("19b10001e8f2537e4f6cd104768a1214", BLERead | BLEWrite);

int BUTTON = USER1_BUTTON;

void setup() {
  Serial.begin(9600);
  
  attachInterrupt(BUTTON, deleteBondInformation, LOW);

  // set advertised local name and service UUID
  blePeripheral.setLocalName("BONDExample");
  blePeripheral.setAdvertisedServiceUuid(dummyService.uuid());

  // add service and characteristic
  blePeripheral.addAttribute(dummyService);
  blePeripheral.addAttribute(dummyCharacteristic);

  //enable bonding and set the type
  blePeripheral.enableBond(CONFIRM_PASSKEY);

  // assign event handlers for connected, disconnected, passkey requested and bond to peripheral
  blePeripheral.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  blePeripheral.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);
  blePeripheral.setEventHandler(BLEPasskeyRequested, writePasskey);
  blePeripheral.setEventHandler(BLEBonded, bond);

  // use BLEMessage event handler to retrieve information about internal BLE status
  blePeripheral.setEventHandler(BLEMessage, receiveMessage);
  
  // begin initialization
  blePeripheral.begin();
  
  Serial.println(F("BLE Bonding example"));
}

void loop() {
  // since we want to realize a low power application we don't handle the
  // BLE_LED in order to save power but put the board in low power mode instead
  LowPower.sleep();
}

void writePasskey(BLECentral& central) {
  // central required passkey event handler
  char passkey[6];
  int i=0;
  int j=0;
  Serial.println("Type the 6 digits code you see on the other device");
  Serial.flush();
  while(i < 6){
    j=Serial.available();
      for(int k=0; k<j; k++){
        passkey[i]=Serial.read();
        i++;
        }
   }
   Serial.print("you typed: ");
   Serial.println(passkey);
   blePeripheral.sendPasskey(passkey);
}

void bond(BLECentral& central) {
  // central bonded event handler
  Serial.println("Bonded");
}

void deleteBondInformation(){
  // button has been pressed. Delete bond
  blePeripheral.clearBondStoreData();
  Serial.println("Bond data cleared");
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

void receiveMessage(int evtCode, int messageCode){
  blePeripheral.printBleMessage(evtCode, messageCode);
}