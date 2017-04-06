/*
  led_switch_client.ino
  
  Written by Chiara Ruggeri (chiara@arduino.org)
  
  The example scans for peripheral devices and connects
  to a device that advertises the name "LED Switch".
  The complementary example, led_switch.ino, can be found
  in File->Examples->BLE->Peripheral menu.
  Once the board finds a peripheral that advertises that
  name, it connects to it and subscribe the characteristic
  connected to USER1 button on the other board. At the same
  time it updates the value of the led's characteristic
  when USER1 button onboard is pressed.
  This means that a pressure of USER1 button on one board
  will cause the led's state change on the other board and
  vice versa.

  In this example BLE_LED shows the status of the board. It will blink every 200 ms when the board is scanning.
  It will be on when the board is connected to a peripheral. It will be off when the board is disconnected.
  
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

  // begin initialization and start scanning
  bleCentral.begin(); 
  
  Serial.println("Scanning...");
}

void loop() {
  if(bleCentral.connected()){
	
	//turn on BLE led
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
    else{ // if we are not connected we are scanning hence blink BLE led
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