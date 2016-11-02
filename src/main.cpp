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
    statusLed.setPixelColor(0, statusLed.Color(0,0,70));
    statusLed.show();
    //Set WiFi to station mode
    WiFi.mode(WIFI_STA);
    //Init SPIFFS
    SPIFFS.begin();
    //Init RGB LED
    led.begin();
    led.show();
    blinkLed.blue(&led, 50, 3);
#ifdef DEBUGPRINT
    DEBUGPRINT.begin(115200);
    //printKeys(key1,key2,key3);            //Print keys
    //debugSPIFFS();                        //Print SPIFFS files tree
#endif
    if (!loadWiFiSavedConfig()) {
        blinkLed.red(&led, 50, 3);
        statusLed.setPixelColor(0, statusLed.Color(125,0,0));
        statusLed.show();
        isOnline = (boolean) false;
    } else if (!checkWiFiConnection()) {
        statusLed.setPixelColor(0, statusLed.Color(125,0,0));
        statusLed.show();
        blinkLed.red(&led, 50, 3);
        isOnline = (boolean) false;
    }
    SPI.begin();
    mfrc522.PCD_Init();
    //DEBUG
    //mfrc522.PCD_DumpVersionToSerial();
    mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);
    if (isOnline) {
        statusLed.setPixelColor(0, statusLed.Color(0,70,0));
        statusLed.show();
        DEBUGPRINT.println("System online.");
    } else {
        statusLed.setPixelColor(0, statusLed.Color(125,0,0));
        statusLed.show();
        DEBUGPRINT.println("System offline.");
        setupModeStage1();
    }
}

void loop() {
    if (apmode) {
        DNS_SERVER.processNextRequest();
        WEB_SERVER.handleClient();
        statusLed.setPixelColor(0, statusLed.Color(204,0,102));
        statusLed.show();
        //Restart ESP after timeout
        if ((millis() - startTime) > TIMEOUT) {
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
                saveJsonConfig("wifi", "ssid", "");
                DEBUGPRINT.println("*****Reboot in SetupMode*****");
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
