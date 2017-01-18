// Copyright (C) 2017, Samuel Ivan Gunadi
// All rights reserved.

#include <DHT.h> // DHT sensor library by AdaFruit

DHT dht(2, DHT22); // Initialize DHT22 (AM2302) sensor on pin 2

float humidity;
float temperature;
int light_intensity;

void setup()
{
  Serial.begin(9600);
  dht.begin();
}

void loop()
{
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  light_intensity = analogRead(0); // range: 0-1023
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %, Temperature: ");
  Serial.print(temperature);
  Serial.print(" Celsius");
  Serial.print(", Light Intensity: ");
  Serial.println(light_intensity);
  delay(2000);
}
