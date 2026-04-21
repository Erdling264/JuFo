#ifndef MY_MQTT_CLIENT_H
#define MY_MQTT_CLIENT_H

#include <PubSubClient.h>
#include <WiFiClient.h> // Da PubSubClient einen WiFiClient benötigt

extern WiFiClient espClient; // Externe Referenz auf das globale WiFiClient-Objekt

class MQTTClient {
public:
    MQTTClient();
    void setupClient(const char* server_ip);
    void ensureConnected();
    void publish(const char* topic, const char* payload);
    void loop(); // Muss regelmäßig aufgerufen werden, um Nachrichten zu verarbeiten
private:
    PubSubClient _client;
    long _lastReconnectAttempt = 0;

    void reconnect();
};

#endif // MY_MQTT_CLIENT_H