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

    if (!loadWiFiSavedConfig()) {
        Serial.println("WARNING: WiFi configuration not found");
        blinkLed.red(&led, 50, 3);
        isOnline = (boolean) false;
    } else if (!checkWiFiConnection()) {
        Serial.println("ERROR: Connection lost");
        blinkLed.red(&led, 50, 3);
        isOnline = (boolean) false;
    }

    //System ready!
    if (isOnline) {
        Serial.println("System online.");
        Serial.println("Ready to scan:");
    } else {
        Serial.println("System offline.");
        setupModeStage1();
    }

}

void loop() {
    if (isOnline) {
        if (!mfrc522.PICC_IsNewCardPresent()) {
            delay(10);
            return;
        }
        // Select one of the cards
        if (!mfrc522.PICC_ReadCardSerial()) {
            delay(10);
            return;
        }
        String dataToSend = PICC_DumpMifareClassicBlockToString(mfrc522,
                                                                &(mfrc522.uid),
                                                                (MFRC522::MIFARE_Key *) &key1,
                                                                sectorA, block);
        Serial.println(dataToSend.c_str());

        if (dataToSend != "CARD ERROR") {
            customurl(dataToSend);
        }
        // Halt PICC
        mfrc522.PICC_HaltA();
        // Stop encryption on PCD
        mfrc522.PCD_StopCrypto1();

    } else {
        if (setupModeStatus) {
            if (!mfrc522.PICC_IsNewCardPresent()) {
                delay(10);
                return;
            }
            // Select one of the cards
            if (!mfrc522.PICC_ReadCardSerial()) {
                delay(10);
                return;
            }
            String dataToSend = PICC_DumpMifareClassicBlockToString(mfrc522,
                                                                    &(mfrc522.uid),
                                                                    (MFRC522::MIFARE_Key *) &key1,
                                                                    sectorA, block);
            Serial.println(dataToSend.c_str());

            if (dataToSend != "CARD ERROR") {
                setupModeStage2();
            }
            // Halt PICC
            mfrc522.PICC_HaltA();
            // Stop encryption on PCD
            mfrc522.PCD_StopCrypto1();
        } else {
            DNS_SERVER.processNextRequest();
            WEB_SERVER.handleClient();
            blinkLed.violet(&led, 100, 1);
            if ((millis() - startTime) > TIMEOUT) {
                Serial.println("Set up mode timed out.");
                delay(1000);
                ESP.restart();
            }
        }
    }
}

