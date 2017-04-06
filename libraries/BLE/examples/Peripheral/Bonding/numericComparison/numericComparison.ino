/* numericComparison.ino

   Written by Chiara Ruggeri (chiara@arduino.org)

   This example shows how to enable bonding features on BLEPeripheral module.
   To know all the possible bonding types please refer to the documentation.
  
   When passkey is displayed, press USER1_BUTTON on the board to confirm the
   value. Note that the value has to be confirmed on the both of the devices
   to bond them.
   Use the complementary example numericComparisonCentral.ino in File->Examples->BLE->Central->Bonding
   to test this feature.
   In this example BLE_LED shows the status of the board. It will blink every 200 ms when the board is advertising.
   It will be on when the board is connected to a central. It will be off when the board is disconnected.
  
   This example code is in the public domain.
*/

#include <BLEPeripheral.h>

#define CONFIRM_BUTTON    USER1_BUTTON

// create peripheral instance
BLEPeripheral           blePeripheral          = BLEPeripheral();

// create service
BLEService              dummyService           = BLEService("19b10000e8f2537e4f6cd104768a1214");

// create characteristic
BLECharCharacteristic   dummyCharacteristic    = BLECharCharacteristic("19b10001e8f2537e4f6cd104768a1214", BLERead | BLEWrite);

void setup() {
  Serial.begin(9600);
  
  //initialize BLE led
  pinMode(BLE_LED, OUTPUT);

  // initialize button
  pinMode(CONFIRM_BUTTON, INPUT);

  // set advertised local name
  blePeripheral.setLocalName("BONDExample");

  // add service and characteristic
  blePeripheral.addAttribute(dummyService);
  blePeripheral.addAttribute(dummyCharacteristic);

  // enable bonding and set the type
  blePeripheral.enableBond(LESC_NUM_COMPARISON);

  // assign event handlers for connected, disconnected to peripheral
  blePeripheral.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  blePeripheral.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);
  blePeripheral.setEventHandler(BLEPasskeyReceived, showPasskey);
  blePeripheral.setEventHandler(BLEBonded, bond);

  // use BLEMessage event handler to retrieve information about internal BLE status
  blePeripheral.setEventHandler(BLEMessage, receiveMessage);

  // begin initialization
  blePeripheral.begin();
  
  Serial.println(F("BLE LED Peripheral"));
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
  Serial.print("Press the button to confirm the received passkey: ");
  Serial.println(blePeripheral.getPasskey());
  
  bool confirm = false;
  while(confirm == false){
    if(digitalRead(CONFIRM_BUTTON) == 0) // button pressed!
      confirm = true;
    }
    
  blePeripheral.confirmPasskey(confirm);
}

void bond(BLECentral& central) {
  Serial.println("Bonded");
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