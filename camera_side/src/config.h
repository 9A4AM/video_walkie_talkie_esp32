#ifndef CONFIG_H
#define CONFIG_H

#include <freertos/FreeRTOS.h>
#include <driver/i2s.h>
#include <driver/gpio.h>

// WiFi credentials
#define WIFI_SSID "WiFi-C5BF"
#define WIFI_PSWD "WiFi-C5BF"


#define PDM_CLK_FREQ (SAMPLE_RATE * 64)

// sample rate for the system
#define SAMPLE_RATE 16000

// are you using an I2S microphone - comment this if you want to use an analog mic and ADC input
#define USE_I2S_MIC_INPUT
#define I2S_MIC_PDM

// I2S Microphone Settings

// Which channel is the I2S microphone on? I2S_CHANNEL_FMT_ONLY_LEFT or I2S_CHANNEL_FMT_ONLY_RIGHT
// Generally they will default to LEFT - but you may need to attach the L/R pin to GND
#define I2S_MIC_CHANNEL I2S_CHANNEL_FMT_ONLY_LEFT
// #define I2S_MIC_CHANNEL I2S_CHANNEL_FMT_ONLY_RIGHT
#ifdef I2S_MIC_PDM
#define I2S_MIC_SERIAL_CLOCK I2S_PIN_NO_CHANGE// 42// GPIO_NUM_12 //scl
#define I2S_MIC_LEFT_RIGHT_CLOCK 42// I2S_PIN_NO_CHANGE// GPIO_NUM_13 //ws
#define I2S_MIC_SERIAL_DATA 41 // GPIO_NUM_15 //sd
#else
#define I2S_MIC_SERIAL_CLOCK GPIO_NUM_3//I2S_PIN_NO_CHANGE// 42// GPIO_NUM_12 //scl
#define I2S_MIC_LEFT_RIGHT_CLOCK GPIO_NUM_4 //ws
#define I2S_MIC_SERIAL_DATA GPIO_NUM_2 //sd
#endif
// Analog Microphone Settings - ADC1_CHANNEL_7 is GPIO35
#define ADC_MIC_CHANNEL ADC1_CHANNEL_7

// speaker settings
#define USE_I2S_SPEAKER_OUTPUT
#define I2S_SPEAKER_SERIAL_CLOCK  GPIO_NUM_18
#define I2S_SPEAKER_LEFT_RIGHT_CLOCK GPIO_NUM_19
#define I2S_SPEAKER_SERIAL_DATA GPIO_NUM_5
// Shutdown line if you have this wired up or -1 if you don't
#define I2S_SPEAKER_SD_PIN -1// GPIO_NUM_22

// transmit button
#define GPIO_TRANSMIT_BUTTON 5

// Which LED pin do you want to use? TinyPico LED or the builtin LED of a generic ESP32 board?
// Comment out this line to use the builtin LED of a generic ESP32 board
// #define USE_LED_GENERIC

// Which transport do you want to use? ESP_NOW or UDP?
// comment out this line to use UDP
#define USE_ESP_NOW

// On which wifi channel (1-11) should ESP-Now transmit? The default ESP-Now channel on ESP32 is channel 1
#define ESP_NOW_WIFI_CHANNEL 1

// In case all transport packets need a header (to avoid interference with other applications or walkie talkie sets), 
// specify TRANSPORT_HEADER_SIZE (the length in bytes of the header) in the next line, and define the transport header in config.cpp
#define TRANSPORT_HEADER_SIZE 0
extern uint8_t transport_header[TRANSPORT_HEADER_SIZE];


// i2s config for using the internal ADC
extern i2s_config_t i2s_adc_config;
// i2s config for reading from of I2S
extern i2s_config_t i2s_mic_Config;
// i2s microphone pins
extern i2s_pin_config_t i2s_mic_pins;
// i2s speaker pins
extern i2s_pin_config_t i2s_speaker_pins;

#endif