#ifndef GALLNFC_UTILS_H
#define GALLNFC_UTILS_H

#include <Arduino.h>

void dump_byte_array(byte *buffer, byte bufferSize);
void printKeys(byte key1[MFRC522::MF_KEY_SIZE],
               byte key2[MFRC522::MF_KEY_SIZE],
               byte key3[MFRC522::MF_KEY_SIZE]);

#endif //GALLNFC_UTILS_H
