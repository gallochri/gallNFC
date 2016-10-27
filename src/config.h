/*
Wiring the MFRC522 to ESP8266
            ESP12-e     Wemos-D1 mini

GND     =   GND         GND
RST     =   GPIO5       D1
RGB_LED1=   GPIO4       D2
SDA(SS) =   GPIO2       D4

3.3V    =   3.3V        3V3
SCK     =   GPIO14      D5
MISO    =   GPIO12      D6
MOSI    =   GPIO13      D7
RGB_LED2=   GPIO15      D8
*/

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <MFRC522.h>
#include <ColorBlink.h>

//Include key file
#include "keys.h"

//PIN definitions
#define RST_PIN 5
#define SS_PIN 2
#define RGB_PIN1 4
#define RGB_PIN2 15

//Global objects
MFRC522 mfrc522(SS_PIN, RST_PIN);
Adafruit_NeoPixel led = Adafruit_NeoPixel(1, RGB_PIN1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel statusLed = Adafruit_NeoPixel(1,RGB_PIN2, NEO_GRB + NEO_KHZ800);
ColorBlink blinkLed = ColorBlink();
ESP8266WebServer WEB_SERVER(80);
DNSServer DNS_SERVER;

//Global vars
unsigned long startTime;
#define TIMEOUT (240*1000)  //Setupmode timeout in milliseconds
boolean setupModeStatus = (boolean) false;
boolean isOnline = (boolean) true;
boolean apmode = (boolean) false;
IPAddress AP_IP(192, 168, 5, 1);
String CHIP_ID = String(ESP.getChipId());
String DEVICE_TITLE = "gallNFC";
String AP_SSID = "**" + DEVICE_TITLE + "-" + CHIP_ID + "**";
String SSID_LIST;
String masterID = " 44 4f";

//card configuration
byte sectorA = 9;
byte trailerBlockA = 39;

byte sectorB = 10;
byte trailerBlockB = 43;

byte sectorC = 11;
byte trailerBlockC = 47;
byte sectorD = 12;
byte trailerBlockD = 51;

//Block to send
byte block = 40;

#endif //CONFIG_H
