#include "webserver.h"

void handleNotFound() {
    String iftt_status;
    if (loadJsonParam("ifttt")) {
        iftt_status = "checked";
    }

    String s = "<h2>Configuration Mode</h2>\n";
    s += "<p><a href='wifi.html'>WiFi</a></p>\n";
    s += "<form method='get' action='reboot.html'>\n";
    s += "<fieldset>\n<legend>Select services that you want to enable.</legend>\n<br>\n";
    s += "<p><input type='checkbox' name='ifttt' value='1' title='IFTTT'" + iftt_status +
         "/>&nbsp;&nbsp;&nbsp;<a href='ifttt.html'>IFTTT</a></p>\n";
    s += "<p><input type='submit' value='Submit' /></p>\n";
    s += "</fieldset>\n</form>\n";
    WEB_SERVER.send(200, "text/html", makePage(DEVICE_TITLE, "Configuration mode", s));
}

void handleReboot() {
    String reboot = urlDecode(WEB_SERVER.arg("reboot"));
    boolean rebootnow = reboot.equals("Reboot Now!");
    if (rebootnow){
        Serial.println("Rebooting...");
        ESP.restart();
    }
    String s = "<h2>Summary</h2>\n";
    s += "<p>Services enabled: <p>\n";
    String ifttt = urlDecode(WEB_SERVER.arg("ifttt"));
    boolean ifttt_status = ifttt.equals("1");
    saveJsonConfig("ifttt", "enabled", ifttt_status);
    if (ifttt_status) {
        s += "<p><b>IFTTT</b></p>\n";
    }
    s +="<form method='get' action='reboot.html'>\n";
    s +="<input type='submit' name='reboot' value='Reboot Now!'>\n";
    s +="</form>";
    WEB_SERVER.send(200, "text/html", makePage(DEVICE_TITLE, "Rebooting", s));
    //NOTE only the first time after flash Reboot will stuck the chip
}

void handleWiFi() {
    String s = "<h2>Wi-Fi Settings</h2>\n";
    s += "<p>Please select an SSID from the list or add your own.</p>\n";
    s += "<p>Enter the password and submit.</p>\n";
    s += "<form method='get' action='setwifi.html'>\n";
    s += "<label>SSID: </label>\n<select name='ssid' title='ssid'>";
    s += SSID_LIST;
    s += "\n</select>\n";
    s += "<br><label>or</label>\n<input name='dssid' maxlength='32' placeholder='SSID'/>\n";
    s += "<br><br><label>Password: </label>\n<input name='pass' maxlength='64' placeholder='WiFi password'>\n";
    s += "<br><br><input type='submit' value='Submit'>\n</form>";
    WEB_SERVER.send(200, "text/html", makePage(DEVICE_TITLE, "Wi-Fi Settings", s));
}

void handleSetWiFi() {
    String ssid = urlDecode(WEB_SERVER.arg("ssid"));
    String dssid = urlDecode(WEB_SERVER.arg("dssid"));
    if (!dssid.equals("")) {
        ssid = dssid;
    }
    Serial.print("SSID: ");
    Serial.println(ssid);

    String pass = urlDecode(WEB_SERVER.arg("pass"));
    Serial.print("Password: ");
    Serial.println(pass);

    Serial.println("Writing SSID and Password to config.json...");
    saveJsonConfig("wifi", "enabled", 1);
    saveJsonConfig("wifi", "ssid", ssid.c_str());
    saveJsonConfig("wifi", "password", pass.c_str());
    Serial.println("Done!");
    String s = "<h1>Wifi Setup complete.</h1>\n";
    s += "<p>At restart airbutton will try to connected to <b>";
    s += ssid;
    s += "</b> net, with <b>";
    s += pass;
    s += "</b> as password.\n";
    s += "<br><a href='/'>Back</a></p>";
    WEB_SERVER.send(200, "text/html", makePage(DEVICE_TITLE, "Write Wi-Fi Settings", s));
}