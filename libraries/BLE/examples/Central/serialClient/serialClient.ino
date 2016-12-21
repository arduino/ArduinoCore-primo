/*
  serialClient.ino
  
  Written by Chiara Ruggeri (chiara@arduino.org)
  
  This example implements a Serial client over BLE.
  The example scans for peripheral devices and connects
  to a device that advertises NUS (Nordic Uart Service).
  Serial example in File->Examples->BLE->Peripheral menu
  can act as server for this example.
  
  This example code is in the public domain.
  
*/

#include <BLECentralRole.h>

// create central instance
BLECentralRole                   bleCentral                       = BLECentralRole();

// create remote service with UUID compliant to NUS service
BLERemoteService                 remoteService                    = BLERemoteService("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");

// create remote characteristics with UUID and properties compliant NUS service
BLERemoteCharacteristic          remoteRxCharacteristic           = BLERemoteCharacteristic("6E400002-B5A3-F393-E0A9-E50E24DCCA9E", BLEWriteWithoutResponse);
BLERemoteCharacteristic          remoteTxCharacteristic           = BLERemoteCharacteristic("6E400003-B5A3-F393-E0A9-E50E24DCCA9E", BLERead | BLENotify);

void setup() {
  Serial.begin(9600);
  
  // add service and characteristics
  bleCentral.addRemoteAttribute(remoteService);
  bleCentral.addRemoteAttribute(remoteTxCharacteristic);
  bleCentral.addRemoteAttribute(remoteRxCharacteristic);

  // assign event handlers for central events
  bleCentral.setEventHandler(BLEScanReceived, receiveAdvPck);  
  bleCentral.setEventHandler(BLEConnected, bleCentralConnectHandler);
  bleCentral.setEventHandler(BLEDisconnected, bleCentralDisconnectHandler);
  bleCentral.setEventHandler(BLERemoteServicesDiscovered, bleCentralRemoteServicesDiscoveredHandler);

  // assign event handlers for characteristic
  remoteTxCharacteristic.setEventHandler(BLEValueUpdated, bleRemoteTxCharacteristicValueUpdatedHandle);
 
  // set scan parameters
  // interval and window in 0.625 ms increments 
  bleCentral.setScanInterval(3200); // 2 sec
  bleCentral.setScanWindow(800);  // 0.5 sec

  // begin initialization and start scanning
  bleCentral.begin(); 
  
  Serial.println("Scanning...");
}

void loop() {
  // if a device is connected send data read from the Serial port
  if(bleCentral.connected()){
  int len = 0;
  // send the message when it reaches the maximum value (20 char)
  // or when a carriage return (\n) is pressed
  unsigned char message[BLE_ATTRIBUTE_MAX_VALUE_LENGTH];
  bool sendNow = false;
     while(len < BLE_ATTRIBUTE_MAX_VALUE_LENGTH){
   int k = Serial.available();
    for(int j = 0; j < k; j++){
      message[len] = Serial.read();
      len++;
      if(message[len-1] == '\n')
        sendNow = true;
        }
           
   if(sendNow)
     break;

    }
   // send the message
   bleCentral.writeRemoteCharacteristic(remoteRxCharacteristic, message, len);
  }
}

void receiveAdvPck(BLENode& node){
  char scannedUuid[31];
  byte len;
  // search for the NUS's uuid
  node.getFieldInAdvPck(BLE_GAP_AD_TYPE_128BIT_SERVICE_UUID_MORE_AVAILABLE, scannedUuid, len);
  if(len != 0){ // the field was found
    if(!strcmp(scannedUuid, remoteService.rawUuid()))
      // the uuids match. Connect to the peripheral
      bleCentral.connect(node);
   } 
}

void bleCentralConnectHandler(BLENode& node) {
  // node connected event handler
  Serial.print("Connected event, node: ");
  Serial.println(node.address());
}

void bleCentralDisconnectHandler(BLENode& node) {
  // node disconnected event handler
  Serial.print("Disconnected event, node: ");
  Serial.println(node.address());
}

void bleCentralRemoteServicesDiscoveredHandler(BLENode& node) {
  // node remote services discovered event handler
  Serial.print("Remote services discovered event, node: ");
  Serial.println(node.address());

  // subscribe Tx characteristic in order to automatically receive
  // messages sent from peripheral
  if (remoteTxCharacteristic.canSubscribe()) {
    remoteTxCharacteristic.subscribe();
  }
}


void bleRemoteTxCharacteristicValueUpdatedHandle(BLENode& node, BLERemoteCharacteristic& characteristic) {
  Serial.println("Remote characteristic value update handle");
  Serial.println((char *)remoteTxCharacteristic.value());
 }
