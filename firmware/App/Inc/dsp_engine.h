/**
  * ==============================================================================
  * @file           : dsp_engine.h
  * @brief          : Header for dsp_engine.c (acoustic signal processing)
  * @project        : OpenDVL
  * ==============================================================================
  */

#ifndef __DSP_ENGINE_H
#define __DSP_ENGINE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* Processing parameters */
#define NUM_BEAMS             4
#define FFT_SIZE              1024
#define SAMPLE_RATE_HZ        2000000.0f  // 2 MSPS
#define CARRIER_FREQ_HZ       300000.0f   // 300 kHz

/**
  * @brief Structure representing the acoustic echo raw and processed data
  */
typedef struct {
    float32_t beam_frequencies[NUM_BEAMS];  // Estimated Doppler peak frequencies (Hz)
    float32_t beam_velocities[NUM_BEAMS];   // Calculated velocities along each beam (m/s)
    uint8_t   beam_quality[NUM_BEAMS];      // Signal-to-noise ratio indicator (0-100)
} DVL_AcousticDataType;

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Initialize the DSP engine resources (FFT configurations, filters)
  * @retval None
  */
void DSP_Engine_Init(void);

/**
  * @brief  Process the raw ADC buffer to estimate the Doppler frequency shift on all beams
  * @param  adc_buffer: Pointer to the interleaved raw DMA ADC buffer
  * @param  buffer_size: Number of samples in the buffer
  * @param  output: Pointer to the results structure
  * @retval None
  */
void DSP_Process_Ping(uint16_t* adc_buffer, uint32_t buffer_size, DVL_AcousticDataType* output);

#ifdef __cplusplus
}
#endif

#endif /* __DSP_ENGINE_H */
