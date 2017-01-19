// Copyright (C) 2017, Samuel Ivan Gunadi
// All rights reserved.

#include <Wire.h>
#include <LiquidCrystal_I2C.h> // LCD library
#include <DHT.h> // DHT sensor library by AdaFruit

DHT dht(2, DHT22); // Initialize DHT22 (AM2302) sensor on pin 2
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 

float humidity;
float temperature;
int light_intensity;

void setup()
{
  Serial.begin(9600);
  dht.begin();
  lcd.begin(20,4);
  // blink
  delay(500);
  lcd.noBacklight();
  delay(500);
  lcd.backlight(); 
}

void loop()
{
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  light_intensity = analogRead(0); // range: 0-1023
  lcd.clear();
  lcd.setCursor(0,0);
  String text;
  text += temperature;
  text += " C ";
  text += humidity;
  text += " % ";
  lcd.print(text);
  lcd.setCursor(0,1);
  text = "";
  text += light_intensity;
  text += " light";
  lcd.print(text);
  lcd.setCursor(0,3);
  text = "Hello world!";
  lcd.print(text);
  delay(2000);
}

