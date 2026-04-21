#include "SensorManager.h"
#include "../config/Config.h" // Für Sensor-Pins und Kalibrierungskonstanten
#include "../utils/Utils.h"  // Für ltos
#include <math.h>

// Buffer für Debug-Ausgaben mit ltos
static char debugBuf[32]; 

SensorManager::SensorManager(MQTTClient& mqtt) : 
    _mqttClient(mqtt),              // Initialisiert die MQTTClient-Referenz
    _frequencyCounter()             // Initialisiert die FrequencyCounter-Instanz
{
    // Pin-Modi für analoge Sensoren setzen
        // ESP32 ADC1 Pins: 32, 33, 34, 35, 36, 39
        // ESP32 ADC2 Pins: 0, 2, 4, 12, 13, 14, 15, 25, 26, 27
    pinMode(PIRANI_ANALOG_PIN, INPUT);
    pinMode(NTC_ANALOG_PIN, INPUT);

    adcAttachPin(PIRANI_ANALOG_PIN);
    analogSetAttenuation(ADC_11db); // Dämpfung ADC 11db, damit bis 3,3V gemessen werden können
    analogReadResolution(12); // Standard ist 12 Bit
}

void SensorManager::begin() {
    // Initialisiere den Frequenzzähler
    _frequencyCounter.begin();
    // Setze den initialen Wert der letzten gemessenen Frequenz
    _lastMeasuredFrequency = _frequencyCounter.getFrequency();
}

// Methode zum Lesen und Glätten analoger Eingangswerte
void SensorManager::readAnalogInput(int pin, float& raw_value) {
    //raw_value = analogReadMilliVolts(pin);
    float sum_mv = 0;
    for (int i = 0; i < ADC_READS_PER_SAMPLE; i++) {
        sum_mv += analogReadMilliVolts(pin); // Liest direkt in Millivolt
    }
    // Gleitender Mittelwert in Millivolt
    //raw_value = (raw_value * (1.0f - ADC_ALPHA)) + ((sum_mv / ADC_READS_PER_SAMPLE) * ADC_ALPHA);
    raw_value = sum_mv / ADC_READS_PER_SAMPLE;
}


// Berechnung des Pirani-Drucks
float SensorManager::calculatePiraniPressure() {
    readAnalogInput(PIRANI_ANALOG_PIN, _piraniAnalogInput); // _piraniAnalogInput speichert jetzt Millivolt
    //Serial.printf("Pirani-Spannung (mV): %.15f\n", _piraniAnalogInput);

    // 1. Spannung V_ADC (am ADC-Eingang) in Millivolt
    float V_ADC_mv = _piraniAnalogInput;
    float V_ADC = V_ADC_mv / 1000.0f; // Umrechnung nach Volt
   Serial.printf("Pirani-Spannung (V): %.15f\n", V_ADC);
    // 2. Spannung V_ADC (nach Spannungsteiler) zurück auf die Sensor-Ausgangsspannung V_Sensor umrechnen
    // V_Sensor = V_ADC * (R1 + R2) / R2
    float V_Sensor = V_ADC * 4.4053f; // Spannungsverhältnis gemessen (unpräzise Widerstände)
     //Serial.printf("Pirani-Spannung an Roehre (V): %.15f\n", V_Sensor);
    // (PIRANI_VOLTAGE_DIVIDER_R1 + PIRANI_VOLTAGE_DIVIDER_R2) / PIRANI_VOLTAGE_DIVIDER_R2;

    // 3. Sensor-Ausgangsspannung V_Sensor in Druck P umrechnen
    // P = 10^(1.5 * V_Sensor - 12) mbar
    float pressure = pow(10, PIRANI_PRESSURE_FACTOR_A * V_Sensor - PIRANI_PRESSURE_FACTOR_B);
    Serial.printf("Pirani-Druck nach Rechnung (mbar): %.15f\n", pressure);
    return pressure;
}


// Berechnung der NTC-Temperatur
float SensorManager::calculateNTC_Temperature() {
    readAnalogInput(NTC_ANALOG_PIN, _ntcAnalogInput);
    // Hier ist es besonders wichtig, die Umrechnung vom Rohwert zum Widerstand exakt abzugleichen.
    // Der Originalcode hatte: NTCvoltage = (((2.5-0.19)/2900)*NTCinput+0.167)
    // Das impliziert, dass NTCinput bereits ein gefilterter Wert ist.
    // Ich verwende hier einen generischeren Ansatz, der davon ausgeht, dass _ntcAnalogInput der Rohwert ist.
    float voltage = (_ntcAnalogInput / 4095.0f) * NTC_REF_VOLTAGE; // Rohwert in Spannung umwandeln
    
    // Die folgende Berechnung aus deinem Originalcode.
    // Überprüfe die Konstanten in Config.h!
    float Rres = (voltage * NTC_REF_VOLTAGE_DIVIDER_RESISTANCE) / (NTC_REF_VOLTAGE - voltage);
    float NTCohm = (Rres * NTC_SERIES_RESISTANCE) / (NTC_SERIES_RESISTANCE - Rres);
    
    return NTC_TEMP_COEFF_A * pow(NTCohm, 3) + 
           NTC_TEMP_COEFF_B * pow(NTCohm, 2) +
           NTC_TEMP_COEFF_C * NTCohm + 
           NTC_TEMP_COEFF_D;
}

