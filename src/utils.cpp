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
    s += "<img src='img/logo.png' title='gallNFC' alt='gallNFC Logo' align='middle'>\n";
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
        return;
    }

    byte byteCount;
    byte buffer[18];
    isSectorTrailer = true;

    Serial.print(block);
    Serial.print(F("  "));

    if (isSectorTrailer) {
        status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, firstBlock, key, uid);
        if (status != MFRC522::STATUS_OK) {
            Serial.print(F("PCD_Authenticate() failed: "));
            Serial.println(mfrc522.GetStatusCodeName(status));
            blinkLed.red(&led, 50, 3);
            return;
        }
    }

    // Read block
    byteCount = sizeof(buffer);
    status = mfrc522.MIFARE_Read(block, buffer, &byteCount);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Read() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
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

    Serial.println();

    String data = "          ";

    for (byte index = 0; index < 16; index++) {
        if (buffer[index] < 0x10) {
            data += " 0";
        }
        else {
            data += " ";
        }
        data += String (buffer[index],HEX);
        if ((index % 4) == 3) {
            data += (" ");
        }
    }
    printf(data.c_str());
}

String PICC_DumpMifareClassicBlockToString(MFRC522 mfrc522,MFRC522::Uid *uid, MFRC522::MIFARE_Key *key, byte sector, byte block){
    MFRC522::StatusCode status;
    byte firstBlock;		// Address of lowest address to dump actually last block dumped
    byte lastBlock;         // Address of highest address
    byte no_of_blocks;		// Number of blocks in sector
    bool isSectorTrailer;	// Set to true while handling the "last" (ie highest address) in the sector.
    String dataBlock;            // Return data string
    String errorString = "CARD ERROR"; // Return Error String

    // Determine position and size of sector.
    if (sector < 32) { // Sectors 0..31 has 4 blocks each
        no_of_blocks = 4;
        firstBlock = sector * no_of_blocks;
    }
    else if (sector < 40) { // Sectors 32-39 has 16 blocks each
        no_of_blocks = 16;
        firstBlock = 128 + (sector - 32) * no_of_blocks;
    }
    else {
        Serial.print(F("ERROR: Illegal input, no MIFARE Classic PICC has more than 40 sectors."));
        blinkLed.red(&led, 50, 10);
        return errorString;
    }

    lastBlock = firstBlock + no_of_blocks;

    if (block < firstBlock | block > lastBlock){
        Serial.println(F("ERROR Block out of sector"));
        blinkLed.red(&led, 50, 10);
        return errorString;
    }

    byte byteCount;
    byte buffer[18];
    isSectorTrailer = true;

    if (isSectorTrailer) {
        status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, firstBlock, key, uid);
        if (status != MFRC522::STATUS_OK) {
            Serial.print(F("PCD_Authenticate() failed: "));
            Serial.println(mfrc522.GetStatusCodeName(status));
            blinkLed.red(&led, 50, 10);
            return errorString;
        }
    }

    // Read block
    byteCount = sizeof(buffer);
    status = mfrc522.MIFARE_Read(block, buffer, &byteCount);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Read() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        blinkLed.red(&led, 50, 10);
        return errorString;
    }

    for (byte index = 0; index < 16; index++) {
        if (buffer[index] < 0x10) {
            dataBlock += " 0";
        }
        else {
            dataBlock += " ";
        }
        dataBlock += String (buffer[index],HEX);
        if ((index % 4) == 3) {
            dataBlock += (" ");
        }
    }

    blinkLed.green(&led, 50, 10);
    return dataBlock;
}

void PICCdetails(){
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

}