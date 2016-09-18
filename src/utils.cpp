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