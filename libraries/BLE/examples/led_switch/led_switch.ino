#include <BLEPeripheral.h>

// LED and button pin
#define LED_PIN     13
#define BUTTON_PIN  USER1_BUTTON

// create peripheral instance, see pinouts above
BLEPeripheral            blePeripheral        = BLEPeripheral();

// create service
BLEService               ledService           = BLEService("19b10010e8f2537e4f6cd104768a1214");

// create switch and button characteristic
BLECharCharacteristic    switchCharacteristic = BLECharCharacteristic("19b10011e8f2537e4f6cd104768a1214", BLERead | BLEWrite);
BLECharCharacteristic    buttonCharacteristic = BLECharCharacteristic("19b10012e8f2537e4f6cd104768a1214", BLERead | BLENotify);

void setup() {
  Serial.begin(115200);

  // set LED pin to output mode, button pin to input mode
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);

  // set advertised local name and service UUID
  blePeripheral.setLocalName("LED Switch");
  blePeripheral.setAdvertisedServiceUuid(ledService.uuid());

  // add service and characteristics
  blePeripheral.addAttribute(ledService);
  blePeripheral.addAttribute(switchCharacteristic);
  blePeripheral.addAttribute(buttonCharacteristic);

  // begin initialization
  blePeripheral.begin();

  Serial.println(F("BLE LED Switch Peripheral"));
}

void loop() {
  // poll peripheral
  blePeripheral.poll();

  // read the current button pin state
  char buttonValue = digitalRead(BUTTON_PIN);

  // has the value changed since the last read
  bool buttonChanged = (buttonCharacteristic.value() != buttonValue);

  if (buttonChanged) {
    // button state changed, update characteristics
    switchCharacteristic.setValue(buttonValue);
    buttonCharacteristic.setValue(buttonValue);
  }

  if (switchCharacteristic.written() || buttonChanged) {
    // update LED, either central has written to characteristic or button state has changed
    if (switchCharacteristic.value()) {
      Serial.println(F("LED on"));
      digitalWrite(LED_PIN, HIGH);
    } else {
      Serial.println(F("LED off"));
      digitalWrite(LED_PIN, LOW);
    }
  }
}
