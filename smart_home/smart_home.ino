/*
 * Author: Yannic
 */
#include <DS3231.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <dht11.h>
#include "Adafruit_SHT31.h"

/**
 * PINOUT
 */
const int ledPin = 3;
const int switchPin = 2;
const int echoPinRanger = 5;
const int trigPinRanger = 4;
const int DHT11PIN = 8;
const int current0Pin = A0;
const int current1Pin = A1;
const int dustSensorOutputPin = A2;
const int dustSensorLedPin = 12;

/**
 * Constantes
 */
#define        COV_RATIO                       0.2            //ug/mmm / mv
#define        NO_DUST_VOLTAGE                 400            //mv
#define        SYS_VOLTAGE                     5000           

bool century=false;
const int switchMax = 32000; // * 100ms = environ 1h (max int 32767)
bool h12=false;
bool PM=false;

/**
 * Variables
 */
bool forceOnState,timerState = false;
int switchCntr = switchMax + 1;
int lastSeconds, lastMinutes, lastHours, lastDate;
int seconds,minutes,hours,date,year,month,dow,temperature;
long inUseSeconds[2];
int maxCurrent[2];
int minCurrent[2];
float dustDensity;
float outputAirSht31Temperature = 0;
float outputAirSht31Humidity = 0;

/**
 * Objets
 */
DS3231 rtc;
dht11 DHT11;
LiquidCrystal_I2C lcd(0x27,20,4);  // Set the LCD I2C address
Adafruit_SHT31 outputAirSht31 = Adafruit_SHT31();

void setup()  
{
  Serial.begin(9600);
  Wire.begin();
  lcd.begin(20,4);
  lcd.backlight();
  pinMode(trigPinRanger, OUTPUT);
  pinMode(echoPinRanger, INPUT);
  inUseSeconds[0] = 0;
  inUseSeconds[1] = 0;
  outputAirSht31.begin(0x44);
}

void loop() 
{
  switchLoopHandler();
  timerLoopHandler();
  
  if (seconds != lastSeconds) {
    onEventSecondChanged(seconds);
    lastSeconds = seconds;
  }
  if (minutes != lastMinutes) {
    onEventMinuteChanged(minutes);
    lastMinutes = minutes;
  }
  if (hours != lastHours) {
    onEventHourChanged(hours);
    lastHours = hours;
  }
  if (date != lastDate) {
    onEventDateChanged(date);
    lastDate = date;
  }

  delay(100);
}

void (* displayFunc[6])() = { &temperatureLoopHandler, &currentDisplay0, &currentDisplay1, &rangerLoopHandler, &dustDensityDisplay, &outputAirDisplay};

void onEventSecondChanged(int sec) {
  lcd.clear();

  printDateTimetoLcd();
  printTimerToLcd();
  currentLoopHandler(current0Pin, 0);
  currentLoopHandler(current1Pin, 1);
  outputAirSht31Measure();

  displayFunc[sec % 6]();
}

void onEventMinuteChanged(int minute) {
  dustDensityLoopHandler();
}

void onEventHourChanged(int hours) {}

void onEventDateChanged(int date) {
  inUseSeconds[0] = 0;
  inUseSeconds[1] = 0;
}

void currentDisplay0() {
  currentDisplay(0);
}

void currentDisplay1() {
  currentDisplay(1);
}

void printDateTimetoLcd() {
  lcd.setCursor(0,0);
  printZeroPrefix(hours);
  lcd.print(hours);
  lcd.print(":");
  printZeroPrefix(minutes);
  lcd.print(minutes);
  lcd.print(":");
  printZeroPrefix(seconds);
  lcd.print(seconds);
  lcd.print(" 20");
  lcd.print(year);
  lcd.print("-");
  printZeroPrefix(month);
  lcd.print(month);
  lcd.print("-");
  printZeroPrefix(date);
  lcd.print(date);
}

void printTimerToLcd() {
  lcd.setCursor(0,1);
  lcd.print("Timer:");
  if (forceOnState) {
     lcd.print("MON ");
  } else if (timerState) {
     lcd.print("AON ");
  } else {
     lcd.print("OFF ");
  }
  lcd.print("Ti:");
  lcd.print(temperature);
}



