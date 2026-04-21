#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Arduino.h>
#include "../networking/MQTTClient.h"
#include "FrequencyCounter.h"

class SensorManager {
public:
    // Konstruktor, der den MQTTClient referenziert
    SensorManager(MQTTClient& mqtt);

    // Initialisiert alle Sensoren, inklusive Frequenzzähler
    void begin();

    // Liest alle Sensordaten, berechnet und veröffentlicht sie über MQTT
    void readAndPublishAllSensors();

    // Ermöglicht das Setzen der Oszillator-Frequenz von außen, z.B. für Tests
    void setFrequencyOscillator(uint32_t freq_hz);

private:
    MQTTClient& _mqttClient; // Referenz auf den MQTTClient
    FrequencyCounter _frequencyCounter; // Instanz des FrequencyCounters

    // Zwischenspeicher für analoge Rohwerte
    float _piraniAnalogInput = 0;
    float _ionivacAnalogInput = 0;
    float _ntcAnalogInput = 0;

    // Für die Berechnung der Aufdampfrate
    float _lastMeasuredFrequency = 0;

    // Hilfsfunktionen zum Lesen und Verarbeiten einzelner Sensor-Typen
    void readAnalogInput(int pin, float& raw_value);
    
    float calculatePiraniPressure();
    float calculateNTC_Temperature();

    // Neue Methode zur Verarbeitung der Frequenzzählerdaten
    void processFrequencyMeasurement();

    // Hilfsfunktion zum Veröffentlichen von Gleitkommazahlen über MQTT
    void publishFloat(const char* topic, float value, int precision, const char* label);
};

#endif // SENSOR_MANAGER_H