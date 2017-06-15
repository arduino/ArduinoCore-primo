/*
  Sensors_DataLog_Terminal.ino

  Written by Jeff Wu (jeff@arduino.org)

  This example for the Arduino Primo Core shows how to use
  CoreSensors library.
  Gatting data from Humidity, Temperature, Accelerometer
  and Gyroscope sensors.

  This example code is in the public domain.

*/

#include <CoreSensors.h>

void setup() {
  // Led.
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  
  // Initialize serial for output.
  Serial.begin(115200);
  coresensors.begin();
}


void loop() {
  char report[256];
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
  
  digitalWrite(10, HIGH);
  delay(100);
  digitalWrite(10, LOW);
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