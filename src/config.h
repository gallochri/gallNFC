/*
Wiring the MFRC522 to ESP8266
            ESP12-e     Wemos D1 mini PRO

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

//TODO add vars for debug on serial
//Uncomment to enable serial print
#define DEBUGPRINT Serial

//Global objects
MFRC522 mfrc522(SS_PIN, RST_PIN);
Adafruit_NeoPixel led = Adafruit_NeoPixel(1, RGB_PIN1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel statusLed = Adafruit_NeoPixel(1,RGB_PIN2, NEO_GRB + NEO_KHZ800);
ColorBlink blinkLed = ColorBlink();
ESP8266WebServer WEB_SERVER(80);
DNSServer DNS_SERVER;

//Mode vars
boolean apmode = (boolean) false;                               //True if access point is on
boolean isOnline = (boolean) true;                              //True if successful connection
boolean setupModeStatus = (boolean) false;                      //True when waiting master cardID
unsigned long startTime;                                        //Access point startup time
#define TIMEOUT (240*1000)                                      //Access point timeout
IPAddress AP_IP(192, 168, 5, 1);                                //Access point IP address
String CHIP_ID = String(ESP.getChipId());                       //Chip ID
String DEVICE_TITLE = "gallNFC";                                //Brand
String AP_SSID = "**" + DEVICE_TITLE + "-" + CHIP_ID + "**";    //Access point SSID
String SSID_LIST;                                               //String to store scanned SSID

//Card vars
String masterID = " 44 4f";                                     //Master config card ID

byte sectorA = 9;                                               //First data block
byte trailerBlockA = 39;                                        //access with key1

byte sectorB = 10;                                              //Second data block
byte trailerBlockB = 43;                                        //access with key2

byte sectorC = 11;                                              //Third data block
byte trailerBlockC = 47;                                        //access with key3

byte sectorD = 12;                                              //Fourth data block
byte trailerBlockD = 51;                                        //access with key3

byte block = 40;                                                //Block containing the data that will be transmitted

#endif //CONFIG_H
