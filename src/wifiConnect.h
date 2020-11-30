#include <HTTPClient.h>
#include <WiFi.h>
#include <stdio.h>

const char *ssid = "seu-wlan";  // Your ssid
const char *password = "";      // Your password
const char *id = "";            // Your student id

IPAddress myIP;

void wifi_setup() {
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    // Applying SSID and password
    WiFi.begin(ssid, password);

    // Waiting the connection to a router
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    // Connection is complete
    Serial.println("");

    Serial.println("WiFi connected");

    Serial.println(myIP = WiFi.localIP());

    char url_seuwlan[256];
    sprintf(
        url_seuwlan,
        "http://10.9.10.100:801/eportal/"
        "?c=Portal&a=login&callback=dr1003&login_method=1&user_account=%%2C0%%"
        "2C%s&user_password=123456&wlan_user_ip=%s&wlan_user_"
        "ipv6=&wlan_user_mac=000000000000&wlan_ac_ip=10.1.1.1&wlan_ac_name=&"
        "jsVersion=3.3.2&v=6886",
        id, myIP.toString().c_str());

    // supposed to work, not recommended, edit wifiConnect.h first
    // seu-wlan login, add ID & IP address
    HTTPClient http_seu;
    // seu login
    http_seu.begin(url_seuwlan);
    http_seu.GET();
}
