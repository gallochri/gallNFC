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
    Serial.begin(115200);
    delay(250);
    Serial.println(F("Booting...."));
    SPI.begin();
    mfrc522.PCD_Init();
    for (int i = 0; i < 6; i++) {
        keyA9.keyByte[i] = key1[i];
    }
    for (int i = 0; i < 6; i++) {
        keyA10.keyByte[i] = key2[i];
    }
    for (int i = 0; i < 6; i++) {
        keyA11_12.keyByte[i] = key3[i];
    }
    Serial.println(F("Scan a MIFARE Classic PICC to demonstrate Value Block mode."));
    Serial.print(F("Using key (for A) in sector 9:"));
    dump_byte_array(keyA9.keyByte, MFRC522::MF_KEY_SIZE);
    Serial.println();
    Serial.print(F("Using key (for A) in sector 10:"));
    dump_byte_array(keyA10.keyByte, MFRC522::MF_KEY_SIZE);
    Serial.println();
    Serial.print(F("Using key (for A) in sector 11 and 12:"));
    dump_byte_array(keyA11_12.keyByte, MFRC522::MF_KEY_SIZE);
    Serial.println();
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
        Serial.println(F("This sample only works with MIFARE Classic cards."));
        return;
    }
    byte sector         = 9;
    byte valueBlockA    = 0;
    byte valueBlockB    = 1;
    byte valueBlockC    = 2;
    byte trailerBlock   = 3;
    MFRC522::StatusCode status;
    byte buffer[18];
    byte size = sizeof(buffer);
    long value;

    // Authenticate using key A
    Serial.println(F("Authenticating using key A..."));
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &keyA9, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }

    // Show the whole sector as it currently is
    Serial.println(F("Current data in sector:"));
    mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &keyA9, sector);
    Serial.println();
}

