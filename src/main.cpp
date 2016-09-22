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

        // Show some details of the PICC (that is: the tag/card)
        Serial.print(F("Card UID:"));
        dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
        Serial.println();
        Serial.print(F("PICC type: "));
        MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
        Serial.println(mfrc522.PICC_GetTypeName(piccType));

        // Check for compatibility
        if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI
            && piccType != MFRC522::PICC_TYPE_MIFARE_1K
            && piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
            Serial.println(F("This reader works with MIFARE Classic cards only."));
            blinkLed.red(&led, 50, 3);
            return;
        }

        MFRC522::StatusCode status;

        // Authenticate using key A
        Serial.println(F("Authenticating using key A..."));
        status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlockA, (MFRC522::MIFARE_Key *) &key1,
                                          &(mfrc522.uid));
        if (status != MFRC522::STATUS_OK) {
            Serial.print(F("PCD_Authenticate() failed: "));
            Serial.println(mfrc522.GetStatusCodeName(status));
            blinkLed.red(&led, 50, 3);
            return;
        }

     //   mfrc522.PICC_DumpMifareClassicToSerial(&(mfrc522.uid), piccType, (MFRC522::MIFARE_Key *) &key1);
        // Show the whole sector as it currently is
        Serial.println(F("Current data in sector:"));
        mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), (MFRC522::MIFARE_Key *) &key1, sectorA);
        Serial.println();

        // Show the whole sector as it currently is
        Serial.println(F("Current data in sector:"));
        mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), (MFRC522::MIFARE_Key *) &key2, sectorB);
        Serial.println();

        // Show the whole sector as it currently is
        Serial.println(F("Current data in sector:"));
        mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), (MFRC522::MIFARE_Key *) &key3, sectorC);
        Serial.println();

        // Show the whole sector as it currently is
        Serial.println(F("Current data in sector:"));
        mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), (MFRC522::MIFARE_Key *) &key3, sectorD);
        Serial.println();

        Serial.println(F("TEST:"));
        byte block = 36;
        PICC_DumpMifareClassicBlockToSerial(mfrc522, &(mfrc522.uid), (MFRC522::MIFARE_Key *) &key1, sectorA, block);
        Serial.println();

        // Halt PICC
        mfrc522.PICC_HaltA();
        // Stop encryption on PCD
        mfrc522.PCD_StopCrypto1();
        blinkLed.green(&led, 50, 3);
    }
}
