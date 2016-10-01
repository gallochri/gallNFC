/*
Wiring the MFRC522 to ESP8266
            ESP12-e     Wemos-D1 mini

RGB_LED =   GPIO4       D2
RST     =   GPIO5       D1
SDA(SS) =   GPIO2       D4

MOSI    =   GPIO13      D7
MISO    =   GPIO12      D6
SCK     =   GPIO14      D5

 GND     =   GND         GND
3.3V    =   3.3V        3.3V

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
#define RGB_PIN 4

//Global objects
MFRC522 mfrc522(SS_PIN, RST_PIN);
Adafruit_NeoPixel led = Adafruit_NeoPixel(1, RGB_PIN, NEO_GRB + NEO_KHZ800);
ColorBlink blinkLed = ColorBlink();
ESP8266WebServer WEB_SERVER(80);
DNSServer DNS_SERVER;

//Global vars
unsigned long startTime;
#define TIMEOUT (240*1000)  //Setupmode timeout in milliseconds
boolean setupModeStatus = false;
boolean isOnline = true;
IPAddress AP_IP(192, 168, 5, 1);
String CHIP_ID = String(ESP.getChipId());
String DEVICE_TITLE = "AIRBUTTON";
String AP_SSID = "**" + DEVICE_TITLE + "-" + CHIP_ID + "**";
String SSID_LIST;

//card configuration
MFRC522::MIFARE_Key keyA;
byte sectorA = 9;
byte trailerBlockA = 39;

MFRC522::MIFARE_Key keyB;
byte sectorB = 10;
byte trailerBlockB = 43;

MFRC522::MIFARE_Key keyC;
byte sectorC = 11;
byte trailerBlockC = 47;
byte sectorD = 12;
byte trailerBlockD = 51;

//Block to send
byte block = 36;

#endif //CONFIG_H
