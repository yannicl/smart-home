/*
 * Author: Yannic
 */
#include <DS3231.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <dht11.h>

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

/**
 * Constantes
 */
bool century=false;
const int switchMax = 32000; // * 100ms = environ 1h (max int 32767)
bool h12=false;
bool PM=false;

/**
 * Variables
 */
bool forceOnState,timerState = false;
int switchCntr = switchMax + 1;
int lastSeconds;
int seconds,minutes,hours,date,year,month,dow,temperature;
long inUseSeconds[2];

/**
 * Objets
 */
DS3231 rtc;
dht11 DHT11;
LiquidCrystal_I2C lcd(0x27,20,4);  // Set the LCD I2C address

void setup()  
{
  Wire.begin();
  lcd.begin(20,4);
  lcd.backlight();
  pinMode(trigPinRanger, OUTPUT);
  pinMode(echoPinRanger, INPUT);
  inUseSeconds[0] = 0;
  inUseSeconds[1] = 0;
}

void loop() 
{
  switchLoopHandler();
  timerLoopHandler();
  
  seconds=rtc.getSecond();
  if (seconds != lastSeconds) {
    onEventSecondChanged(seconds);
    lastSeconds = seconds;
  }

  delay(100);
}

void onEventSecondChanged(int sec) {
  printDateTimetoLcd();
  printTimerToLcd();

  if ((sec % 4) == 0) {
    temperatureLoopHandler();
  } else if ((sec % 4) == 2) {
    currentLoopHandler(current0Pin, 0);
    currentInUseDisplay(0);
  } else if ((sec % 4) == 3) {
    currentLoopHandler(current1Pin, 1);
    currentInUseDisplay(1);
  } else {
    rangerLoopHandler();    
  }
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
  lcd.print("               ")
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
  lcd.print(" ")
}

void currentLoopHandler(int pin, int pinId)
{
  lcd.setCursor(0,2);
  lcd.print("A");
  lcd.print(pinId);
  lcd.print(" min:");
  int maxCurrentValue = 0;
  int minCurrentValue = 1024;
  for (int i=0;i<180;i++) {
    int currentValue = analogRead(pin);    
    if (currentValue > maxCurrentValue) {
      maxCurrentValue = currentValue;
    }
    if (currentValue < minCurrentValue) {
      minCurrentValue = currentValue;
    }
  }
  if (maxCurrentValue > 100) {
    inUseSeconds[pinId]++;
  }

  lcd.print(minCurrentValue);
  lcd.print(" max:");
  lcd.print(maxCurrentValue);
  if (maxCurrentValue < 1000) {
    lcd.print(" ");
  }
}

void currentInUseDisplay(int pinId) {
  lcd.setCursor(0,3);
  lcd.print("A");
  lcd.print(pinId);
  lcd.print(" inUse:");
  lcd.print(inUseSeconds[pinId]);
  lcd.print("   ");
}


