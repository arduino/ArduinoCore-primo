/* enterPasskeyCentral.ino

   Written by Chiara Ruggeri (chiara@arduino.org)

   This example shows how to enable bonding features on BLECentral module.
   To know all the possible bonding types please refer to the documentation.

   To delete bond information an interrupt has been attached to USER1_BUTTON (in Primo board).
   If you're using a Primo Core connect a button to the board and change USER1_BUTTON with the
   number of pin the button is connected to.
 
   Use the complementary example showPasskey.ino in File->Examples->BLE->Peripheral->Bonding
   to test this feature.
  
   In this example BLE_LED shows the status of the board. It will blink every 200 ms when the board is scanning.
   It will be on when the board is connected to a peripheral. It will be off when the board is disconnected.
  
   This example code is in the public domain.
*/

#include <BLECentralRole.h>

// create central instance
BLECentralRole                   bleCentral                    = BLECentralRole();

// create remote service
BLERemoteService                 dummyRemoteService                  = BLERemoteService("19b10010e8f2537e4f6cd104768a1214");

// create remote characteristics 
BLERemoteCharacteristic          dummyRemoteCharacteristic           = BLERemoteCharacteristic("19b10011e8f2537e4f6cd104768a1214", BLERead | BLEWrite);

int BUTTON = USER1_BUTTON;

void setup() {
  Serial.begin(9600);

  attachInterrupt(BUTTON, deleteBondInformation, LOW);

  //initialize BLE led
  pinMode(BLE_LED, OUTPUT);

  // add service and characteristic
  bleCentral.addRemoteAttribute(dummyRemoteService);
  bleCentral.addRemoteAttribute(dummyRemoteCharacteristic);

  // enable bonding and set the type
  bleCentral.enableBond(CONFIRM_PASSKEY);
  
  // assign event handlers
  bleCentral.setEventHandler(BLEScanReceived, receiveAdvPck);
  bleCentral.setEventHandler(BLEConnected, bleCentralConnectHandler);
  bleCentral.setEventHandler(BLEDisconnected, bleCentralDisconnectHandler);
  bleCentral.setEventHandler(BLEPasskeyRequested, writePasskey);
  bleCentral.setEventHandler(BLEBonded, bond);
  
  // use BLEMessage event handler to retrieve information about internal BLE status
  bleCentral.setEventHandler(BLEMessage, receiveMessage);
  
  // begin initialization
  bleCentral.begin(); 
  
  Serial.println(F("BLE Bonding example"));
}

void loop() {
  // Handle BLE led
  blinkOnScan();
}

void blinkOnScan(){
  // retrieve the central status in order to blink only when scanning
  if(bleCentral.status() == SCANNING){
    digitalWrite(BLE_LED, LOW);
    delay(200);
    digitalWrite(BLE_LED, HIGH);
    delay(200);
  }
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
  // turn BLE_LED on
  digitalWrite(BLE_LED, HIGH);
}

void bleCentralDisconnectHandler(BLEPeripheralPeer& peer) {
  // peer disconnected event handler
  Serial.print("Disconnected event, peripheral: ");
  Serial.println(peer.address());
  // turn BLE_LED off
  digitalWrite(BLE_LED, LOW);
}

void writePasskey(BLEPeripheralPeer& peer) {
 // peripheral required passkey event handler
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
   bleCentral.sendPasskey(passkey);
}

void bond(BLEPeripheralPeer& peer) {
  // central bonded event handler
  Serial.println("Bonded");
}

void deleteBondInformation(){
  // button has been pressed. Delete bond
  bleCentral.clearBondStoreData();
  Serial.println("Bond data cleared");
}

void receiveMessage(int evtCode, int messageCode){
  bleCentral.printBleMessage(evtCode, messageCode);
}