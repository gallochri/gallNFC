#include "utils.h"

boolean loadWiFiSavedConfig() {
    String ssid = loadJsonParam("wifi", "ssid");
    if (ssid == "") {
        return (boolean) false;
    }
    String password = loadJsonParam("wifi", "password");
    if (WiFi.begin(ssid.c_str(), password.c_str())) {
        Serial.print("SSID: ");
        Serial.println(ssid);
        Serial.print("Password: ");
        Serial.println(password);
        return (boolean) true;
    }
    return (boolean) false;
}

// Wi-Fi check connection
boolean checkWiFiConnection() {
    int count = 0;
    Serial.print("Waiting to connect to WiFi network");
    while (count < 20) {
        delay(1000);
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println();
            Serial.println("Connected!");
            blinkLed.green(&led, 100, 3);
            return (boolean) true;
        }
        Serial.print(".");
        blinkLed.blue(&led, 100, 1);
        count++;
    }
    Serial.println("Timed out.");
    blinkLed.red(&led, 100, 3);
    return (boolean) false;
}

// Wi-Fi access point list
String ssidList() {
    String ssid_list;
    int n = WiFi.scanNetworks();
    for (int i = 0; i < n; ++i) {
        ssid_list += "\n<option value=\"";
        ssid_list += WiFi.SSID((uint8_t) i);
        ssid_list += "\">";
        ssid_list += WiFi.SSID((uint8_t) i);
        ssid_list += "</option>";
    }
    return ssid_list;
}

// HTML Page maker
String makePage(String device_title, String page_title, String contents) {
    String s = "<!DOCTYPE html>\n<html>\n<head>\n";
    s += "<meta name='viewport' content='width=device-width,user-scalable=0'>\n";
    s += "<link rel='stylesheet' href='css/simple.css'>\n";
    s += "<link rel='stylesheet' href='css/basic.css'>\n";
    s += "<link rel='stylesheet' href='css/custom.css'>\n";
    s += "<title>";
    s += device_title + " | " + page_title;
    s += "</title>\n</head>\n<body>\n<header>\n";
    s += "<img src='img/logo_color_small.png' title='AIRBUTTON' alt='Airbutton Logo' align='middle'>\n";
    s += "</header>\n<div class=\"content-body\">\n";
    s += contents;
    s += "</div>\n</body>\n</html>";
    return s;
}

// Decode URL
String urlDecode(String input) {
    String s = input;
    s.replace("%20", " ");
    s.replace("+", " ");
    s.replace("%21", "!");
    s.replace("%22", "\"");
    s.replace("%23", "#");
    s.replace("%24", "$");
    s.replace("%25", "%");
    s.replace("%26", "&");
    s.replace("%27", "\'");
    s.replace("%28", "(");
    s.replace("%29", ")");
    s.replace("%30", "*");
    s.replace("%31", "+");
    s.replace("%2C", ",");
    s.replace("%2E", ".");
    s.replace("%2F", "/");
    s.replace("%2C", ",");
    s.replace("%3A", ":");
    s.replace("%3A", ";");
    s.replace("%3C", "<");
    s.replace("%3D", "=");
    s.replace("%3E", ">");
    s.replace("%3F", "?");
    s.replace("%40", "@");
    s.replace("%5B", "[");
    s.replace("%5C", "\\");
    s.replace("%5D", "]");
    s.replace("%5E", "^");
    s.replace("%5F", "-");
    s.replace("%60", "`");
    return s;
}

//Load config from Json file in SPIFFS
boolean loadJsonParam(const char *service) {
    File configFile = SPIFFS.open("/config/config.json", "r");
    if (!configFile) {
        Serial.println("ERROR: Failed to open config file (loadJsonParam)");
        return (boolean) false;
    }
    size_t size = configFile.size();
    if (size > 1024) {
        Serial.println("ERROR: Config file size is too large (loadJsonParam)");
        return (boolean) false;
    }
    std::unique_ptr<char[]> buf(new char[size]);
    configFile.readBytes(buf.get(), size);
    DynamicJsonBuffer jsonBuffer;
    JsonObject &json = jsonBuffer.parseObject(buf.get());
    if (!json.success()) {
        Serial.println("ERROR: Failed to parse config file (loadJsonParam)");
        return (boolean) false;
    }
    boolean config = json[service]["enabled"];
    if (config) {
        return (boolean) true;
    }
    return (boolean) false;
}

