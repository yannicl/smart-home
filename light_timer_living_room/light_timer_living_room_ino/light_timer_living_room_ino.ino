/*
 * Author: Yannic
 */
#include <DS3231.h>
#include <Wire.h>

DS3231 Clock;
//initialize variable
bool Century=false;
bool h12=false;
bool PM=false;
byte ADay, AHour, AMinute, ASecond, ABits;
boolean ADy, A12h, Apm;
int second,minute,hour,date,month,year,val; 

String comdata ="";
int numdata[7] = {0},mark = 0;

const int ledPin = 3;
const int switchPin = 2;
bool forceOnState = false;
const int switchMax = 32000; // * 100ms = environ 1h (max int 32767)
int switchCntr = switchMax + 1;
const int printOnceEvery = 50;
int printCntr = 0;

void setup()                                                                                     
{
  pinMode(ledPin, OUTPUT);
  pinMode(switchPin, INPUT);
  Wire.begin();
    
  Serial.begin(9600);
  Serial.println("set_time :");
  Serial.println("year mouth day week hour minute second");
  Serial.println();
  Serial.println("week : 1 -> Sunday; 2 -> Monday; 3 -> Tuesday:  ....7 -> Saturday");
  Serial.println();
  Serial.println("for example :2016-5-20 Tue 0:33:30  ");
  Serial.println("set_time :");
  Serial.println("16 5 20 3 0 33 30");
  Serial.println();
  delay(2000);

}
void loop() 
{  
   timerLoopHandler();
   switchLoopHandler();
   Serial_data();
   delay(100);
}
void WriteDS3231()
{
  Clock.setSecond(numdata[6]); 
  Clock.setMinute(numdata[5]); 
  Clock.setHour(numdata[4]);   
  Clock.setDoW(numdata[3]);    
  Clock.setDate(numdata[2]);   
  Clock.setMonth(numdata[1]);  
  Clock.setYear(numdata[0]);   
}
void Serial_data()
{
  int j = 0;
  while (Serial.available() > 0)    //Serial data detection
  {
    comdata += char(Serial.read());
    delay(2);
    mark = 1;
    Print_time();
  }
  if(mark == 1)  
  {
    Serial.println(comdata);             //Already detected data
    for(int i = 0; i < comdata.length() ; i++) //data conversion
    {
      if(comdata[i] == ' ')
      {
        j++;
      }
      else
      {
        numdata[j] = numdata[j] * 10 + (comdata[i] - '0');
      }
    }

   comdata = String("");
   Serial.print("set_time... ");
   WriteDS3231();
   Serial.println(" OK ");
   for(int i = 0; i < 7; i++)
    {
      numdata[i] = 0;
    }
    mark = 0;
  }
 if (printCntr > printOnceEvery) {
   Print_time();
   printCntr = 0;
 }
 printCntr++;

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
    
  if ((hour < 23 && hour > 16) || forceOnState){
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin,LOW);
  } 
}
void Print_time()
{
  int second,minute,hour,date,year,dow,temperature;
  second=Clock.getSecond();
  minute=Clock.getMinute();
  hour=Clock.getHour(h12,PM);
  date=Clock.getDate();
  year=Clock.getYear();
  dow=Clock.getDoW();

  temperature=Clock.getTemperature();
  
  Serial.print(hour);
  Serial.print(":");
  Serial.print(minute);
  Serial.print(":");
  Serial.print(second);
  Serial.print(" - ");
  Serial.print(temperature);
  Serial.print(" - ");
  Serial.println(switchCntr);
  
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

