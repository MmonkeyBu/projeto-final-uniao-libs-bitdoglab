#ifndef MIC_H
#define MIC_H

#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/dma.h"

#define DB_OFFSET -15.0f  // Ajuste este valor baseado nos seus testes

extern float var_real;  // Declare as extern

// Pino e canal do microfone no ADC.
#define MIC_CHANNEL 2
#define MIC_PIN (26 + MIC_CHANNEL)

// Parâmetros e macros do ADC.
#define ADC_CLOCK_DIV 96.f
#define SAMPLES 300 // Número de amostras que serão feitas do ADC.
#define ADC_ADJUST(x) (x * 3.3f / (1 << 12u) - 1.65f) // Ajuste do valor do ADC para Volts.
#define ADC_MAX 3.3f
#define ADC_STEP (3.3f/5.f) // Intervalos de volume do microfone.

#define abs(x) ((x < 0) ? (-x) : (x))

// More realistic reference values
#define MIC_REF_VOLTAGE      0.001f    // Standard reference voltage
#define MIC_SENSITIVITY      0.02f     // Sensitivity in Volts/Pascal
#define REF_SOUND_PRESSURE   20e-6f    // Reference sound pressure (20 µPa)

// // mic.h (adicionar no final do arquivo)
// #define MIC_REF_VOLTAGE      3.3f    // Tensão de referência do ADC
// #define MIC_SENSITIVITY_DB   -46.0f  // Sensibilidade do microfone (ex.: -46dB)
// #define REF_PRESSURE_DB      94.0f   // Nível de referência para 0dB (94dB = 1 Pascal)

/**
 * Converte a tensão RMS do microfone para decibéis (dB SPL).
 * @param rms_voltage Tensão RMS ajustada (em Volts)
 * @return Nível de pressão sonora em dB
 */
float mic_rms_to_db(float rms_voltage);

/**
 * Inicializa o módulo de microfone, configurando ADC e DMA.
 * @return Canal DMA utilizado para leitura do microfone
 */
uint mic_init();

/**
 * Realiza as leituras do ADC e armazena os valores no buffer.
 * @param adc_buffer Buffer para armazenar as amostras
 * @param dma_channel Canal DMA a ser utilizado
 */
void mic_sample(uint16_t* adc_buffer, uint dma_channel);

/**
 * Calcula a potência média das leituras do ADC. (Valor RMS)
 * @param adc_buffer Buffer com as amostras do ADC
 * @return Valor RMS das amostras
 */
float mic_power(const uint16_t* adc_buffer);

/**
 * Calcula a intensidade do volume registrado no microfone, de 0 a 4, usando a tensão.
 * @param v Tensão medida
 * @return Valor de intensidade de 0 a 4
 */
uint8_t mic_get_intensity(float v);

#endif // MIC_H