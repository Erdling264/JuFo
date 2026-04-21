#ifndef FREQUENCY_COUNTER_H
#define FREQUENCY_COUNTER_H

#include <Arduino.h>
#include "driver/ledc.h" // ESP32 LEDC
#include "driver/pcnt.h" // ESP32 PCNT
#include "soc/pcnt_struct.h"
#include "esp_timer.h" // Für esp_timer

class FrequencyCounter {
public:
    FrequencyCounter();
    void begin();
    float getFrequency();
    void reset();
    void setOscillatorFrequency(uint32_t freq_hz); // Optional, für Testzwecke

private:
    uint32_t _multPulses = 0;
    int16_t _pulses = 0;
    float _currentFrequency = 0;

    esp_timer_create_args_t _create_args;
    esp_timer_handle_t _timer_handle;
    portMUX_TYPE _timerMux = portMUX_INITIALIZER_UNLOCKED;

    // Statische ISR-Funktion, die auf die Instanz zugreifen kann
    static void IRAM_ATTR pcnt_intr_handler_wrapper(void *arg);
    static void IRAM_ATTR read_pcnt_wrapper(void *arg);

    void pcnt_intr_handler_internal();
    void read_pcnt_internal();
    void initPcnt();
    void initOscillator(uint32_t osc_freq);
};

#endif // FREQUENCY_COUNTER_H