/*
 * Author: Yannic
 */
#include <DS3231.h>
#include <Wire.h>

/**
 * PINOUT
 */
const int ledPin = 3;
const int switchPin = 2;

/**
 * Constantes
 */
const bool century=false;
const int switchMax = 32000; // * 100ms = environ 1h (max int 32767)
const bool h12=false;
const bool PM=false;

/**
 * Variables
 */
bool forceOnState,timerState = false;
int switchCntr = switchMax + 1;
int second,minute,hour,date,year,month,dow,temperature;
int mainLoopCntr;

/**
 * Objets
 */
DS3231 rtc;
LiquidCrystal_I2C lcd(0x27,20,4);  // Set the LCD I2C address

void setup()  
{
  Wire.begin();
  lcd.begin(20,4);
  lcd.backlight();
}

void loop() 
{
  mainLoopCntr++;
  if ((mainLoopCntr % 10) == 0) {
    printDateTimetoLcd();
    printTimerToLcd();
  }
  switchLoopHandler();
  timerLoopHandler();
  delay(100);
}

void printDateTimetoLcd() {
  lcd.setCursor(0,0);
  printZeroPrefix(hour);
  lcd.print(hour);
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
  lcd.print("Timer: ");
  if (forceOnState) {
     lcd.print("FORCED");
  } else if (timerState) {
     lcd.print("ON    ");
  } else {
     lcd.print("OFF   ");
  }
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
  second=rtc.getSecond();
  minute=rtc.getMinute();
  hour=rtc.getHour(h12,PM);
  date=rtc.getDate();
  month=rtc.getMonth(century);
  year=rtc.getYear();
  dow=rtc.getDoW();
  temperature=rtc.getTemperature();
    
  if ((hour < 23 && hour > 16) || forceOnState){
    timerState = true;
    digitalWrite(ledPin, HIGH);
  } else {
    timerState = false;
    digitalWrite(ledPin,LOW);
  } 
}

