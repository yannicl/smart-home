/*
 * Author: SMRAZA KEEN
 * Date:2016/6/29
 * IDE V1.6.9
 * Email:TechnicSmraza@outlook.com
 * Function:
 */
#include <DS3231.h>
#include <Wire.h>

DS3231 Clock;
bool Century=false;
bool h12=false;
bool PM=false;

const int ledPin = 12;

void setup()                                                                                     
{
  Wire.begin();
  pinMode(ledPin, OUTPUT);
  initialPeriodHandler();
}

void loop() 
{  
 timerLoopHandler();
 delay(5000);
}

void initialPeriodHandler(){
  digitalWrite(ledPin,HIGH);
  for(int i=0; i<5; i++){
    delay(1000);
  }
}

void timerLoopHandler()
{
  int second,minute,hour,date,year,dow,temperature;
  second=Clock.getSecond();
  minute=Clock.getMinute();
  hour=Clock.getHour(h12,PM);
  date=Clock.getDate();
  year=Clock.getYear();
  dow=Clock.getDoW();
  temperature=Clock.getTemperature();
    
  if (second >= 10 && second <= 40){
    digitalWrite(ledPin, LOW);
  } else {
    digitalWrite(ledPin,HIGH);
  }
  
}

