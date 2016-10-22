#include <Arduino.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <FS.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

#include "config.h"
#include "utils.h"
#include "setupmode.h"

void setup() {
    //Init status LED to BLU on boot
    statusLed.begin();
    statusLed.setPixelColor(0, statusLed.Color(0,0,125));
    statusLed.show();
    //Set WiFi to station mode
    WiFi.mode(WIFI_STA);
    //Init Serial port
    Serial.begin(115200);
    //Init SPIFFS
    SPIFFS.begin();
    //Init RGB LED
    led.begin();
    led.show();
    blinkLed.blue(&led, 50, 3);
    //DEBUG
    //printKeys(key1,key2,key3);

    if (!loadWiFiSavedConfig()) {
        Serial.println("WARNING: WiFi configuration not found");
        blinkLed.red(&led, 50, 3);
        statusLed.setPixelColor(0, statusLed.Color(125,0,0));
        statusLed.show();
        isOnline = (boolean) false;
    } else if (!checkWiFiConnection()) {
        Serial.println("ERROR: Connection lost");
        statusLed.setPixelColor(0, statusLed.Color(125,0,0));
        statusLed.show();
        blinkLed.red(&led, 50, 3);
        isOnline = (boolean) false;
    }

    //Init reader
    SPI.begin();
    pn532.begin();
    pn532.SAMConfig();

    //System ready!
    if (isOnline) {
        statusLed.setPixelColor(0, statusLed.Color(0,125,0));
        statusLed.show();
        Serial.println("System online.");
        Serial.println("Ready to scan:");
    } else {
        statusLed.setPixelColor(0, statusLed.Color(125,0,0));
        statusLed.show();
        Serial.println("System offline.");
        setupModeStage1();
    }
}

void loop() {
    if (apmode) {
        DNS_SERVER.processNextRequest();
        WEB_SERVER.handleClient();
        statusLed.setPixelColor(0, statusLed.Color(204,0,102));
        statusLed.show();
        if ((millis() - startTime) > TIMEOUT) {
            Serial.println("Set up mode timed out.");
            delay(1000);
            ESP.restart();
        }
        return;
    }

    if (setupModeStatus) {
        pn532.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
        if (uidLength == 4) {
            if (pn532.mifareclassic_AuthenticateBlock(uid, uidLength, block, 0, key2)) {
                Serial.println("Block has been authenticated");
                uint8_t data[16];
                if (pn532.mifareclassic_ReadDataBlock(block, data)) {
                    blinkLed.green(&led, 50, 3);
                    Serial.println("Reading Block ");
                    pn532.PrintHexChar(data, 16);
                    String dataToSend = String((const char *) data);
                    String firstID = dataToSend.substring(0,2);
                    if (firstID == masterID) {
                        setupModeStage2();
                        apmode = (boolean) true;
                        return;
                    }
                    delay(3000);
                } else {
                    Serial.println("Ooops ... unable to read the requested block.  Try another key?");
                    blinkLed.red(&led, 50, 3);
                }
            } else {
                Serial.println("Ooops ... authentication failed: Try another key?");
                blinkLed.red(&led, 50, 3);
            }
        }

    }
    if (isOnline) {
        pn532.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
        if (uidLength == 4) {
            if (pn532.mifareclassic_AuthenticateBlock(uid, uidLength, block, 0, key2)) {
                Serial.println("Block has been authenticated");
                uint8_t data[16];
                if (pn532.mifareclassic_ReadDataBlock(block, data)) {
                    blinkLed.green(&led, 50, 3);
                    Serial.println("Reading Block ");
                    pn532.PrintHexChar(data, 16);
                    String dataToSend = String((const char *) data);
                    String firstID = dataToSend.substring(0,2);
                    if (firstID == masterID) {
                        Serial.println("********MasterID*************");
                        saveJsonConfig("wifi", "ssid", "");
                        Serial.println("*****Reboot in SetupMode*****");
                        delay(500);
                        ESP.reset();
                        return;
                    }
                    customurl(dataToSend);
                    delay(3000);
                } else {
                    Serial.println("Ooops ... unable to read the requested block.  Try another key?");
                    blinkLed.red(&led, 50, 3);
                }
            } else {
                Serial.println("Ooops ... authentication failed: Try another key?");
                blinkLed.red(&led, 50, 3);
            }
        }

    }
}
