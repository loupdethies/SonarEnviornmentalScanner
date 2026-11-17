/**
 ******************************************************************************
 * @file    sonar.h
 * @brief   Sonar-Messkern: 40-kHz-Burst (TIM1) + Echoerfassung (ADC1/DMA)
 ******************************************************************************
 */
#ifndef SONAR_H
#define SONAR_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "ses_config.h"

typedef enum {
    SONAR_IDLE = 0,
    SONAR_MEASURING,        /* Einzelmessung: Burst + Erfassungsfenster      */
    SONAR_STREAMING         /* kontinuierliche Audioerfassung (Tiererkennung)*/
} sonar_state_t;

typedef struct {
    ADC_HandleTypeDef  *hadc;      /* ADC1, DMA2_Stream0                     */
    TIM_HandleTypeDef  *htim_tx;   /* TIM1: Burst-PWM CH1/CH1N               */
    TIM_HandleTypeDef  *htim_trig; /* TIM2: TRGO als ADC-Trigger             */

    volatile sonar_state_t state;
    volatile bool       done;          /* Einzelmessung abgeschlossen        */
    volatile uint8_t    stream_ready;  /* Bit0: 1. Haelfte, Bit1: 2. Haelfte */
    uint32_t            n_pairs;       /* Samplepaare der laufenden Messung  */
    uint16_t            n_pulses;      /* Pulse des letzten Bursts           */
} sonar_t;

extern sonar_t g_sonar;

/* Erfassungspuffer: interleaved [L0, R0, L1, R1, ...]                        */
extern uint16_t g_adc_buf[2u * SES_MAX_SAMPLE_PAIRS];

void sonar_init(ADC_HandleTypeDef *hadc, TIM_HandleTypeDef *htim_tx,
                TIM_HandleTypeDef *htim_trig);

/* Einzelmessung starten: Burst senden, danach n_pairs Samplepaare erfassen.
 * Nicht blockierend; Abschluss via sonar_poll_done().                        */
HAL_StatusTypeDef sonar_measure_start(uint16_t n_pulses, uint32_t n_pairs);
bool              sonar_poll_done(void);

/* Kontinuierliches Streaming (zirkulaeres DMA, Halbpuffer-Meldungen)         */
HAL_StatusTypeDef sonar_stream_start(void);
void              sonar_stop(void);

/* Nur Burst senden (z. B. fuer Transducer-Test)                              */
HAL_StatusTypeDef sonar_tx_burst(uint16_t n_pulses);

/* Aus HAL-Callbacks aufzurufen                                               */
void sonar_on_adc_complete(void);
void sonar_on_adc_half(void);

#endif /* SONAR_H */
