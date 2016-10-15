#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <FS.h>
#include <SPI.h>
#include <MFRC522.h>

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

    //Init
    SPI.begin();
    mfrc522.PCD_Init();
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
    //DEBUG
    //mfrc522.PCD_DumpVersionToSerial();
    //byte gain = mfrc522.PCD_GetAntennaGain();
    //Serial.println(gain,HEX);
    mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);
    //gain = mfrc522.PCD_GetAntennaGain();
    //Serial.println(gain,HEX);
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
        if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
            return;
        }


        String dataToSend = PICC_DumpMifareClassicBlockToString(mfrc522,
                                                                &(mfrc522.uid),
                                                                (MFRC522::MIFARE_Key *) &key2,
                                                                sectorB, block);
        String firstID = dataToSend.substring(0,6);

        //DEBUG
        //Serial.println(dataToSend.c_str());

        if (firstID == masterID) {
            mfrc522.PICC_HaltA();
            mfrc522.PCD_StopCrypto1();
            mfrc522.PCD_AntennaOff();
            setupModeStage2();
            apmode = (boolean) true;
            return;
        }
        mfrc522.PICC_HaltA();
        mfrc522.PCD_StopCrypto1();
    }

    if (isOnline) {
        if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
            return;
        }

        String dataToSend = PICC_DumpMifareClassicBlockToString(mfrc522,
                                                                &(mfrc522.uid),
                                                                (MFRC522::MIFARE_Key *) &key2,
                                                                sectorB, block);
        String firstID = dataToSend.substring(0,6);

        //DEBUG
        //Serial.println(dataToSend.c_str());

        if (dataToSend != "CARD ERROR") {
            if (firstID == masterID){
                Serial.println("********MasterID*************");
                saveJsonConfig("wifi", "ssid", "");
                Serial.println("*****Reboot in SetupMode*****");
                delay(500);
                ESP.reset();
                return;
            }
            customurl(dataToSend);
            delay(3000);
        }
        // Halt PICC & Stop encryption on PCD
        mfrc522.PICC_HaltA();
        mfrc522.PCD_StopCrypto1();
    }
}
