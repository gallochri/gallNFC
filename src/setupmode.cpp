#include "setupmode.h"

void setupModeStage1() {
    Serial.println("Setup mode stage 1 started");
    setupModeStatus = (boolean) true;

    //WiFI start in client mode
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(200);

    //Write SSID_LIST in html
    Serial.println("Scan WiFi networks");
    SSID_LIST = ssidList();
    delay(100);

    // Settings Page
    WEB_SERVER.on("/wifi.html", handleWiFi);
    WEB_SERVER.on("/customurl.html", handleCustomURL);

    WEB_SERVER.on("/setwifi.html", handleSetWiFi);
    WEB_SERVER.on("/setcustomurl.html", handleSetCustomURL);

    WEB_SERVER.on("/reboot.html", handleReboot);
    WEB_SERVER.serveStatic("/css/basic.css",SPIFFS,"/css/basic.css");
    WEB_SERVER.serveStatic("/css/custom.css",SPIFFS,"/css/custom.css");
    WEB_SERVER.serveStatic("/css/simple.css",SPIFFS,"/css/simple.css");
    WEB_SERVER.serveStatic("/img/logo.png",SPIFFS,"/img/logo.png");
    WEB_SERVER.onNotFound(handleNotFound);

    Serial.println("Setup mode stage 1 terminated");
    Serial.println("Waiting master card to configure.");
    blinkLed.violet(&led, 100, 3);
}

void setupModeStage2() {
    Serial.println("Setup mode stage 2 started");
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(AP_IP, AP_IP, IPAddress(255, 255, 255, 0));
    WiFi.softAP(AP_SSID.c_str());
    DNS_SERVER.start(53, "*", AP_IP);
    Serial.print("Starting Access Point at ");
    Serial.println(WiFi.softAPIP());
    if (!MDNS.begin("gallNFC")) {
        Serial.println("Error setting up MDNS responder!");
        delay(1000);
    }
    Serial.println("mDNS responder started");
    WEB_SERVER.begin();
    MDNS.addService("http", "tcp", 80);
    startTime = millis();
    setupModeStatus = (boolean) false;
    Serial.println("Setup mode stage 2 terminated");
    Serial.println("Waiting client for configure.");
}
