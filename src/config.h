/*
Created by gallochri on 17/09/16.

Wiring the MFRC522 to ESP8266 (ESP-12)
            ESP12-e     Wemos-D1 mini
RST     =   GPIO5       D1
SDA(SS) =   GPIO2       D4
MOSI    =   GPIO13      D7
MISO    =   GPIO12      D6
SCK     =   GPIO14      D5
GND     =   GND
3.3V    = 3.3V

*/

#ifndef GALLNFC_CONFIG_H
#define GALLNFC_CONFIG_H

#include <Arduino.h>
#include <MFRC522.h>
#include "keys.h"

#define RST_PIN 5
#define SS_PIN 2
MFRC522 mfrc522(SS_PIN, RST_PIN);

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

#endif //GALLNFC_CONFIG_H
