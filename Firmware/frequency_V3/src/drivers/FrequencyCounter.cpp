#include "FrequencyCounter.h"
#include "../config/Config.h" // Für PCNT_xxx Konfiguration
#include "../utils/Utils.h" // Für ltos

// --- Statische Wrapper-Funktionen für ISRs ---
// Diese sind notwendig, da ISRs keine Member-Funktionen sein können
// und stattdessen einen Pointer auf die Instanz übergeben bekommen.
static FrequencyCounter* _instance = nullptr; // Globale Instanz, damit ISR sie finden kann

void IRAM_ATTR FrequencyCounter::pcnt_intr_handler_wrapper(void *arg) {
    if (_instance) _instance->pcnt_intr_handler_internal();
}

void IRAM_ATTR FrequencyCounter::read_pcnt_wrapper(void *arg) {
    if (_instance) _instance->read_pcnt_internal();
}

// --- Implementierung der Member-Funktionen ---
FrequencyCounter::FrequencyCounter() {
    _instance = this; // Setze die globale Instanz auf dieses Objekt
}

void FrequencyCounter::begin() {
    Serial.println("Initializing Frequency Counter...");
    initOscillator(4000000); // Standard-Oszillatorfrequenz setzen
    initPcnt();

    gpio_pad_select_gpio(OUTPUT_CONTROL_GPIO);
    gpio_set_direction(OUTPUT_CONTROL_GPIO, GPIO_MODE_OUTPUT);

    _create_args.callback = read_pcnt_wrapper;
    _create_args.name = "freq_read_timer";
    esp_timer_create(&_create_args, &_timer_handle);

    gpio_matrix_in(PCNT_INPUT_SIG_IO, SIG_IN_FUNC226_IDX, false);
    gpio_matrix_out(ONBOARD_LED, SIG_IN_FUNC226_IDX, false, false); // Für LED-Anzeige
    
    // Startet den Timer für die erste Messung
    esp_timer_start_once(_timer_handle, SAMPLE_TIME_US);
    gpio_set_level(OUTPUT_CONTROL_GPIO, 1);
}

void FrequencyCounter::initOscillator(uint32_t osc_freq) {
    uint32_t resolution = (log(80000000.0 / osc_freq) / log(2)) / 2;
    if (resolution < 1) resolution = 1;
    uint32_t mDuty = (pow(2, resolution)) / 2;

    ledc_timer_config_t ledc_timer = {};
    ledc_timer.duty_resolution = ledc_timer_bit_t(resolution);
    ledc_timer.freq_hz = osc_freq;
    ledc_timer.speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_timer.timer_num = LEDC_TIMER_0;
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel = {};
    ledc_channel.channel = LEDC_CHANNEL_0;
    ledc_channel.duty = mDuty;
    ledc_channel.intr_type = LEDC_INTR_DISABLE;
    ledc_channel.speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_channel.timer_sel = LEDC_TIMER_0;
    //ledc_channel.gpio_num = LEDC_HS_CH0_GPIO; // Wenn der Oszillator ausgegeben werden soll
    ledc_channel_config(&ledc_channel);
}

void FrequencyCounter::initPcnt() {
    pcnt_config_t pcnt_config = {};
    pcnt_config.pulse_gpio_num = PCNT_INPUT_SIG_IO;
    pcnt_config.ctrl_gpio_num = PCNT_INPUT_CTRL_IO;
    pcnt_config.unit = PCNT_COUNT_UNIT;
    pcnt_config.channel = PCNT_COUNT_CHANNEL;
    pcnt_config.counter_h_lim = PCNT_OVERFLOW_VALUE;
    pcnt_config.pos_mode = PCNT_COUNT_INC;
    pcnt_config.neg_mode = PCNT_COUNT_INC;
    pcnt_config.lctrl_mode = PCNT_MODE_DISABLE;
    pcnt_config.hctrl_mode = PCNT_MODE_KEEP;
    pcnt_unit_config(&pcnt_config);

    pcnt_counter_pause(PCNT_COUNT_UNIT);
    pcnt_counter_clear(PCNT_COUNT_UNIT);

    pcnt_event_enable(PCNT_COUNT_UNIT, PCNT_EVT_H_LIM);
    pcnt_isr_register(pcnt_intr_handler_wrapper, NULL, 0, NULL);
    pcnt_intr_enable(PCNT_COUNT_UNIT);

    pcnt_counter_resume(PCNT_COUNT_UNIT);
}

void IRAM_ATTR FrequencyCounter::pcnt_intr_handler_internal() {
    portENTER_CRITICAL_ISR(&_timerMux);
    _multPulses++;
    PCNT.int_clr.val = BIT(PCNT_COUNT_UNIT);
    portEXIT_CRITICAL_ISR(&_timerMux);
}

void IRAM_ATTR FrequencyCounter::read_pcnt_internal() {
    gpio_set_level(OUTPUT_CONTROL_GPIO, 0); // Stop counter
    pcnt_get_counter_value(PCNT_COUNT_UNIT, &_pulses);

    _currentFrequency = (_pulses + (_multPulses * PCNT_OVERFLOW_VALUE)) / 2.0f; // Calculation
    
    // Debug-Ausgabe in ISRs vermeiden, hier nur für Test
    // char buf[32];
    // printf("Freq (ISR): %s Hz \n", ltos(_currentFrequency, buf, 10)); 
}

float FrequencyCounter::getFrequency() {
    return _currentFrequency;
}

void FrequencyCounter::reset() {
    _multPulses = 0;
    _pulses = 0;
    pcnt_counter_clear(PCNT_COUNT_UNIT);
    esp_timer_start_once(_timer_handle, SAMPLE_TIME_US);
    gpio_set_level(OUTPUT_CONTROL_GPIO, 1);
}

void FrequencyCounter::setOscillatorFrequency(uint32_t freq_hz) {
    if (freq_hz != 0) {
        initOscillator(freq_hz);
        Serial.printf("Oscillator frequency set to %lu Hz\n", freq_hz);
    }
}