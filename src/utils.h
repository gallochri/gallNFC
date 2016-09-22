#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include <FS.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <Adafruit_NeoPixel.h>
#include <ColorBlink.h>
#include <ArduinoJson.h>

extern Adafruit_NeoPixel led;
extern ColorBlink blinkLed;
extern MFRC522 mfrc522;

// Load WiFi configuration
boolean loadWiFiSavedConfig();

// Wi-Fi check connection
boolean checkWiFiConnection();

// Wi-Fi access point list
String ssidList();

// HTML Page maker
String makePage(String device_title, String page_title, String contents);

// Decode URL
String urlDecode(String input);

//Read configs from Json on SPIFFS
boolean loadJsonParam(const char *service);
const char *loadJsonParam(const char *service, const char *param);

//Write config to Json on SPIFFS
boolean saveJsonConfig(const char *service, const char *param, const char *config);
boolean saveJsonConfig(const char *service, const char *param, boolean status);

//DEBUG Print SPIFFS content to serial
void debugSPIFFS();

//DEBUG Helper routine to dump a byte array as hex values to Serial
void dump_byte_array(byte *buffer, byte bufferSize);

//DEBUG Print keys to serial
void printKeys(byte key1[MFRC522::MF_KEY_SIZE],
               byte key2[MFRC522::MF_KEY_SIZE],
               byte key3[MFRC522::MF_KEY_SIZE]);

void PICC_DumpMifareClassicBlockToSerial(MFRC522 mfrc522, MFRC522::Uid *uid, MFRC522::MIFARE_Key *key, byte sector, byte block);

#endif //UTILS_H
