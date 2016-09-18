#include <Arduino.h>

#include <SPI.h>
#include <MFRC522.h>

#include "config.h"
#include "utils.h"

void setup() {
    //Init Serial port
    Serial.begin(115200);
    //Init RGB LED
    led.begin();
    led.show();
    blinkLed.violet(&led, 250, 2);
    //Init SPI and PCD
    SPI.begin();
    mfrc522.PCD_Init();
    //System ready!
    Serial.println("System ready:");
    //DEBUG
    printKeys(key1,key2,key3);
}

void loop() {
    // Look for new cards
    if ( ! mfrc522.PICC_IsNewCardPresent()) {
        delay(50);
        return;
    }
    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial()) {
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
    if (    piccType != MFRC522::PICC_TYPE_MIFARE_MINI
            &&  piccType != MFRC522::PICC_TYPE_MIFARE_1K
            &&  piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
        Serial.println(F("This reader works with MIFARE Classic cards only."));
        blinkLed.red(&led, 50, 3);
        return;
    }

    MFRC522::StatusCode status;

    // Authenticate using key A
    Serial.println(F("Authenticating using key A..."));
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlockA, (MFRC522::MIFARE_Key *) &key1, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        blinkLed.red(&led, 50, 3);
        return;
    }

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

    // Halt PICC
    mfrc522.PICC_HaltA();
    // Stop encryption on PCD
    mfrc522.PCD_StopCrypto1();
    blinkLed.green(&led, 50, 3);
}

