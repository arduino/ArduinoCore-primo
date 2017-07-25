/*
  BatteryServicePeripheral.ino
  Written by  Chiara Ruggeri (chiara@arduino.org)
              Jeff Wu (jeff@arduino.org)

  This example shows how to read a characteristic to get battery capacity of Primo board
  Disabling STM32 enter to standby mode when Primo board is powered only by battery.
  You can use nRFConnect app to read the characteristic
  https://www.nordicsemi.com/eng/Products/Nordic-mobile-Apps/nRF-Connect-for-mobile-previously-called-nRF-Master-Control-Panel

  In this example allows you to get battery capacity of Primo board. It will blink every 200 ms when the board is advertising.
  It will be on when the board is connected to a central. It will be off when the board is disconnected.

  This example code is in the public domain.
*/

#include <BLEPeripheral.h>
#include <stm32pwr.h>
#include <bat.h>

// create peripheral instance
BLEPeripheralRole        blePeripheral        = BLEPeripheralRole();

// create service
BLEService               batteryService       = BLEService("180F");

// create battery level characteristic
BLECharCharacteristic    batLevelChar         = BLECharCharacteristic("2A19", BLERead | BLENotify);

bat primo_bat;

void setup() {
  // start serial port at 9600 bps
  Serial.begin(9600);

  // disable STM32 enter to standby mode
  stm32pwr.disableStandbyMode();

  // initialize BLE led
  pinMode(BLE_LED, OUTPUT);

  // set advertised local name and service UUID
  blePeripheral.setLocalName("Battery information");
  blePeripheral.setAdvertisedServiceUuid(batteryService.uuid());

  // add service and characteristic
  blePeripheral.addAttribute(batteryService);
  blePeripheral.addAttribute(batLevelChar);

  // begin initialization
  blePeripheral.begin();

  Serial.println(F("BLE Battery Service Peripheral"));
}

void loop() {
  // retrieve the peripheral status in order to blink only when advertising
  if(blePeripheral.status() == ADVERTISING){
    digitalWrite(BLE_LED, LOW);
    delay(200);
    digitalWrite(BLE_LED, HIGH);
    delay(200);
  }
  else{ // if we are not advertising, we are connected
    double bat_voltage;
    // get battery voltage value
    bat_voltage = primo_bat.analogRead();
    // print battery voltage vaule on serial port
    Serial.print("Battery Voltage:" );
    Serial.println(bat_voltage,3);
  
    int bat_capacity;
    digitalWrite(BLE_LED, HIGH);
    // get battery capacity
    bat_capacity = primo_bat.capacity();
    // retrieve battery level and update the characteristic
    batLevelChar.setValue(bat_capacity);
    Serial.println(bat_capacity);
    delay(1000);
  }
}