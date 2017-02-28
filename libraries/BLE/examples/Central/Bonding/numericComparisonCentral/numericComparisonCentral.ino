/* numericComparisonCentral.ino

   Written by Chiara Ruggeri (chiara@arduino.org)

   This example shows how to enable bonding features on BLECentral module.
   To know all the possible bonding types please refer to the documentation.
  
   When passkey is displayed, press USER1_BUTTON on the board to confirm the
   value. Note that the value has to be confirmed on the both of the devices
   to bond them.
   Use the complementary example numericComparison.ino in File->Examples->BLE->Peripheral->Bonding
   to test this feature.
  
   This example code is in the public domain.
*/

#include <BLECentralRole.h>

#define CONFIRM_BUTTON    USER1_BUTTON
// create central instance
BLECentralRole                   bleCentral                    = BLECentralRole();

// create remote service
BLERemoteService                 dummyRemoteService                  = BLERemoteService("19b10010e8f2537e4f6cd104768a1214");

// create remote characteristics 
BLERemoteCharacteristic          dummyRemoteCharacteristic           = BLERemoteCharacteristic("19b10011e8f2537e4f6cd104768a1214", BLERead | BLEWrite);



void setup() {
  Serial.begin(9600);
  
  // initialize button
  pinMode(CONFIRM_BUTTON, INPUT);

  // add service and characteristic
  bleCentral.addRemoteAttribute(dummyRemoteService);
  bleCentral.addRemoteAttribute(dummyRemoteCharacteristic);

  // enable bonding and set the type
  bleCentral.enableBond(LESC_NUM_COMPARISON);
  
  // assign event handlers
  bleCentral.setEventHandler(BLEScanReceived, receiveAdvPck);
  bleCentral.setEventHandler(BLEConnected, bleCentralConnectHandler);
  bleCentral.setEventHandler(BLEDisconnected, bleCentralDisconnectHandler);
  bleCentral.setEventHandler(BLEPasskeyReceived, showPasskey);
  bleCentral.setEventHandler(BLEBonded, bond);

    // use BLEMessage event handler to retrieve information about internal BLE status
  bleCentral.setEventHandler(BLEMessage, receiveMessage);

  // begin initialization
  bleCentral.begin(); 
  
  Serial.println(F("BLE Bonding example"));
}

void loop() {
  // Do nothing
}

void receiveAdvPck(BLEPeripheralPeer& peer){
  char advertisedName[31];
  byte len;
  // search for a device that advertises "BONDExample" name
  peer.getFieldInAdvPck(BLE_GAP_AD_TYPE_SHORT_LOCAL_NAME, advertisedName, len);
  if(len == 0) // field not found
      peer.getFieldInAdvPck(BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME, advertisedName, len);
  if(len != 0){ // the field was found
  Serial.println(advertisedName);
    if(!strcmp(advertisedName, "BONDExample"))
      // Name found. Connect to the peripheral
      bleCentral.connect(peer);
   } 
}

void bleCentralConnectHandler(BLEPeripheralPeer& peer) {
  // peer connected event handler
  Serial.print("Connected event, peripheral: ");
  Serial.println(peer.address());
}

void bleCentralDisconnectHandler(BLEPeripheralPeer& peer) {
  // peer disconnected event handler
  Serial.print("Disconnected event, peripheral: ");
  Serial.println(peer.address());
}

void showPasskey(BLEPeripheralPeer& peer) {
  // passkey generated event handler
  Serial.print("Press the button to confirm the received passkey: ");
  Serial.println(bleCentral.getPasskey());

  bool confirm = false;
  while(confirm == false){
    if(digitalRead(CONFIRM_BUTTON) == 0) // button pressed!
      confirm = true;
    }

  bleCentral.confirmPasskey(true);
}

void bond(BLEPeripheralPeer& peer) {
  // central bonded event handler
  Serial.println("Bonded");
}

void receiveMessage(int evtCode, int messageCode){
  bleCentral.printBleMessage(evtCode, messageCode);
}