const char *loadJsonParam(const char *service, const char *param) {
    File configFile = SPIFFS.open("/config/config.json", "r");
    if (!configFile) {
        Serial.println("ERROR: Failed to open config file (loadJsonParam)");
        return (boolean) false;
    }
    size_t size = configFile.size();
    if (size > 1024) {
        Serial.println("ERROR: Config file size is too large (loadJsonParam)");
        return (boolean) false;
    }
    std::unique_ptr<char[]> buf(new char[size]);
    configFile.readBytes(buf.get(), size);
    DynamicJsonBuffer jsonBuffer;
    JsonObject &json = jsonBuffer.parseObject(buf.get());
    if (!json.success()) {
        Serial.println("ERROR: Failed to parse config file (loadJsonParam)");
        return (boolean) false;
    }
    const char *config = json[service][param];
    return (char *) config;
}

//Write config in Json file in SPIFFS
boolean saveJsonConfig(const char *service, const char *param, const char *config) {
    File configFile = SPIFFS.open("/config/config.json", "r");
    if (!configFile) {
        Serial.println("ERROR: Failed to open config file (saveJsonConfig)");
        return (boolean) false;
    }
    size_t size = configFile.size();
    if (size > 1024) {
        Serial.println("ERROR: Config file size is too large (saveJsonConfig)");
        return (boolean) false;
    }
    std::unique_ptr<char[]> buf(new char[size]);
    configFile.readBytes(buf.get(), size);
    DynamicJsonBuffer jsonBuffer;
    JsonObject &json = jsonBuffer.parseObject(buf.get());
    if (!json.success()) {
        Serial.println("ERROR: Failed to parse config file (saveJsonConfig)");
        return (boolean) false;
    }
    configFile.close();
    JsonObject &nested = json[service];
    nested.set(param, config);
    configFile = SPIFFS.open("/config/config.json", "w+");
    json.prettyPrintTo(configFile);
    return (boolean) true;
}

boolean saveJsonConfig(const char *service, const char *param, boolean status) {
    File configFile = SPIFFS.open("/config/config.json", "r");
    if (!configFile) {
        Serial.println("ERROR: Failed to open config file (saveJsonConfig)");
        return (boolean) false;
    }
    size_t size = configFile.size();
    if (size > 1024) {
        Serial.println("ERROR: Config file size is too large (saveJsonConfig)");
        return (boolean) false;
    }
    std::unique_ptr<char[]> buf(new char[size]);
    configFile.readBytes(buf.get(), size);
    DynamicJsonBuffer jsonBuffer;
    JsonObject &json = jsonBuffer.parseObject(buf.get());
    if (!json.success()) {
        Serial.println("ERROR: Failed to parse config file (saveJsonConfig)");
        return (boolean) false;
    }
    configFile.close();
    JsonObject &nested = json[service];
    nested.set(param, status);
    configFile = SPIFFS.open("/config/config.json", "w+");
    json.prettyPrintTo(configFile);
    return (boolean) true;
}

void debugSPIFFS() {
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {
        String fileName = dir.fileName();
        Serial.printf("FS File: %s\n", fileName.c_str());
    }
}


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

