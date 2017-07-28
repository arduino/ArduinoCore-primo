/*
  SensorsDataLogTerminal.ino

  Written by Jeff Wu (jeff@arduino.org)

  This example for the Arduino Primo Core shows how to use
  CoreSensors library.
  Gatting data from Humidity, Temperature, Accelerometer 
  and Gyroscope sensors.
  You can check data using serial montior or BLE *nRF Toolbox* 
  uart application. When a device with NFC is near to the 
  Primo Core NFC antenna, it will try to open the 
  Nordic's *nRF Toolbox* or look for the app in the store.


  This example code is in the public domain.

*/

#include <CoreSensors.h>
#include <BLEPeripheral.h>
#include <BLESerial.h>
#include <NFC.h>

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

char report[256];

void setup() {
  // initialize digital pins LED as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
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
}


void loop() {
  float humidity, temperature;
  int32_t accelerometers[3];
  int32_t gyroscope[3];
  
  humidity = coresensors.getHumidity();
  
  temperature = coresensors.getTemperature();
  
  coresensors.getAccelerometer(accelerometers);
  
  coresensors.getGyroscope(gyroscope);

  Serial.print("| Hum[%]:   ");
  Serial.print(humidity);
  Serial.print(" | Temp[C]:   ");
  Serial.print(temperature);
  Serial.print(" ");

  snprintf(report, sizeof(report), "| Acc[mg]: %6ld %6ld %6ld | Gyr[mdps]: %8ld %8ld %8ld |", accelerometers[0], accelerometers[1], accelerometers[2], gyroscope[0], gyroscope[1], gyroscope[2]);
  
  Serial.println(report);
  Serial.println();

  bleSerial.print("Humidity: ");
  bleSerial.println(humidity);
  
  bleSerial.print("Temperature: ");
  bleSerial.println(temperature);
  
  bleSerial.print("A:"); bleSerial.print(accelerometers[0]); bleSerial.print(","); bleSerial.print(accelerometers[1]); bleSerial.print(","); bleSerial.println(accelerometers[2]); 

  bleSerial.print("G:"); bleSerial.print(gyroscope[0]); bleSerial.print(","); bleSerial.print(gyroscope[1]); bleSerial.print(","); bleSerial.println(gyroscope[2]);
  
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
  digitalWrite(11, HIGH);
  delay(100);
  digitalWrite(11, LOW);
  delay(100);
  digitalWrite(12, HIGH);
  delay(100);
  digitalWrite(12, LOW);
  delay(100);
  digitalWrite(13, HIGH);
  delay(100);
  digitalWrite(13, LOW);
  delay(100);
}