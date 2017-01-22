// Copyright (C) 2017, Samuel Ivan Gunadi
// All rights reserved.

#include <YunClient.h>
#include <ThingerYun.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h> // LCD library
#include <DHT.h> // DHT sensor library by AdaFruit
#include <TSL2561.h> // TSL2561 sensor library by AdaFruit

// global variables
DHT dht(4, DHT22); // Initialize DHT22 (AM2302) sensor on pin 4
TSL2561 tsl = TSL2561(TSL2561_ADDR_FLOAT);
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 

const char* thinger_username = "qbit";
const char* thinger_device_id = "yun_1";
const char* thinger_device_credential = "05C1CF1A1BDD3AD06C024EA6D647D9FCBA57CC79825A43F0803955DAF2688C124FAD0951A5AE322CB1DA6D7512D4A1B9E2CE591DDBFC726D73A6CD0F91FE41AD";

ThingerYun thinger_instance(thinger_username, thinger_device_id, thinger_device_credential);

float relative_humidity_percent;
float temperature_celcius;
float light_intensity_lux;
float soil_moisture_percent;

float soil_moisture_percent_too_dry_threshold = 25;
bool cloud_rule = false;
bool remote = false;
volatile bool data_transmission_toggle = false;

// interrupt service routine
void toggle_transmission_isr()
{
  data_transmission_toggle = !data_transmission_toggle;
  remote = false;
}

// arduino setup
void setup()
{
  // initialize serial comm
  Serial.begin(9600);
  // initialize bridge to Linux
  Bridge.begin();
  // initialize sensors
  dht.begin();
  tsl.begin();
  tsl.setGain(TSL2561_GAIN_0X);
  tsl.setTiming(TSL2561_INTEGRATIONTIME_13MS);  
  // initialize lcd and turn on backlight
  lcd.begin(20, 4);
  lcd.backlight();

  // set up thinger.io stuff
  // outputs
  thinger_instance["temperature_celcius"] >> [](pson& out)// C++11 lambdas
  {
    out = temperature_celcius;
  };
  thinger_instance["relative_humidity_percent"] >> [](pson& out)
  {
    out = relative_humidity_percent;
  };
  thinger_instance["light_intensity_lux"] >> [](pson& out)
  {
    out = light_intensity_lux;
  };
  thinger_instance["soil_moisture_percent"] >> [](pson& out)
  {
    out = soil_moisture_percent;
  };
  // inputs
  thinger_instance["too_dry_threshold"] << [](pson& in)
  {
    if(in.is_empty())
    {
        in = soil_moisture_percent_too_dry_threshold;
    }
    else
    {
        soil_moisture_percent_too_dry_threshold = in;
        cloud_rule = true;
    } 
  };
  thinger_instance["transmission_toggle"] << [](pson& in)
  {
    if(in.is_empty())
    {
        in = data_transmission_toggle;
    }
    else
    {
        data_transmission_toggle = in;
        remote = true;
    } 
  };
  
  attachInterrupt(digitalPinToInterrupt(7), toggle_transmission_isr, RISING);
  
}

// arduino loop
void loop()
{
  // read values from sensors
  temperature_celcius = dht.readTemperature();
  relative_humidity_percent = dht.readHumidity();
  soil_moisture_percent = (float)analogRead(0) / 1023 * 100;
  uint32_t lum = tsl.getFullLuminosity();
  uint16_t ir, full;
  ir = lum >> 16;
  full = lum & 0xFFFF;
  light_intensity_lux = tsl.calculateLux(full, ir);

  // print to LCD
  lcd.clear();
  lcd.setCursor(0,0);
  String text = "T: ";
  text += temperature_celcius;
  text += " H: ";
  text += relative_humidity_percent;
  lcd.print(text);
  lcd.setCursor(0,1);
  text = "L: ";
  text += light_intensity_lux;
  text += " S: ";
  text += data_transmission_toggle;
  if (remote)
  {
    text += "*";
  }
  lcd.print(text);
  lcd.setCursor(0,2);
  text = "M: ";
  text += soil_moisture_percent;
  text += " D: ";
  text += soil_moisture_percent_too_dry_threshold;
  if (cloud_rule)
  {
    text += "*";
  }
  lcd.print(text);
  lcd.setCursor(0,3);
  // check soil moisture
  if (soil_moisture_percent < soil_moisture_percent_too_dry_threshold)
  {
    text = "Status: soil too dry";
  }
  else
  {
    text = "Status: OK";
  }
  lcd.print(text);

  // thinger.io stuff
  if (!data_transmission_toggle)
  {
    thinger_instance.handle();
  }
  
  // polling interval is 2 seconds
  delay(2000);
}

