/*
  multilink.ino
  
  Written by Chiara Ruggeri (chiara@arduino.org)
  
  This example shows the capability of a central to be
  connected to multiple peripherals at the same time.
  It implements led_switch_client example, but with the
  function allowMultilink(), it's possible to set up to
  7 simultaneously connections.
  Every time BUTTON_PIN is pressed the corresponding
  value will be sent to all peripherals connected.
  Peripherals have to use led_switch.ino example in
  File->Examples->BLE->Peripheral menu.
  
  This example code is in the public domain.
  
*/

#include <BLECentralRole.h>

// LED and button pin
#define LED_PIN     LED_BUILTIN
#define BUTTON_PIN  USER1_BUTTON

// create central instance
BLECentralRole                   bleCentral                               = BLECentralRole();

// create remote service with UUID compliant
BLERemoteService                 remoteLedService                         = BLERemoteService("19b10010e8f2537e4f6cd104768a1214");

// create remote characteristics with UUID and properties compliant those in led_switch example 
BLERemoteCharCharacteristic          remoteSwitchCharacteristic           = BLERemoteCharCharacteristic("19b10011e8f2537e4f6cd104768a1214", BLERead | BLEWrite);
BLERemoteCharCharacteristic          remoteButtonCharacteristic           = BLERemoteCharCharacteristic("19b10012e8f2537e4f6cd104768a1214", BLERead | BLENotify);

bool buttonState;

void setup() {
  Serial.begin(9600);
  
  //initialize BLE led
  pinMode(BLE_LED, OUTPUT);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  
  buttonState = digitalRead(BUTTON_PIN);
  // add service and characteristics
  bleCentral.addRemoteAttribute(remoteLedService);
  bleCentral.addRemoteAttribute(remoteSwitchCharacteristic);
  bleCentral.addRemoteAttribute(remoteButtonCharacteristic);

  // assign event handlers for central events
  bleCentral.setEventHandler(BLEScanReceived, receiveAdvPck);  
  bleCentral.setEventHandler(BLEConnected, bleCentralConnectHandler);
  bleCentral.setEventHandler(BLEDisconnected, bleCentralDisconnectHandler);
  bleCentral.setEventHandler(BLERemoteServicesDiscovered, bleCentralRemoteServicesDiscoveredHandler);

  // assign event handlers for characteristic
  remoteButtonCharacteristic.setEventHandler(BLEValueUpdated, bleRemoteButtonCharacteristicValueUpdatedHandle);
 
  // set scan parameters
  // interval and window in 0.625 ms increments 
  bleCentral.setScanInterval(3200); // 2 sec
  bleCentral.setScanWindow(800);  // 0.5 sec

  // allows the central to be connected to 2 peripherals simultaneously
  // you can choose up to 7 peripherals
  bleCentral.allowMultilink(2);

  // begin initialization and start scanning
  bleCentral.begin(); 
  
  Serial.println("Scanning...");
}

void loop() {
  if(bleCentral.connected()){

	// We are connected to at least one peripheral. turn BLE led on.
	// Please note that until the maximum number of allowed peripheral is reached
	// the board will be scan at the same time.
	digitalWrite(BLE_LED, HIGH);

    bool buttonValue = digitalRead(BUTTON_PIN);
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
  else{ // we are not connected to any peripheral. Signal the scan again
    digitalWrite(BLE_LED, LOW);
    delay(200);
    digitalWrite(BLE_LED, HIGH);
    delay(200);
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

void bleCentralDisconnectHandler(BLEPeripheralPeer& peer) {
  // peer disconnected event handler
  Serial.print("Disconnected event, peripheral: ");
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