// Hilfsfunktion zum Publishen von Floats
void SensorManager::publishFloat(const char* topic, float value, int precision, const char* label) {
    // Fehlerwert -1.0f als Indikator für ungültige Messung
    if (value == -1.0f || isnan(value) || isinf(value)) { 
        Serial.printf("Skipping invalid sensor reading for %s\n", label);
        return;
    }
    char payload[20]; // Puffergröße anpassen, falls längere Zahlen erwartet werden
    sprintf(payload, "%.*e", precision, value);
    _mqttClient.publish(topic, payload);
    Serial.printf("%s (mqtt): %s\n", label, payload);
}

// Verarbeitet die Frequenzmessung, berechnet Schichtdicke/Aufdampfrate und veröffentlicht
void SensorManager::processFrequencyMeasurement() {
    float currentFrequency = _frequencyCounter.getFrequency();

    Serial.printf("Frequency : %s Hz \n", ltos(currentFrequency, debugBuf, 10));
        
    publishFloat(TOPIC_FREQUENCY, currentFrequency, 7, "Frequency");

    // // Nur verarbeiten, wenn eine neue gültige Messung vorliegt und sich geändert hat
    // if (currentFrequency != 0 && currentFrequency != _lastMeasuredFrequency) {
    //     Serial.printf("--------------------------------\n");
    //     Serial.printf("Frequency : %s Hz \n", ltos(currentFrequency, debugBuf, 10));
        
    //     publishFloat(TOPIC_FREQUENCY, currentFrequency, 7, "Frequency");

    //     // Aufdampfrate berechnen und publishen
    //     float aufdampfrate = (currentFrequency - _lastMeasuredFrequency);
    //     // Nur publishen, wenn die Rate sich signifikant von 0 unterscheidet
    //     if (fabs(aufdampfrate) > 0.001f) { // Schwellenwert für "signifikant" anpassen
    //         publishFloat(TOPIC_DIFFERENZSCHICHTDICKE, aufdampfrate, 2, "Aufdampfrate / Differenz Schichtdicke");
    //     }

    //     // Schichtdicke berechnen und publishen
    //     float schichtdicke = (currentFrequency - SCHICHTDICKE_OFFSET_FREQ) / SCHICHTDICKE_CONV_FACTOR;
    //     if (schichtdicke >= 0) { // Nur positive Schichtdicken publishen
    //         publishFloat(TOPIC_SCHICHTDICKE, schichtdicke, 2, "Schichtdicke");
    //     }
        
    //     _lastMeasuredFrequency = currentFrequency;
    //     _frequencyCounter.reset(); // Zähler für die nächste Messung zurücksetzen
    // }

    // Optional: Serial-Input für Oszillator-Frequenz (Testzwecke)
    // Diese Logik könnte hierher verschoben werden, wenn der SensorManager
    // direkten Zugriff auf den Serial-Port haben soll, oder in main.cpp bleiben
    // und dann setFrequencyOscillator aufrufen.
    static String inputString = "";
    static uint32_t osc_freq_input = 0;
    while (Serial.available())
    {
        char inChar = (char)Serial.read();
        inputString += inChar;
        if (inChar == '\n')
        {
            osc_freq_input = inputString.toInt();
            inputString = "";
            if (osc_freq_input != 0)
            {
                setFrequencyOscillator(osc_freq_input);
            }
        }
    }
}

void SensorManager::readAndPublishAllSensors() {
    // Verarbeitet Frequenzmessung und die davon abhängigen Werte
    processFrequencyMeasurement();

    // Liest und veröffentlicht die anderen analogen Sensoren
    float p_pirani = calculatePiraniPressure();
    Serial.printf("Piranisensor Druck: %.3e\n", p_pirani);
    publishFloat(TOPIC_PIRANI, p_pirani, 3, "Piranisensor Druck");

    float temp_ntc = calculateNTC_Temperature();
    Serial.printf("Temperature: %.2e\n", temp_ntc);
    publishFloat(TOPIC_TEMPERATURE, temp_ntc, 2, "Temperature");

}

void SensorManager::setFrequencyOscillator(uint32_t freq_hz) {
    _frequencyCounter.setOscillatorFrequency(freq_hz);
}