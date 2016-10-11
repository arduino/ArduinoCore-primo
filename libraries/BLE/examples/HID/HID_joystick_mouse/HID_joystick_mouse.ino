#include <BLEHIDPeripheral.h>
#include <BLEMouse.h>

#define JOYSTICK_BUTTON_PIN 3
#define JOYSTICK_X_AXIS_PIN A0
#define JOYSTICK_Y_AXIS_PIN A1
#define JOYSTICK_RANGE 24

// create peripheral instance, see pinouts above
BLEHIDPeripheral bleHIDPeripheral = BLEHIDPeripheral();
BLEMouse bleMouse;

int buttonState;
int joystickXCenter;
int joystickYCenter;

void setup() {
  Serial.begin(9600);
  
  pinMode(JOYSTICK_BUTTON_PIN, INPUT_PULLUP);
  buttonState = digitalRead(JOYSTICK_BUTTON_PIN);
  
  if (buttonState == LOW) {
    Serial.println(F("BLE HID Peripheral - clearing bond data"));
    
    // clear bond store data
    bleHIDPeripheral.clearBondStoreData();
  }
  
  bleHIDPeripheral.setLocalName("HID Mouse");
  bleHIDPeripheral.addHID(bleMouse);

  bleHIDPeripheral.begin();

  Serial.println(F("BLE HID Mouse"));
  
  joystickXCenter = readJoystickAxis(JOYSTICK_X_AXIS_PIN);
  joystickYCenter = readJoystickAxis(JOYSTICK_Y_AXIS_PIN);
}

void loop() {
  BLECentral central = bleHIDPeripheral.central();

  if (central) {
    // central connected to peripheral
    Serial.print(F("Connected to central: "));
    Serial.println(central.address());

    while (central.connected()) {
      int tempButtonState = digitalRead(JOYSTICK_BUTTON_PIN);
      if (tempButtonState != buttonState) {
        buttonState = tempButtonState;
        
        if (buttonState == LOW) {
          Serial.println(F("Mouse press"));
          bleMouse.press();
        } else {
          Serial.println(F("Mouse release"));
          bleMouse.release();
        }
      }

      int x = readJoystickAxis(JOYSTICK_X_AXIS_PIN) - joystickXCenter;
      int y = readJoystickAxis(JOYSTICK_Y_AXIS_PIN) - joystickYCenter;
      
      if (x || y) {
        bleMouse.move(x, y);
      }
    }

    // central disconnected
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}

int readJoystickAxis(int pin) {
  int rawValue = analogRead(pin);
  int mappedValue = map(rawValue, 0, 1023, 0, JOYSTICK_RANGE);
  int centeredValue = mappedValue - (JOYSTICK_RANGE / 2);
  
  return (centeredValue * -1); // reverse direction
}
