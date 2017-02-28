/*
  multirole.ino
  
  Written by Chiara Ruggeri (chiara@arduino.org)
  
  This example implements central and peripheral roles at the
  same time. It exposes a button and a led characteristic and
  advertises "Led Switch" service and characteristics.
  At the same time it scans for peripherals that advertise
  "LED Switch" name and connect to them when they are available.
  Use led_switch example in File->Example->BLE->Peripheral on
  another board to test the central capabilities.
  Use led_switch_client example in File->Example->BLE->Central
  on another board or use nRFConnect app to test the peripheral
  capabilities.
  https://www.nordicsemi.com/eng/Products/Nordic-mobile-Apps/nRF-Connect-for-mobile-previously-called-nRF-Master-Control-Panel
 
  This example code is in the public domain.
 
*/

#include <BLECentralRole.h>
#include <BLEPeripheral.h>

// LED and button pin
#define LED_PIN     LED_BUILTIN
#define BUTTON_PIN  USER1_BUTTON


// create central and peripheral instances
BLECentralRole                 bleCentral                  = BLECentralRole();
BLEPeripheralRole              blePeripheral               = BLEPeripheralRole();

// create remote service and remote characteristics
BLERemoteService               remoteLedService            = BLERemoteService("19b10010e8f2537e4f6cd104768a1214");

BLERemoteCharCharacteristic    remoteSwitchCharacteristic  = BLERemoteCharCharacteristic("19b10011e8f2537e4f6cd104768a1214", BLERead | BLEWrite);
BLERemoteCharCharacteristic    remoteButtonCharacteristic  = BLERemoteCharCharacteristic("19b10012e8f2537e4f6cd104768a1214", BLERead | BLENotify);

// create local service and local characteristics
BLEService                     ledService                  = BLEService("19b10010e8f2537e4f6cd104768a1214");

BLECharCharacteristic          switchCharacteristic        = BLECharCharacteristic("19b10011e8f2537e4f6cd104768a1214", BLERead | BLEWrite);
BLECharCharacteristic          buttonCharacteristic        = BLECharCharacteristic("19b10012e8f2537e4f6cd104768a1214", BLERead | BLENotify);


bool buttonState;

void setup() {
  Serial.begin(9600);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);

  buttonState = digitalRead(BUTTON_PIN);

  // set advertised local name and service UUID
  blePeripheral.setLocalName("LED Switch");
  blePeripheral.setAdvertisedServiceUuid(ledService.uuid());
    
  // add peripheral service and characteristics
  blePeripheral.addAttribute(ledService);
  blePeripheral.addAttribute(switchCharacteristic);
  blePeripheral.addAttribute(buttonCharacteristic);

  // add central service and characteristics
  bleCentral.addRemoteAttribute(remoteLedService);
  bleCentral.addRemoteAttribute(remoteSwitchCharacteristic);
  bleCentral.addRemoteAttribute(remoteButtonCharacteristic);

  // assign event handler for central and peripheral
  bleCentral.setEventHandler(BLEScanReceived, receiveAdvPck);  
  bleCentral.setEventHandler(BLEConnected, bleCentralConnectHandler);
  bleCentral.setEventHandler(BLEDisconnected, bleCentralDisconnectHandler);
  bleCentral.setEventHandler(BLERemoteServicesDiscovered, bleCentralRemoteServicesDiscoveredHandler);
  
  blePeripheral.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  blePeripheral.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);
  

  // assign event handlers for remote characteristic
  remoteButtonCharacteristic.setEventHandler(BLEValueUpdated, bleRemoteButtonCharacteristicValueUpdatedHandle);
 
  // begin both of the initializations and start advertising and scanning
  blePeripheral.begin();
  bleCentral.begin();
}

void loop() {
   if(bleCentral.connected()){

    char buttonValue = digitalRead(BUTTON_PIN);
    bool buttonChanged = (buttonState != buttonValue);
  
    if(buttonChanged){ // send the message if the state has changed
      buttonState = buttonValue;
      unsigned char message;
      if(buttonState == LOW) // button pressed. Send 1 to turn on the led
        message = 1;
      else
        message = 0;
      bleCentral.writeRemoteCharacteristic(remoteSwitchCharacteristic, &message, sizeof(message));
    }
  }

  if(blePeripheral.connected()){
    // read the current button pin state
  char buttonValue = digitalRead(BUTTON_PIN);

  // has the value changed since the last read
  bool buttonChanged = (buttonCharacteristic.value() != buttonValue);

  if (buttonChanged) {
    // button state changed, update characteristics
    switchCharacteristic.setValue(buttonValue);
    buttonCharacteristic.setValue(buttonValue);
  }

  if (switchCharacteristic.written()) {
    // update LED if central has written to characteristic
    if (switchCharacteristic.value()) {
      Serial.println(F("LED on"));
      digitalWrite(LED_PIN, HIGH);
    } else {
      Serial.println(F("LED off"));
      digitalWrite(LED_PIN, LOW);
    }
   }
  }
}


void receiveAdvPck(BLEPeripheralPeer& peer){
  char advertisedName[31];
  byte len;
  // search for a device that advertises "LED Switch" name
  peer.getFieldInAdvPck(BLE_GAP_AD_TYPE_SHORT_LOCAL_NAME, advertisedName, len);
  if(len == 0) // field not found
      peer.getFieldInAdvPck(BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME, advertisedName, len);
  if(len != 0){ // the field was found
  Serial.println(advertisedName);
    if(!strcmp(advertisedName, "LED Switch"))
      // Name found. Connect to the peripheral
      bleCentral.connect(peer);
   } 
}

void bleCentralConnectHandler(BLEPeripheralPeer& peer) {
  // peer connected event handler
  Serial.print("Connected event, peripheral: ");
  Serial.println(peer.address());
}

void blePeripheralConnectHandler(BLECentralPeer& peer){
  Serial.print("Connected event, central: ");
  Serial.println(peer.address());
}  

void bleCentralDisconnectHandler(BLEPeripheralPeer& peer) {
  // peer disconnected event handler
  Serial.print("Disconnected event, peripheral: ");
  Serial.println(peer.address());
}


void blePeripheralDisconnectHandler(BLECentralPeer& peer) {
  // peer disconnected event handler
  Serial.print("Disconnected event, central: ");
  Serial.println(peer.address());
}

void bleCentralRemoteServicesDiscoveredHandler(BLEPeripheralPeer& peer) {
  // peer remote services discovered event handler
  Serial.print("Remote services discovered event, peripheral: ");
  Serial.println(peer.address());

  // subscribe button characteristic in order to automatically receive
  // messages sent from peripheral
  if (remoteButtonCharacteristic.canSubscribe()) {
    remoteButtonCharacteristic.subscribe();
  }
}


void bleRemoteButtonCharacteristicValueUpdatedHandle(BLEPeripheralPeer& peer, BLERemoteCharacteristic& characteristic) {
  if(remoteButtonCharacteristic.value() == 0){ // button pressed
    Serial.println("LED on");
    digitalWrite(LED_PIN, HIGH);
  }
  else{
    Serial.println("LED off");
    digitalWrite(LED_PIN, LOW);
  }
}