void PICC_DumpMifareClassicBlockToSerial(MFRC522 mfrc522,MFRC522::Uid *uid, MFRC522::MIFARE_Key *key, byte sector, byte block){
    MFRC522::StatusCode status;
    byte firstBlock;		// Address of lowest address to dump actually last block dumped
    byte lastBlock;         // Address of highest address
    byte no_of_blocks;		// Number of blocks in sector
    bool isSectorTrailer;	// Set to true while handling the "last" (ie highest address) in the sector.

    // The access bits are stored in a peculiar fashion.
    // There are four groups:
    //		g[3]	Access bits for the sector trailer, block 3 (for sectors 0-31) or block 15 (for sectors 32-39)
    //		g[2]	Access bits for block 2 (for sectors 0-31) or blocks 10-14 (for sectors 32-39)
    //		g[1]	Access bits for block 1 (for sectors 0-31) or blocks 5-9 (for sectors 32-39)
    //		g[0]	Access bits for block 0 (for sectors 0-31) or blocks 0-4 (for sectors 32-39)
    // Each group has access bits [C1 C2 C3]. In this code C1 is MSB and C3 is LSB.
    // The four CX bits are stored together in a nible cx and an inverted nible cx_.
    byte c1, c2, c3;		// Nibbles
    byte c1_, c2_, c3_;		// Inverted nibbles
    bool invertedError;		// True if one of the inverted nibbles did not match
    byte g[4];				// Access bits for each of the four groups.
    byte group;				// 0-3 - active group for access bits
    bool firstInGroup;		// True for the first block dumped in the group

    // Determine position and size of sector.
    if (sector < 32) { // Sectors 0..31 has 4 blocks each
        no_of_blocks = 4;
        firstBlock = sector * no_of_blocks;
    }
    else if (sector < 40) { // Sectors 32-39 has 16 blocks each
        no_of_blocks = 16;
        firstBlock = 128 + (sector - 32) * no_of_blocks;
    }
    else { // Illegal input, no MIFARE Classic PICC has more than 40 sectors.
        return;
    }
    lastBlock = firstBlock + no_of_blocks;
    if (block < firstBlock | block > lastBlock){
        Serial.println(F("ERROR Block out of sector"));
    }
    // Dump blocks, highest address first.
    byte byteCount;
    byte buffer[18];
    byte blockAddr;
    isSectorTrailer = true;
    for (int8_t blockOffset = no_of_blocks - 1; blockOffset >= 0; blockOffset--) {
        blockAddr = firstBlock + blockOffset;
        if (blockAddr == block) {
            // Sector number - only on first line
            if (isSectorTrailer) {
                if (sector < 10)
                    Serial.print(F("   ")); // Pad with spaces
                else
                    Serial.print(F("  ")); // Pad with spaces
                Serial.print(sector);
                Serial.print(F("   "));
            } else {
                Serial.print(F("       "));
            }
            // Block number
            if (blockAddr < 10)
                Serial.print(F("   ")); // Pad with spaces
            else {
                if (blockAddr < 100)
                    Serial.print(F("  ")); // Pad with spaces
                else
                    Serial.print(F(" ")); // Pad with spaces
            }
            Serial.print(blockAddr);
            Serial.print(F("  "));
            // Establish encrypted communications before reading the first block
            if (isSectorTrailer) {
                status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, firstBlock, key, uid);
                if (status != MFRC522::STATUS_OK) {
                    Serial.print(F("PCD_Authenticate() failed: "));
                    Serial.println(mfrc522.GetStatusCodeName(status));
                    return;
                }
            }
            // Read block
            byteCount = sizeof(buffer);
            status = mfrc522.MIFARE_Read(blockAddr, buffer, &byteCount);
            if (status != MFRC522::STATUS_OK) {
                Serial.print(F("MIFARE_Read() failed: "));
                Serial.println(mfrc522.GetStatusCodeName(status));
                continue;
            }
            // Dump data
            for (byte index = 0; index < 16; index++) {
                if (buffer[index] < 0x10)
                    Serial.print(F(" 0"));
                else
                    Serial.print(F(" "));
                Serial.print(buffer[index], HEX);
                if ((index % 4) == 3) {
                    Serial.print(F(" "));
                }
            }

            // Parse sector trailer data
            if (isSectorTrailer) {
                c1 = buffer[7] >> 4;
                c2 = buffer[8] & 0xF;
                c3 = buffer[8] >> 4;
                c1_ = buffer[6] & 0xF;
                c2_ = buffer[6] >> 4;
                c3_ = buffer[7] & 0xF;
                invertedError = (c1 != (~c1_ & 0xF)) || (c2 != (~c2_ & 0xF)) || (c3 != (~c3_ & 0xF));
                g[0] = ((c1 & 1) << 2) | ((c2 & 1) << 1) | ((c3 & 1) << 0);
                g[1] = ((c1 & 2) << 1) | ((c2 & 2) << 0) | ((c3 & 2) >> 1);
                g[2] = ((c1 & 4) << 0) | ((c2 & 4) >> 1) | ((c3 & 4) >> 2);
                g[3] = ((c1 & 8) >> 1) | ((c2 & 8) >> 2) | ((c3 & 8) >> 3);
                isSectorTrailer = false;
            }

            // Which access group is this block in?
            if (no_of_blocks == 4) {
                group = blockOffset;
                firstInGroup = true;
            } else {
                group = blockOffset / 5;
                firstInGroup = (group == 3) || (group != (blockOffset + 1) / 5);
            }
/*
            if (firstInGroup) {
                // Print access bits
                Serial.print(F(" [ "));
                Serial.print((g[group] >> 2) & 1, DEC);
                Serial.print(F(" "));
                Serial.print((g[group] >> 1) & 1, DEC);
                Serial.print(F(" "));
                Serial.print((g[group] >> 0) & 1, DEC);
                Serial.print(F(" ] "));
                if (invertedError) {
                    Serial.print(F(" Inverted access bits did not match! "));
                }
            }
*/
            if (group != 3 && (g[group] == 1 || g[group] == 6)) { // Not a sector trailer, a value block
                long value =
                        (long(buffer[3]) << 24) | (long(buffer[2]) << 16) | (long(buffer[1]) << 8) | long(buffer[0]);
                Serial.print(F(" Value=0x"));
                Serial.print(value, HEX);
                Serial.print(F(" Adr=0x"));
                Serial.print(buffer[12], HEX);
            }
            Serial.println();
        }
    }
    return;

}