#include <Arduino.h>

/*
Many thanks to nikxha from the ESP8266 forum
*/

#include <SPI.h>
#include <MFRC522.h>

#include "config.h"

// Helper routine to dump a byte array as hex values to Serial
void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}

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
    //Reading keys
    for (int i = 0; i < 6; i++) {
        keyA.keyByte[i] = key1[i];
    }
    for (int i = 0; i < 6; i++) {
        keyB.keyByte[i] = key2[i];
    }
    for (int i = 0; i < 6; i++) {
        keyC.keyByte[i] = key3[i];
    }
    /*
    Serial.println(F("Scan a MIFARE Classic PICC to demonstrate Value Block mode."));
    Serial.print(F("Using key (for A) in sector 9:"));
    dump_byte_array(keyA.keyByte, MFRC522::MF_KEY_SIZE);
    Serial.println();
    Serial.print(F("Using key (for A) in sector 10:"));
    dump_byte_array(keyB.keyByte, MFRC522::MF_KEY_SIZE);
    Serial.println();
    Serial.print(F("Using key (for A) in sector 11 and 12:"));
    dump_byte_array(keyC.keyByte, MFRC522::MF_KEY_SIZE);
    Serial.println();
    */
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
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlockA, &keyA, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        blinkLed.red(&led, 50, 3);
        return;
    }

    // Show the whole sector as it currently is
    Serial.println(F("Current data in sector:"));
    mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &keyA, sectorA);
    Serial.println();

    // Show the whole sector as it currently is
    Serial.println(F("Current data in sector:"));
    mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &keyB, sectorB);
    Serial.println();

    // Show the whole sector as it currently is
    Serial.println(F("Current data in sector:"));
    mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &keyC, sectorC);
    Serial.println();

    // Show the whole sector as it currently is
    Serial.println(F("Current data in sector:"));
    mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &keyC, sectorD);
    Serial.println();

    // Halt PICC
    mfrc522.PICC_HaltA();
    // Stop encryption on PCD
    mfrc522.PCD_StopCrypto1();
    blinkLed.green(&led, 50, 3);
}

