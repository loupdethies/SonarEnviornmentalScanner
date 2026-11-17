/**
 ******************************************************************************
 * @file    sonar.c
 * @brief   Sonar-Messkern
 *
 * Ablauf Einzelmessung:
 *   1. TIM2 (TRGO @ SES_ADC_FS_HZ) laeuft, ADC1 wartet auf Trigger.
 *   2. ADC1 + DMA (normal mode, n_pairs * 2 Transfers) wird gestartet.
 *   3. TIM1 sendet Burst: One-Pulse-Mode, Repetition Counter = n_pulses-1.
 *   4. DMA-Complete-Interrupt -> Messung fertig, Frame wird vom Hauptloop
 *      per USB-CDC an den Host gesendet.
 *
 * Streaming: identisch, aber zirkulaeres DMA ueber den ganzen Puffer und
 * Half-/Complete-Callbacks melden abwechselnd die Pufferhaelften.
 ******************************************************************************
 */
#include "sonar.h"

sonar_t  g_sonar;
uint16_t g_adc_buf[2u * SES_MAX_SAMPLE_PAIRS];

void sonar_init(ADC_HandleTypeDef *hadc, TIM_HandleTypeDef *htim_tx,
                TIM_HandleTypeDef *htim_trig)
{
    g_sonar.hadc      = hadc;
    g_sonar.htim_tx   = htim_tx;
    g_sonar.htim_trig = htim_trig;
    g_sonar.state     = SONAR_IDLE;
    g_sonar.done      = false;
    g_sonar.stream_ready = 0u;

    /* ADC-Triggertimer dauerhaft laufen lassen (TRGO bei Update) */
    HAL_TIM_Base_Start(htim_trig);
}

HAL_StatusTypeDef sonar_tx_burst(uint16_t n_pulses)
{
    TIM_HandleTypeDef *ht = g_sonar.htim_tx;

    if ((n_pulses == 0u) || (n_pulses > 256u)) {
        return HAL_ERROR;
    }

    /* Vorherigen Burst sauber beenden (HAL-Channel-State freigeben) */
    (void)HAL_TIM_PWM_Stop(ht, TIM_CHANNEL_1);
    (void)HAL_TIMEx_PWMN_Stop(ht, TIM_CHANNEL_1);

    /* Repetition Counter: RCR = n-1 Wiederholungen im One-Pulse-Mode */
    __HAL_TIM_DISABLE(ht);
    ht->Instance->RCR = (uint32_t)(n_pulses - 1u);
    ht->Instance->EGR = TIM_EGR_UG;             /* RCR uebernehmen           */
    __HAL_TIM_SET_COUNTER(ht, 0u);

    if (HAL_TIM_PWM_Start(ht, TIM_CHANNEL_1) != HAL_OK) {
        return HAL_ERROR;
    }
    if (HAL_TIMEx_PWMN_Start(ht, TIM_CHANNEL_1) != HAL_OK) {
        return HAL_ERROR;
    }
    /* One-Pulse-Mode: Timer stoppt nach RCR+1 Perioden selbst */
    __HAL_TIM_ENABLE(ht);

    g_sonar.n_pulses = n_pulses;
    return HAL_OK;
}

HAL_StatusTypeDef sonar_measure_start(uint16_t n_pulses, uint32_t n_pairs)
{
    if (g_sonar.state != SONAR_IDLE) {
        return HAL_BUSY;
    }
    if ((n_pairs == 0u) || (n_pairs > SES_MAX_SAMPLE_PAIRS)) {
        return HAL_ERROR;
    }

    g_sonar.n_pairs = n_pairs;
    g_sonar.done    = false;
    g_sonar.state   = SONAR_MEASURING;

    /* DMA im Normal-Mode fuer exakt ein Erfassungsfenster */
    g_sonar.hadc->DMA_Handle->Init.Mode = DMA_NORMAL;
    (void)HAL_DMA_Init(g_sonar.hadc->DMA_Handle);

    if (HAL_ADC_Start_DMA(g_sonar.hadc, (uint32_t *)g_adc_buf,
                          n_pairs * SES_ADC_CHANNELS) != HAL_OK) {
        g_sonar.state = SONAR_IDLE;
        return HAL_ERROR;
    }

    return sonar_tx_burst(n_pulses);
}

bool sonar_poll_done(void)
{
    return g_sonar.done;
}

HAL_StatusTypeDef sonar_stream_start(void)
{
    if (g_sonar.state != SONAR_IDLE) {
        return HAL_BUSY;
    }
    g_sonar.state        = SONAR_STREAMING;
    g_sonar.stream_ready = 0u;

    g_sonar.hadc->DMA_Handle->Init.Mode = DMA_CIRCULAR;
    (void)HAL_DMA_Init(g_sonar.hadc->DMA_Handle);

    if (HAL_ADC_Start_DMA(g_sonar.hadc, (uint32_t *)g_adc_buf,
                          2u * SES_MAX_SAMPLE_PAIRS) != HAL_OK) {
        g_sonar.state = SONAR_IDLE;
        return HAL_ERROR;
    }
    return HAL_OK;
}

void sonar_stop(void)
{
    (void)HAL_ADC_Stop_DMA(g_sonar.hadc);
    HAL_TIM_PWM_Stop(g_sonar.htim_tx, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Stop(g_sonar.htim_tx, TIM_CHANNEL_1);
    g_sonar.state        = SONAR_IDLE;
    g_sonar.done         = false;
    g_sonar.stream_ready = 0u;
}

void sonar_on_adc_half(void)
{
    if (g_sonar.state == SONAR_STREAMING) {
        g_sonar.stream_ready |= 0x01u;
    }
}

void sonar_on_adc_complete(void)
{
    if (g_sonar.state == SONAR_MEASURING) {
        (void)HAL_ADC_Stop_DMA(g_sonar.hadc);
        g_sonar.done  = true;
        g_sonar.state = SONAR_IDLE;
    } else if (g_sonar.state == SONAR_STREAMING) {
        g_sonar.stream_ready |= 0x02u;
    }
}
