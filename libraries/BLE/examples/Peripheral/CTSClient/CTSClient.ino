/*
  CTSClient.ino
  
  Written by Chiara Ruggeri (chiara@arduino.org)
  
  This example shows the client capabilities of the
  BLEPeripheral library.
  It acts as a CTS client and recovers the current date and
  time from a central every time USER1 button is pressed.
  The informations are transmitted as a string. A parse to
  byte type is needed to read the correct values.
  You can use nRFConnect app to set up a CTS server or use
  another board with CTSServer example in
  File->Examples->BLE->Central menu.
  To have further informations about the CTS service please
  refer to the Bluetooth specifications:
  https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.service.current_time.xml
  
  This example code is in the public domain.
  
*/

#include <BLEPeripheral.h>
// download ArduinoLowPower library from library manager to enter in low power mode
#include "ArduinoLowPower.h"

char * months [] = {"Unknown", "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
char * days [] = {"Unknown", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};

// create peripheral instance
BLEPeripheral                    blePeripheral                            = BLEPeripheral();

// create remote service with UUID compliant to CTS service
BLERemoteService                 remoteCtsService            = BLERemoteService("1805");

// create remote characteristics with UUID and properties compliant to CTS service 
BLERemoteCharacteristic          remoteCtsCharacteristic           = BLERemoteCharacteristic("2a2b", BLERead | BLENotify);


void setup() {
  Serial.begin(9600);

  blePeripheral.setLocalName("CTS-Client");

  // set device name and appearance
  blePeripheral.setDeviceName("CTS client");
  blePeripheral.setAppearance(BLE_APPEARANCE_GENERIC_CLOCK);

  blePeripheral.addRemoteAttribute(remoteCtsService);
  blePeripheral.addRemoteAttribute(remoteCtsCharacteristic);

   // assign event handlers for connected, disconnected to peripheral
  blePeripheral.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  blePeripheral.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);
  blePeripheral.setEventHandler(BLERemoteServicesDiscovered, blePeripheralRemoteServicesDiscoveredHandler);

  // assign event handlers for characteristic
  remoteCtsCharacteristic.setEventHandler(BLEValueUpdated, bleRemoteCtsCharacteristicValueUpdatedHandle);
 
  // begin initialization
  blePeripheral.begin();

  attachInterrupt(USER1_BUTTON, readTime, LOW);
  Serial.println(F("BLE Peripheral - cts"));
}

void loop() {
  // since we want to realize a low power application we don't handle the
  // BLE_LED in order to save power but put the board in low power mode instead
  LowPower.sleep();
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

void blePeripheralRemoteServicesDiscoveredHandler(BLECentral& central) {
  // central remote services discovered event handler
  Serial.print(F("Remote services discovered event, central: "));
  Serial.println(central.address());

  if (remoteCtsCharacteristic.canRead()) {
    remoteCtsCharacteristic.read();
  }
}


void bleRemoteCtsCharacteristicValueUpdatedHandle(BLECentral& central, BLERemoteCharacteristic& characteristic) {
  // copy the time value in a local variable
  char currentTime[BLE_REMOTE_ATTRIBUTE_MAX_VALUE_LENGTH + 1];
  memset(currentTime, 0, sizeof(currentTime));
  memcpy(currentTime, remoteCtsCharacteristic.value(), remoteCtsCharacteristic.valueLength());
  // year is the first two bytes of the string
  byte first=currentTime[0];
  byte second=currentTime[1];
  uint16_t year=first | (second << 8);
  // month is the third byte
  byte month = currentTime[2];
  // day, hours, minutes and seconds follow
  byte day = currentTime[3];
  byte hours = currentTime[4];
  byte minutes = currentTime[5];
  byte seconds = currentTime[6];
  // day of the week
  byte dow = currentTime[7];
  // 1/256th of a second
  byte fraction = currentTime[8];
  // Print current time
  Serial.println("*** Current Time received ***");
  Serial.print(day);
  Serial.print(" ");
  Serial.print(months[month]);
  Serial.print(" ");
  Serial.print(year);
  Serial.print(", ");
  Serial.print(hours);
  Serial.print(":");
  Serial.print(minutes);
  Serial.print(":");
  Serial.println(seconds);
  Serial.print("Day of the week: ");
  Serial.println(days[dow]);
  Serial.print("Fractions: ");
  Serial.print(fraction);
  Serial.println(" / 256 s");
  Serial.println("-------------------------------------");
}

void readTime(){
  remoteCtsCharacteristic.read();
  }