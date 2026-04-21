#include "WiFiManager.h"
#include "../config/Config.h" // Zugang zu den WLAN-Konstanten

WiFiManager::WiFiManager() {
    pinMode(WIFI_SELECT_PIN, INPUT); // Pin für WLAN-Auswahl initialisieren
}

void WiFiManager::connect() {
    Serial.println("Attempting to connect to WiFi...");

    const char* ssid;
    const char* password;

    // if (digitalRead(WIFI_SELECT_PIN) == LOW) {
    //     ssid = JUFOSSID;
    //     password = JUFOPASSWORD;
    //     Serial.println("Connecting to Jufo WiFi...");
    //Debugging

    //} else {
        ssid = FRITZISSID;
        password = FRITZIPASSWORD;
        Serial.println("Connecting to Fritzi WiFi...");
    //}

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }

    Serial.println("\nWiFi Connected!");
    printWifiStatus();
}

void WiFiManager::printWifiStatus() {
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("MAC Address: ");
    Serial.println(WiFi.macAddress());
}