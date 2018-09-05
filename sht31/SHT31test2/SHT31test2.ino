/*************************************************** 
  This is an example for the SHT31-D Humidity & Temp Sensor

  Designed specifically to work with the SHT31-D sensor from Adafruit
  ----> https://www.adafruit.com/products/2857

  These sensors use I2C to communicate, 2 pins are required to  
  interface
 ****************************************************/
 
#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_SHT31.h"

Adafruit_SHT31 sht31 = Adafruit_SHT31();
Adafruit_SHT31 sht31_2 = Adafruit_SHT31();

void setup() {
  Serial.begin(9600);

  while (!Serial)
    delay(10);     // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("SHT31 test");
  if (! sht31.begin(0x44)) {
    Serial.println("Couldn't find SHT31");
    while (1) delay(1);
  }
  if (! sht31_2.begin(0x45)) {
    Serial.println("Couldn't find second SHT31");
    while (1) delay(1);
  }
}


void loop() {
  float t = sht31.readTemperature();
  float h = sht31.readHumidity();

  if (! isnan(t)) {  // check if 'is not a number'
    Serial.print("Temp *C = "); Serial.println(t);
  } else { 
    Serial.println("Failed to read temperature");
  }
  
  if (! isnan(h)) {  // check if 'is not a number'
    Serial.print("Hum. % = "); Serial.println(h);
  } else { 
    Serial.println("Failed to read humidity");
  }
  Serial.println();

  float t2 = sht31_2.readTemperature();
  float h2 = sht31_2.readHumidity();

  if (! isnan(t2)) {  // check if 'is not a number'
    Serial.print("2nd Temp *C = "); Serial.println(t2);
  } else { 
    Serial.println("Failed to read 2nd temperature");
  }
  
  if (! isnan(h2)) {  // check if 'is not a number'
    Serial.print("2nd Hum. % = "); Serial.println(h2);
  } else { 
    Serial.println("Failed to read 2nd humidity");
  }
  Serial.println();

  delay(1000);
}