void printZeroPrefix(int number) {
   if (number<10) {
    lcd.print("0");
  }
}

void switchLoopHandler() {
  int val = digitalRead(switchPin);
  if (val == LOW) {
    forceOnState = false;
    switchCntr = 0;
  } else if (!forceOnState && switchCntr == 0) {
    forceOnState = true;
  }
  if (switchCntr > switchMax) {
    forceOnState = false;
  }
  if (forceOnState) {
    switchCntr++;
  }
}

void timerLoopHandler()
{
  seconds=rtc.getSecond();
  minutes=rtc.getMinute();
  hours=rtc.getHour(h12,PM);
  date=rtc.getDate();
  month=rtc.getMonth(century);
  year=rtc.getYear();
  dow=rtc.getDoW();
  temperature=rtc.getTemperature();
    
  if ((hours < 23 && hours > 16) || forceOnState){
    timerState = true;
    digitalWrite(ledPin, HIGH);
  } else {
    timerState = false;
    digitalWrite(ledPin,LOW);
  } 
}

void rangerLoopHandler()
{
  digitalWrite(trigPinRanger, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPinRanger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinRanger, LOW);
  float cm = pulseIn(echoPinRanger, HIGH) / 58.0; //The echo time is converted into cm

  lcd.setCursor(0,2);
  lcd.print("WH:");
  lcd.print((int) cm);
}

void temperatureLoopHandler()
{
  DHT11.read(DHT11PIN);
  lcd.setCursor(0,2);  
  lcd.print("T:");
  lcd.print(DHT11.temperature);
  lcd.print("-");
  lcd.print("H%");
  lcd.print(DHT11.humidity);
}

void outputAirSht31Measure() {
  outputAirSht31Temperature = outputAirSht31.readTemperature();
  outputAirSht31Humidity = outputAirSht31.readHumidity();
}

void outputAirDisplay() {
  lcd.setCursor(0,2);
  lcd.print("Output Air");
  lcd.setCursor(0,3);
  lcd.print(outputAirSht31Temperature);
  lcd.print("oC - R%");
  lcd.print(outputAirSht31Humidity);
  
  Serial.print("Temp *C = "); Serial.println(outputAirSht31Temperature);
  Serial.print("Hum. % = "); Serial.println(outputAirSht31Humidity);
}

void currentLoopHandler(int pin, int pinId)
{
  
  maxCurrent[pinId] = 0;
  minCurrent[pinId] = 1024;
  for (int i=0;i<180;i++) {
    int currentValue = analogRead(pin);    
    if (currentValue > maxCurrent[pinId]) {
      maxCurrent[pinId] = currentValue;
    }
    if (currentValue < minCurrent[pinId]) {
      minCurrent[pinId] = currentValue;
    }
  }
  if (maxCurrent[pinId] > 100) {
    inUseSeconds[pinId]++;
  }

}

void currentDisplay(int pinId) {
  lcd.setCursor(0,2);
  lcd.print("A");
  lcd.print(pinId);
  lcd.print(" min:");
  lcd.print(minCurrent[pinId]);
  lcd.print(" max:");
  lcd.print(maxCurrent[pinId]);

  lcd.setCursor(0,3);
  lcd.print(" inUse (s):");
  lcd.print(inUseSeconds[pinId]);
}

void dustDensityLoopHandler() {
      
  // read sensor
  digitalWrite(dustSensorLedPin, HIGH);
  delayMicroseconds(280);
  int adcvalue = analogRead(dustSensorOutputPin);
  digitalWrite(dustSensorLedPin, LOW);
  
  // convert voltage
  float voltage = (SYS_VOLTAGE / 1024.0) * adcvalue;
  
  // voltage to density
  if(voltage >= NO_DUST_VOLTAGE) {
    voltage -= NO_DUST_VOLTAGE;
    dustDensity = voltage * COV_RATIO;
  } else {
    dustDensity = 0;
  }

}

void dustDensityDisplay() {
  lcd.setCursor(0,2);
  lcd.print("Dust: ");
  lcd.print((int) dustDensity);
  lcd.print(" ug/m3");
}


