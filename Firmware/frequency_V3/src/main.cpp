#include <Arduino.h>
#include "config/Config.h"
#include "networking/WiFiManager.h"
#include "networking/MQTTClient.h"
#include "drivers/SensorManager.h"

// Globale Instanzen der Manager-Klassen
WiFiManager wifiManager;
MQTTClient mqttClient;
SensorManager* sensorManager = nullptr; // Als Pointer, um nach MQTTClient zu initialisieren

void setup() {
    Serial.begin(115200);
    delay(100);
    Serial.println("\n--- Starting ESP32 Vacuum Monitor ---");

     pinMode(ONBOARD_LED, OUTPUT);

    // 1. WLAN-Verbindung herstellen
    wifiManager.connect();

    // 2. MQTT-Client konfigurieren und verbinden
    const char* mqttServer;
    if (String(WiFi.SSID()) == String(JUFOSSID)) {
        mqttServer = MQTT_SERVER_JUFO;
    } else {
        mqttServer = MQTT_SERVER_FRITZI;
    }
    mqttClient.setupClient(mqttServer);
    mqttClient.ensureConnected(); // Erste Verbindung herstellen

    // 3. Sensoren-Manager initialisieren (benötigt MQTTClient)
    sensorManager = new SensorManager(mqttClient);
    sensorManager->begin(); // Initialisiert alle Sensoren, inkl. Frequenzzähler

    Serial.println("--- Setup complete ---");
}

void loop() {
    // Blinke die LED, um zu zeigen, dass der ESP läuft
    digitalWrite(ONBOARD_LED, HIGH);
    delay(100);


    // Stelle sicher, dass die MQTT-Verbindung aktiv ist
    //mqttClient.ensureConnected();
    //mqttClient.loop(); // Wichtig, damit PubSubClient seine Arbeit macht

    // Alle Sensoren lesen und publishen, inklusive Frequenzmessung
    sensorManager->readAndPublishAllSensors();

    // Ein Delay am Ende des Loops, um die CPU nicht zu überlasten
    // Die Frequenzmessung hat ihren eigenen Timer, Sensorablesungen können hier etwas warten.
    digitalWrite(ONBOARD_LED, LOW);
    delay(100);
}