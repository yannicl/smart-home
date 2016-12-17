#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <DS3231.h>

LiquidCrystal_I2C lcd(0x27,20,4);  // Set the LCD I2C address
DS3231 rtc;

//initialize variable
bool Century=false;
bool h12=false;
bool PM=false;

void setup()  
{
  Wire.begin();
  lcd.begin(20,4);
  lcd.backlight();
  
  createCustomCharacters();
 
  printFrame();
}

byte verticalLine[8] = {
  B00100,
  B00100,
  B00100,
  B00100,
  B00100,
  B00100,
  B00100,
  B00100
};  

byte char2[8] = {
  B00000,
  B00000,
  B00000,
  B11100,
  B00100,
  B00100,
  B00100,
  B00100
};

byte char1[8] = {
  B00000,
  B00000,
  B00000,
  B00111,
  B00100,
  B00100,
  B00100,
  B00100
};

byte char3[8] = {
  B00100,
  B00100,
  B00100,
  B00111,
  B00000,
  B00000,
  B00000,
  B00000
};

byte char4[8] = {
  B00100,
  B00100,
  B00100,
  B11100,
  B00000,
  B00000,
  B00000,
  B00000
};

void createCustomCharacters()
{
  lcd.createChar(0, verticalLine);
  lcd.createChar(1, char1);
  lcd.createChar(2, char2);
  lcd.createChar(3, char3);
  lcd.createChar(4, char4);
}

void printFrame()
{
  lcd.setCursor(1,0);
  lcd.print("------------------");
  lcd.setCursor(1,3);
  lcd.print("------------------");
  lcd.setCursor(0,1);
  lcd.write(byte(0));
  lcd.setCursor(0,2);
  lcd.write(byte(0));
  lcd.setCursor(19,1);
  lcd.write(byte(0));
  lcd.setCursor(19,2);
  lcd.write(byte(0));
  lcd.setCursor(0,0);
  lcd.write(byte(1));
  lcd.setCursor(19,0);
  lcd.write(byte(2));
  lcd.setCursor(0,3);
  lcd.write(byte(3));
  lcd.setCursor(19,3);
  lcd.write(byte(4));
}

void loop() 
{
  lcd.setCursor(5,1);
  byte seconds,minutes;
  minutes = rtc.getMinute();
  seconds = rtc.getSecond();
  lcd.print(rtc.getHour(h12,PM));
  lcd.print(":");
  if (minutes<10) {
    lcd.print("0");
  }
  lcd.print(minutes);
  lcd.print(":");
  if (seconds<10) {
    lcd.print("0");
  }
  lcd.print(seconds);
  lcd.setCursor(6,2);
  lcd.print(rtc.getYear());
  lcd.print("-");
  lcd.print(rtc.getMonth(Century));
  lcd.print("-");
  lcd.print(rtc.getDate());

  delay(1000);
}
