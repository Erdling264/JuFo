#include "Config.h" // Inkludiere den eigenen Header, der die extern Deklarationen enthält

// --- WLAN-Konfiguration ---
const char* JUFOSSID = "Jugend-forscht";
const char* JUFOPASSWORD = "jufo2019";
const char* FRITZISSID = "Vacuum";
const char* FRITZIPASSWORD = "jufojufo";
const int WIFI_SELECT_PIN = 15;

// --- MQTT-Konfiguration ---
const char* MQTT_SERVER_JUFO = "192.168.205.205";
const char* MQTT_SERVER_FRITZI = "192.168.178.31";


const int MQTT_PORT = 1883;
const char *MQTT_USERNAME = "vacuum";
const char *MQTT_PASSWORD = "plasma";
const char *MQTT_CLIENT_ID_PREFIX = "esp32-client-";

// --- MQTT-Topics ---
const char *TOPIC_PIRANI = "vacuum/p_sense/pressure";
const char *TOPIC_FREQUENCY = "vacuum/f_sense/frequency";
const char *TOPIC_TEMPERATURE = "vacuum/t_sense/temperature";
const char *TOPIC_SCHICHTDICKE = "vacuum/s_sense/Schichdicke";
const char *TOPIC_DIFFERENZSCHICHTDICKE = "vacuum/ds_sense/DifferenzSchichtdicke";

// --- Frequenzzähler-Konfiguration ---
const uint32_t PCNT_OVERFLOW_VALUE = 20000;
const uint32_t SAMPLE_TIME_US = 1000000;

// --- Sensoren Pins ---
const int PIRANI_ANALOG_PIN = 36;
const int NTC_ANALOG_PIN = 33;

// --- Sensoren Kalibrierungs- und Berechnungskonstanten ---
const float PIRANI_PRESSURE_FACTOR_A = 1.5f;            // Berechnungskonstanten aus Sensordatenblatt: https://mmrc.caltech.edu/Vacuum/Edwards/EXPT%20Pumping%20Station/Edwards%20Wide%20Range%20Gauge.pdf
const float PIRANI_PRESSURE_FACTOR_B = 12.0f;
const float PIRANI_VOLTAGE_DIVIDER_R1 = 33000.0f;       // Spannungsteiler R1 = 33 kOhm
const float PIRANI_VOLTAGE_DIVIDER_R2 = 100000.0f;      // Spannungsteiler R2 = 100 kOhm

const float NTC_REF_VOLTAGE_DIVIDER_RESISTANCE = 2000.0f;
const float NTC_REF_VOLTAGE = 3.3f;
const float NTC_OFFSET_VOLTAGE_CONV_FACTOR = ((2.5f - 0.19f) / 2900.0f) + 0.167f;
const float NTC_SERIES_RESISTANCE = 25000.0f;
const float NTC_TEMP_COEFF_A = 0.00000001823149f;
const float NTC_TEMP_COEFF_B = -0.000112904828918f;
const float NTC_TEMP_COEFF_C = 0.340665710941973f;
const float NTC_TEMP_COEFF_D = -144.92934341647867f;

// --- ADC Filter ---
const float ADC_ALPHA = 0.05f;
const int ADC_READS_PER_SAMPLE = 100;

// --- Frequenz-/Schichtdicken-Konstanten ---
const float SCHICHTDICKE_OFFSET_FREQ = 100.0f;
const float SCHICHTDICKE_CONV_FACTOR = 1.84f;