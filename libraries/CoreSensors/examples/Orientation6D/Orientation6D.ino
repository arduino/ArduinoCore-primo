/*
  DoubleTap.ino

  Written by Jeff Wu (jeff@arduino.org)

  This example for the Arduino Primo Core shows how to use
  CoreSensors library.
  The example is to show how to find out the 6D orientation
  with the Primo Core. Trying to rotate the Primo Core to 
  change the 6D orientation, when the orientation event is 
  triggered, the USER LED will blink once.
  You can check data using serial or BLE *nRF Toolbox* 
  uart application. When a device with NFC is near to the 
  Primo Core with NFC antenna, it will try to open the 
  Nordic's *nRF Toolbox* or look for the app in the store.

  This example code is in the public domain.

*/

#include <CoreSensors.h>
#include <BLEPeripheral.h>
#include <BLESerial.h>
#include <NFC.h>

// interrupt pin
#define INT1 21

// create ble serial instance
BLESerial bleSerial = BLESerial();

// specify the package name for windows and android phone and insert the EOL character at the end '\0'
static const char android_package_name[] = {'n', 'o', '.', 'n', 'o', 'r', 'd', 'i', 'c', 's',
                                               'e', 'm', 'i', '.', 'a', 'n', 'd', 'r', 'o', 'i', 
                                               'd', '.', 'n', 'r', 'f', 't', 'o', 'o', 'l', 'b',
                                               'o', 'x','\0'};

static const char windows_application_id[] = {'{', 'e', '1', '2', 'd', '2', 'd', 'a', '7', '-',
                                                 '4', '8', '8', '5', '-', '4', '0', '0', 'f', '-',
                                                 'b', 'c', 'd', '4', '-', '6', 'c', 'b', 'd', '5',
                                                 'b', '8', 'c', 'f', '6', '2', 'c', '}', '\0'};

int orientation = 0;
char report[256];

void sendOrientation(void);

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  // start serial port at 115200 bps
  Serial.begin(115200);
  // custom services and characteristics can be added as well
  bleSerial.setLocalName("Arduino Primo Core");
  // start ble serial
  bleSerial.begin();
  // set the Android packge name as first and the Windows application id as second
  NFC.setAPPmessage(android_package_name, windows_application_id);
  // start the NFC module
  NFC.start();
  // start the core sensors
  coresensors.begin();
  // enable enable 6D orientation
  coresensors.enable6DOrientation();
  // set up interrupt pin to sensor
  attachInterrupt(INT1, Orientation, RISING);

}

void loop() {
  // check interrupt flag
  if (orientation)
  {
    orientation = 0;
    uint8_t status = 0;
    // get 6D orientation status
    coresensors.getStatus6DOrientation(status);
    if (status)
    {
      // print 6D Orientation
      sendOrientation();
      
      digitalWrite(LED_BUILTIN, HIGH);
      delay(100);
      digitalWrite(LED_BUILTIN, LOW);
    }
  }
}

// interrupt service routine
void Orientation()
{
  orientation = 1;
}

void sendOrientation( void )
{
  uint8_t xl = 0;
  uint8_t xh = 0;
  uint8_t yl = 0;
  uint8_t yh = 0;
  uint8_t zl = 0;
  uint8_t zh = 0;
  
  coresensors.get6DOrientationXL(xl);
  coresensors.get6DOrientationXH(xh);
  coresensors.get6DOrientationYL(yl);
  coresensors.get6DOrientationYH(yh);
  coresensors.get6DOrientationZL(zl);
  coresensors.get6DOrientationZH(zh);
  
  if ( xl == 0 && yl == 0 && zl == 0 && xh == 1 && yh == 0 && zh == 0 )
  {
    sprintf( report, "\r\n  ________________  " \
                      "\r\n |                | " \
                      "\r\n |  *             | " \
                      "\r\n |                | " \
                      "\r\n |                | " \
                      "\r\n |                | " \
                      "\r\n |                | " \
                      "\r\n |________________| \r\n" );
    bleSerial.println("Left");
  }
  
  else if ( xl == 0 && yl == 1 && zl == 0 && xh == 0 && yh == 0 && zh == 0 )
  {
    sprintf( report, "\r\n  ________________  " \
                      "\r\n |                | " \
                      "\r\n |             *  | " \
                      "\r\n |                | " \
                      "\r\n |                | " \
                      "\r\n |                | " \
                      "\r\n |                | " \
                      "\r\n |________________| \r\n" );
    bleSerial.println("Up");
  }
  
  else if ( xl == 0 && yl == 0 && zl == 0 && xh == 0 && yh == 1 && zh == 0 )
  {
    sprintf( report, "\r\n  ________________  " \
                      "\r\n |                | " \
                      "\r\n |                | " \
                      "\r\n |                | " \
                      "\r\n |                | " \
                      "\r\n |                | " \
                      "\r\n |  *             | " \
                      "\r\n |________________| \r\n" );
    bleSerial.println("Down");
  }
  
  else if ( xl == 1 && yl == 0 && zl == 0 && xh == 0 && yh == 0 && zh == 0 )
  {
    sprintf( report, "\r\n  ________________  " \
                      "\r\n |                | " \
                      "\r\n |                | " \
                      "\r\n |                | " \
                      "\r\n |                | " \
                      "\r\n |                | " \
                      "\r\n |             *  | " \
                      "\r\n |________________| \r\n" );
    bleSerial.println("Right");
  }
  
  else if ( xl == 0 && yl == 0 && zl == 0 && xh == 0 && yh == 0 && zh == 1 )
  {
    sprintf( report, "\r\n  __*_____________  " \
                     "\r\n |________________| \r\n" );
    bleSerial.println("Top");
  }
  
  else if ( xl == 0 && yl == 0 && zl == 1 && xh == 0 && yh == 0 && zh == 0 )
  {
    sprintf( report, "\r\n  ________________  " \
                     "\r\n |________________| " \
                     "\r\n    *               \r\n" );
    bleSerial.println("Buttom");
  }
  
  else
  {
    sprintf( report, "None of the 6D orientation axes is set in LSM6DSL - accelerometer.\r\n" );
  }
  
  Serial.print(report);
}