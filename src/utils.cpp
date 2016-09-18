#include <MFRC522.h>
#include "utils.h"

// Helper routine to dump a byte array as hex values to Serial
void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}

//Debug function to print keys values
void printKeys(byte key1[MFRC522::MF_KEY_SIZE],
               byte key2[MFRC522::MF_KEY_SIZE],
               byte key3[MFRC522::MF_KEY_SIZE]) {
    Serial.print("Key1 value:");
    dump_byte_array(key1, MFRC522::MF_KEY_SIZE);
    Serial.println();
    Serial.print("Key2 value:");
    dump_byte_array(key2, MFRC522::MF_KEY_SIZE);
    Serial.println();
    Serial.print("Key3 value:");
    dump_byte_array(key3, MFRC522::MF_KEY_SIZE);
    Serial.println();
}