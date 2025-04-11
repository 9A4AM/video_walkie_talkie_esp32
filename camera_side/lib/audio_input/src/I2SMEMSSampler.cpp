#include "I2SMEMSSampler.h"
#include <Arduino.h>
#include "soc/i2s_reg.h"
// pdm
//  #define MEMS_I2S_MICROPHONE

I2SMEMSSampler::I2SMEMSSampler(
    i2s_port_t i2s_port,
    i2s_pin_config_t &i2s_pins,
    i2s_config_t i2s_config,
    int raw_samples_size,
    bool fixSPH0645) : I2SSampler(i2s_port, i2s_config)
{
    m_i2sPins = i2s_pins;
    m_fixSPH0645 = fixSPH0645;
    m_raw_samples_size = raw_samples_size;
    int32_t m_bits_per_sample = m_i2s_config.bits_per_sample;
    if (m_bits_per_sample == I2S_BITS_PER_SAMPLE_16BIT) // pdm case
    {
        m_raw_samples_16 = (int16_t *)malloc(sizeof(int16_t) * raw_samples_size);
    }
    else 
    {
        m_raw_samples = (int32_t *)malloc(sizeof(int32_t) * raw_samples_size);
    }
    
}

I2SMEMSSampler::~I2SMEMSSampler()
{
    int32_t m_bits_per_sample = m_i2s_config.bits_per_sample;
    if (m_bits_per_sample == I2S_BITS_PER_SAMPLE_16BIT) // pdm case
    {
        free(m_raw_samples_16);
    }
    else
    {
        free(m_raw_samples);
    }
   
}

void I2SMEMSSampler::configureI2S()
{
    if (m_fixSPH0645)
    {
        // FIXES for SPH0645
#if CONFIG_IDF_TARGET_ESP32
        REG_SET_BIT(I2S_TIMING_REG(m_i2sPort), BIT(9));
        REG_SET_BIT(I2S_CONF_REG(m_i2sPort), I2S_RX_MSB_SHIFT);
#endif
    }

    i2s_set_pin(m_i2sPort, &m_i2sPins);
}

void printBinary(int32_t num)
{
    for (int i = 31; i >= 0; i--)
    {
        Serial.print((num >> i) & 1); // Extract each bit
        if (i % 8 == 0)
            Serial.print(" "); // Add spacing every 8 bits
    }
    Serial.println();
}

int I2SMEMSSampler::read(int16_t *samples, int count)
{
    // read from i2s
    size_t bytes_read = 0;
    int32_t m_bits_per_sample = m_i2s_config.bits_per_sample;
    if (count > m_raw_samples_size)
    {
        count = m_raw_samples_size; // Buffer is too small
    }
    // i2s_read(m_i2sPort, m_raw_samples, sizeof(int32_t) * count, &bytes_read, portMAX_DELAY);
    int samples_read; // = bytes_read / sizeof(int32_t);

    // Process samples based on configured bit depth
    if (m_bits_per_sample == I2S_BITS_PER_SAMPLE_32BIT) // INMP441 case
    {
        i2s_read(m_i2sPort, m_raw_samples, sizeof(int32_t) * count, &bytes_read, portMAX_DELAY);
        samples_read = bytes_read / sizeof(int32_t);
        for (int i = 0; i < samples_read; i++)
        {
            int32_t temp = m_raw_samples[i] >> 12; // original code had 11, but wanted to turn the gain down
            samples[i] = (temp > INT16_MAX) ? INT16_MAX : (temp < -INT16_MAX) ? -INT16_MAX
                                                                              : (int16_t)temp;
        }
    }
    else if (m_bits_per_sample == I2S_BITS_PER_SAMPLE_16BIT) // Xiao mic case
    {
        i2s_read(m_i2sPort, m_raw_samples_16, sizeof(int16_t) * count, &bytes_read, portMAX_DELAY);
        samples_read = bytes_read / sizeof(int16_t);
        for (int i = 0; i < samples_read; i++)
        {
            int32_t temp = (m_raw_samples_16[i]) << 3; //>> 13; // 13; //original code had 11, but wanted to turn the gain down
            samples[i] = (temp > INT16_MAX) ? INT16_MAX : (temp < -INT16_MAX) ? -INT16_MAX
                                                                              : (int16_t)temp;
            // Serial.printf("Raw sample: %d | Binary: ", m_raw_samples_16[i]);
            // printBinary(m_raw_samples_16[i]);
            // Serial.printf("Shifted sample: %d | Binary: ", samples[i]);
            // printBinary(samples[i]);
        }
        // for (int i = 0; i < samples_read; i++) {
        //     Serial.printf("Raw sample: %10d | Hex: 0x%08X | Binary: ",
        //                   m_raw_samples[i], m_raw_samples[i]);

        //     // Print binary format
        //     for (int j = 31; j >= 0; j--) {
        //         Serial.print((m_raw_samples[i] >> j) & 1);
        //         if (j % 8 == 0) Serial.print(" "); // Add space every 8 bits
        //     }
        //     Serial.println();
        // }
        // int16_t *raw_samples_16 = (int16_t *)m_raw_samples;
        // // samples_read *= 2; // 16-bit samples take half the space of 32-bit
        // for (int i = 0; i < samples_read; i++)
        // {
        //     samples[i] = raw_samples_16[i];  // Directly store 16-bit values
        // }
    }
    return samples_read;
}

int I2SMEMSSampler::read_original(int16_t *samples, int count)
{
    // read from i2s
    size_t bytes_read = 0;
    if (count > m_raw_samples_size)
    {
        count = m_raw_samples_size; // Buffer is too small
    }
    i2s_read(m_i2sPort, m_raw_samples, sizeof(int32_t) * count, &bytes_read, portMAX_DELAY);
    int samples_read = bytes_read / sizeof(int32_t);
    for (int i = 0; i < samples_read; i++)
    {
        int32_t temp;
#ifdef MEMS_I2S_MICROPHONE
        temp = m_raw_samples[i] >> 12;
#else
        temp = m_raw_samples[i] >> 11;
#endif
        samples[i] = (temp > INT16_MAX) ? INT16_MAX : (temp < -INT16_MAX) ? -INT16_MAX
                                                                          : (int16_t)temp;
    }
    return samples_read;
}
