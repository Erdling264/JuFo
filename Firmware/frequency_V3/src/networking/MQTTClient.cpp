#include "MQTTClient.h"
#include "../config/Config.h" // Für MQTT-Konstanten
#include <WiFi.h> // Für WiFi.macAddress()

// Globale Instanz von WiFiClient, wie von PubSubClient erwartet
// Dies ist eine der wenigen Stellen, wo ein globales Objekt sinnvoll sein kann,
// da PubSubClient es so erwartet.
WiFiClient espClient;

MQTTClient::MQTTClient() : _client(espClient) { // Initialisiere PubSubClient mit dem WiFiClient
    // Initialisierung kann hier erfolgen, aber setupClient ist besser für dynamische Server-IP
}

void MQTTClient::setupClient(const char* server_ip) {
    _client.setServer(server_ip, MQTT_PORT);
    Serial.println("Setup MQTT");
    // _client.setCallback(callback); // Optional, wenn du Nachrichten empfangen willst
}

void MQTTClient::ensureConnected() {
    Serial.println("MQTT Checking Connection");
    while (!_client.connected()) {
        long now = millis();
        // Versuche nur alle 5 Sekunden neu zu verbinden
        if (now - _lastReconnectAttempt > 5000) {
            _lastReconnectAttempt = now;
            reconnect();
        }
    }
}

void MQTTClient::reconnect() {
    String client_id = MQTT_CLIENT_ID_PREFIX;
    client_id += WiFi.macAddress();
    Serial.printf("Attempting to connect to MQTT broker as %s...\n", client_id.c_str());

    if (_client.connect(client_id.c_str(), MQTT_USERNAME, MQTT_PASSWORD)) {
        Serial.println("MQTT broker connected!");
        // _client.subscribe("your/topic"); // Hier evtl. Topics subskribieren
    } else {
        Serial.print("MQTT connection failed, rc=");
        Serial.print(_client.state());
        Serial.println(" Retrying...");
    }
}

void MQTTClient::publish(const char* topic, const char* payload) {
    if (_client.connected()) {
        _client.publish(topic, payload);
    } else {
        Serial.printf("MQTT not connected, cannot publish to topic: %s\n", topic);
    }
}

void MQTTClient::loop() {
    _client.loop();
}