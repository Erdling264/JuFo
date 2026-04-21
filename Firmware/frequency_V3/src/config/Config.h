#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// --- WLAN-Konfiguration ---
// Jetzt als extern deklariert
extern const char* JUFOSSID;
extern const char* JUFOPASSWORD;
extern const char* FRITZISSID;
extern const char* FRITZIPASSWORD;
extern const int WIFI_SELECT_PIN;

// --- MQTT-Konfiguration ---
extern const char* MQTT_SERVER_JUFO;
extern const char* MQTT_SERVER_FRITZI;
extern const int MQTT_PORT;
extern const char *MQTT_USERNAME;
extern const char *MQTT_PASSWORD;
extern const char *MQTT_CLIENT_ID_PREFIX;

// --- MQTT-Topics ---
extern const char *TOPIC_PIRANI;
extern const char *TOPIC_FREQUENCY;
extern const char *TOPIC_TEMPERATURE;
extern const char *TOPIC_SCHICHTDICKE;
extern const char *TOPIC_DIFFERENZSCHICHTDICKE;

// --- Frequenzzähler-Konfiguration ---
// #define-Makros sind präprozessor-Direktiven und verursachen keine multiple definition errors
// Sie bleiben daher wie sie sind.
#define PCNT_COUNT_UNIT       PCNT_UNIT_0
#define PCNT_COUNT_CHANNEL    PCNT_CHANNEL_0
#define PCNT_INPUT_SIG_IO     GPIO_NUM_34
#define PCNT_INPUT_CTRL_IO    GPIO_NUM_35
#define OUTPUT_CONTROL_GPIO   GPIO_NUM_32
extern const uint32_t PCNT_OVERFLOW_VALUE; // extern machen
extern const uint32_t SAMPLE_TIME_US;      // extern machen

// --- LED-Konfiguration ---
#define ONBOARD_LED           GPIO_NUM_2

// --- Sensoren Pins ---
extern const int PIRANI_ANALOG_PIN;
extern const int NTC_ANALOG_PIN;

// --- Sensoren Kalibrierungs- und Berechnungskonstanten ---
extern const float PIRANI_PRESSURE_FACTOR_A;
extern const float PIRANI_PRESSURE_FACTOR_B;
extern const float PIRANI_VOLTAGE_DIVIDER_R1;   // Spannungsteiler R1 = 33 kOhm
extern const float PIRANI_VOLTAGE_DIVIDER_R2;   // Spannungsteiler R2 = 100 kOhm

extern const float NTC_REF_VOLTAGE_DIVIDER_RESISTANCE;
extern const float NTC_REF_VOLTAGE;
extern const float NTC_OFFSET_VOLTAGE_CONV_FACTOR;
extern const float NTC_SERIES_RESISTANCE;
extern const float NTC_TEMP_COEFF_A;
extern const float NTC_TEMP_COEFF_B;
extern const float NTC_TEMP_COEFF_C;
extern const float NTC_TEMP_COEFF_D;

// --- ADC Filter ---
extern const float ADC_ALPHA;
extern const int ADC_READS_PER_SAMPLE;

// --- Frequenz-/Schichtdicken-Konstanten ---
extern const float SCHICHTDICKE_OFFSET_FREQ;
extern const float SCHICHTDICKE_CONV_FACTOR;

#endif // CONFIG_H