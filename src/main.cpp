#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <FS.h>
#include <SPI.h>
#include <MFRC522.h>

#include "config.h"
#include "utils.h"
#include "setupmode.h"

void setup() {
    //Set WiFi to station mode
    WiFi.mode(WIFI_STA);
    //Init Serial port
    Serial.begin(115200);
    //Init SPI and PCD
    SPI.begin();
    SPIFFS.begin();
    mfrc522.PCD_Init();
    //Init RGB LED
    led.begin();
    led.show();
    blinkLed.blue(&led, 50, 3);
    //DEBUG
    //printKeys(key1,key2,key3);

    //Try to load saved config
    if (!loadWiFiSavedConfig()) {
        Serial.println("WARNING: WiFi configuration not found");
        blinkLed.red(&led, 50, 3);
        setupMode();
        return;
    }

    //Check connection
    if (!checkWiFiConnection()) {
        Serial.println("ERROR: Connection lost");
        blinkLed.red(&led, 50, 3);
        setupMode();
        return;
    }
    //System ready!
    Serial.println("System ready:");
}

void loop() {
    if (setupModeStatus) {
        DNS_SERVER.processNextRequest();
        WEB_SERVER.handleClient();
        blinkLed.violet(&led, 0, 10);
        if ((millis() - startTime) > TIMEOUT) {
            Serial.println("Set up mode timed out.");
            Serial.println("WARNING: APixel Board power off");
            delay(1000);
        }
    } else {

        // Look for new cards
        if (!mfrc522.PICC_IsNewCardPresent()) {
            delay(50);
            return;
        }
        // Select one of the cards
        if (!mfrc522.PICC_ReadCardSerial()) {
            delay(50);
            return;
        }

        //DEBUG
        //PICCdetails();


        Serial.print(F("Block:"));
        String dataTosend = PICC_DumpMifareClassicBlockToString(mfrc522, &(mfrc522.uid), (MFRC522::MIFARE_Key *) &key1, sectorA, block);
        Serial.println();

        customurl(dataTosend);

        // Halt PICC
        mfrc522.PICC_HaltA();
        // Stop encryption on PCD
        mfrc522.PCD_StopCrypto1();
        blinkLed.green(&led, 50, 3);
    }
}
