#ifndef GALLNFC_UTILS_H
#define GALLNFC_UTILS_H

#include <Arduino.h>
#include <FS.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <Adafruit_NeoPixel.h>
#include <ColorBlink.h>
#include <ArduinoJson.h>

extern Adafruit_NeoPixel led;
extern ColorBlink blinkLed;

// Load WiFi configuration
boolean loadWiFiSavedConfig();

// Wi-Fi check connection
boolean checkWiFiConnection();

boolean loadJsonParam(const char *service);

const char *loadJsonParam(const char *service, const char *param);

void dump_byte_array(byte *buffer, byte bufferSize);
void printKeys(byte key1[MFRC522::MF_KEY_SIZE],
               byte key2[MFRC522::MF_KEY_SIZE],
               byte key3[MFRC522::MF_KEY_SIZE]);

#endif //GALLNFC_UTILS_H
