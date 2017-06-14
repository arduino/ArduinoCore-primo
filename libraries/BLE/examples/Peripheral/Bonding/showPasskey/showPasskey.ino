/* showPasskey.ino

   Written by Chiara Ruggeri (chiara@arduino.org)

   This example shows how to enable bonding features on BLEPeripheral module.
   To know all the possible bonding types please refer to the documentation.

   To delete bond information an interrupt has been attached to USER1_BUTTON (in Primo board).
   If you're using a Primo Core connect a button to the board and change USER1_BUTTON with the
   number of pin the button is connected to.
   
   Use the complementary example enterPasskeyCentral.ino in File->Examples->BLE->Central->Bonding
   to test this feature.  

   In this example BLE_LED shows the status of the board. It will blink every 200 ms when the board is advertising.
   It will be on when the board is connected to a central. It will be off when the board is disconnected.
   
   This example code is in the public domain.
*/

#include <BLEPeripheral.h>

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

  //initialize BLE led
  pinMode(BLE_LED, OUTPUT);

  // set advertised local name and service UUID
  blePeripheral.setLocalName("BONDExample");
  blePeripheral.setAdvertisedServiceUuid(dummyService.uuid());

  // add service and characteristic
  blePeripheral.addAttribute(dummyService);
  blePeripheral.addAttribute(dummyCharacteristic);

  //enable bonding and set the type
  blePeripheral.enableBond(DISPLAY_PASSKEY);

  // assign event handlers for connected, disconnected, passkey received and bond to peripheral
  blePeripheral.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  blePeripheral.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);
  blePeripheral.setEventHandler(BLEPasskeyReceived, showPasskey);
  blePeripheral.setEventHandler(BLEBonded, bond);

  // use BLEMessage event handler to retrieve information about internal BLE status
  blePeripheral.setEventHandler(BLEMessage, receiveMessage);

  // begin initialization
  blePeripheral.begin();

  Serial.println(F("BLE Bonding example"));
}

void loop() {
  // blink if the board is advertising
  blinkOnAdv();
}

void blinkOnAdv(){
  // retrieve the peripheral status in order to blink only when advertising
  if(blePeripheral.status() == ADVERTISING){
    digitalWrite(BLE_LED, LOW);
    delay(200);
    digitalWrite(BLE_LED, HIGH);
    delay(200);
  }
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

void deleteBondInformation(){
  // button has been pressed. Delete bond
  blePeripheral.clearBondStoreData();
  Serial.println("Bond data cleared");
}

void blePeripheralConnectHandler(BLECentral& central) {
  // central connected event handler
  Serial.print(F("Connected event, central: "));
  Serial.println(central.address());
  // turn BLE_LED on
  digitalWrite(BLE_LED, HIGH);
}

void blePeripheralDisconnectHandler(BLECentral& central) {
  // central disconnected event handler
  Serial.print(F("Disconnected event, central: "));
  Serial.println(central.address());
  // turn BLE_LED off
  digitalWrite(BLE_LED, LOW);
}

void receiveMessage(int evtCode, int messageCode){
  blePeripheral.printBleMessage(evtCode, messageCode);
}