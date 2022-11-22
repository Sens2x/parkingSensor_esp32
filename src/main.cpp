/**
 * @file main.cc
 *
 * @brief main file for coordinating all routines
 *
 * @author Afaq Javed, Abdul Haseeb
 * Dated: 20-11-2022
 *
 */

#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <key.h>
#include "rm3100_esp.h"


// rm3100 options
#define initialCC 200 // Set the cycle count
#define singleMode 0 //0 = use continuous measurement mode; 1 = use single measurement mode
#define useDRDYPin 1 //0 = not using DRDYPin ; 1 = using DRDYPin to wait for data

// WLAN credentials
const char *ssid = "Mr. Wick";
const char *password = "myself.23";
String serverName = "https://sensor.afaqjaved.com/data";
// String url = "https://sensor.afaqjaved.com/data";

#define uS_TO_S_FACTOR 1000000
#define TIME_TO_SLEEP 10 // Seconds

RTC_DATA_ATTR int bootCount = 0;


void setup()
{
  Serial.begin(9600);
  delay(1000); // Take some time to open up the Serial Monitor
  pinMode(26, OUTPUT);
  digitalWrite(26, HIGH);
  pinMode(PIN_DRDY, INPUT); //Data ready pin rm3100
  delay(2000); // For Wire Start
 

  Wire.begin(21, 22);

  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) +
                 " Seconds");

  // **********  Initialize magnetometer RM3100 ***********

  rm3100_init(1); //0 = use continuous measurement mode; 1 = use single measurement mode

// single measurement Routine

  long x = 0;
  long y = 0;
  long z = 0;
  uint8_t x2,x1,x0,y2,y1,y0,z2,z1,z0;

//check if sensor is ready to send data (either use DRDY pin or Status Reg)
  if(useDRDYPin){ 
    
    Serial.println("wait for Data ready");
    while(digitalRead(PIN_DRDY) == LOW)
    {
      Serial.print(".");
    } 
      Serial.print(".Compass");

    //check RDRY pin
  }
  else{
      
      Serial.println("wait for Internal Status Reg");
    while((readReg(RM3100_STATUS_REG) & 0x80) != 0x80){
            Serial.print(".");
    } //read internal status register
  }

  Wire.beginTransmission(RM3100Address);
  Wire.write(0x24); //request from the first measurement results register
  Wire.endTransmission();

  // Request 9 bytes from the measurement results registers
  Wire.requestFrom(RM3100Address, 9);
  if(Wire.available() == 9) {
    x2 = Wire.read();
    x1 = Wire.read();
    x0 = Wire.read();
    
    y2 = Wire.read();
    y1 = Wire.read();
    y0 = Wire.read();
    
    z2 = Wire.read();
    z1 = Wire.read();
    z0 = Wire.read();
  }

  //special bit manipulation since there is not a 24 bit signed int data type
  if (x2 & 0x80){
      x = 0xFF;
  }
  if (y2 & 0x80){
      y = 0xFF;
  }
  if (z2 & 0x80){
      z = 0xFF;
  }

  //format results into single 32 bit signed value
  x = (x * 256 * 256 * 256) | (int32_t)(x2) * 256 * 256 | (uint16_t)(x1) * 256 | x0;
  y = (y * 256 * 256 * 256) | (int32_t)(y2) * 256 * 256 | (uint16_t)(y1) * 256 | y0;
  z = (z * 256 * 256 * 256) | (int32_t)(z2) * 256 * 256 | (uint16_t)(z1) * 256 | z0;

  //calculate magnitude of results
  double uT = sqrt(pow(((float)(x)/gain),2) + pow(((float)(y)/gain),2)+ pow(((float)(z)/gain),2));

  //display results
  Serial.print("Data in counts:");
  Serial.print("   X:");
  Serial.print(x);
  Serial.print("   Y:");
  Serial.print(y);
  Serial.print("   Z:");
  Serial.println(z);

  Serial.print("Data in microTesla(uT):");
  Serial.print("   X:");
  Serial.print((float)(x)/gain);
  Serial.print("   Y:");
  Serial.print((float)(y)/gain);
  Serial.print("   Z:");
  Serial.println((float)(z)/gain);

  //Magnitude should be around 45 uT (+/- 15 uT)
  Serial.print("Magnitude(uT):");
  Serial.println(uT);
  Serial.println();  

// ********************************************************


  WiFi.begin(ssid, password);
  delay(2000);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
// fgf
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  if (WiFi.status() == WL_CONNECTED)
  {
    WiFiClientSecure client;
    HTTPClient http;
    client.setCACert(key);

    http.begin(client, serverName);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String httpRequestData = "x=" + String((float)(x)/gain) + "&y=" + String((float)(y)/gain) + "&z=" + String((float)(z)/gain);

    int httpResponseCode = http.POST(httpRequestData);

    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    http.end();
  }

  Serial.println("Going to sleep now");
  delay(1000);
  Serial.flush();
  Wire.flush();
  digitalWrite(26, LOW);
  esp_deep_sleep_start();
  Serial.println("This will never be printed");
}

void loop()
{
  // This is not going to be called
}

