/**
 ******************************************************************************
 * @file    ses_config.h
 * @brief   Sonar Environmental Scanner - zentrale Konfiguration
 *
 * Hardware: SonarLandscapeScanner V1_1, STM32F411RET6 (LQFP64)
 * Pinbelegung entsprechend Schaltplan MCU_Unit.kicad_sch
 ******************************************************************************
 */
#ifndef SES_CONFIG_H
#define SES_CONFIG_H

/* ---------------------------- Firmware-Version --------------------------- */
#define SES_FW_VERSION_MAJOR    0
#define SES_FW_VERSION_MINOR    1
#define SES_FW_VERSION_PATCH    0

/* ------------------------------- Pinbelegung ----------------------------- */
/* ADC1 (Sensor Unit, INA333-Ausgaenge)                                       */
#define SES_MIC_L_PORT          GPIOA          /* PA0  ADC1_IN0  Sound_In_left  */
#define SES_MIC_L_PIN           GPIO_PIN_0
#define SES_MIC_R_PORT          GPIOA          /* PA1  ADC1_IN1  Sound_In_Right */
#define SES_MIC_R_PIN           GPIO_PIN_1

/* TIM1 Ultraschall-Sender (MA40S4S-Treiberstufe)                             */
#define SES_TX_PWM_PORT         GPIOA          /* PA8  TIM1_CH1   TX_PWM   */
#define SES_TX_PWM_PIN          GPIO_PIN_8
#define SES_TX_PWMN_PORT        GPIOB          /* PB13 TIM1_CH1N  TX_PWM_N */
#define SES_TX_PWMN_PIN         GPIO_PIN_13

/* Steuer-/Status-Pins                                                        */
#define SES_LED_PORT            GPIOA          /* PA5  Status-LED          */
#define SES_LED_PIN             GPIO_PIN_5
#define SES_BOOST_EN_PORT       GPIOC          /* PC8  Enable LT8330 Boost */
#define SES_BOOST_EN_PIN        GPIO_PIN_8
#define SES_PD_ALERT_PORT       GPIOC          /* PC6  STUSB4500 ALERT (od, low-aktiv) */
#define SES_PD_ALERT_PIN        GPIO_PIN_6
#define SES_PD_ATTACH_PORT      GPIOC          /* PC7  STUSB4500 ATTACH    */
#define SES_PD_ATTACH_PIN       GPIO_PIN_7

/* ------------------------------ Sonar-Parameter -------------------------- */
#define SES_TX_FREQ_HZ          40000u         /* MA40S4S Resonanzfrequenz  */
#define SES_TIM1_CLK_HZ         96000000u      /* APB2-Timertakt            */
#define SES_TX_DEFAULT_PULSES   8u             /* Pulse pro Burst (max 255) */
#define SES_TX_DEADTIME_NS      250u           /* Totzeit Halbbruecke       */

#define SES_ADC_FS_HZ           200000u        /* Abtastrate je Kanal (TIM2-Trigger) */
#define SES_ADC_CHANNELS        2u
#define SES_MAX_SAMPLE_PAIRS    8192u          /* 8192 Paare = 32 KiB Puffer */
#define SES_DEFAULT_SAMPLE_PAIRS 4096u         /* ~20,5 ms Echofenster ≙ ~3,5 m */

/* Audio-Streaming (Tiererkennung): halber Puffer je USB-Transfer            */
#define SES_STREAM_HALF_PAIRS   (SES_MAX_SAMPLE_PAIRS / 2u)

/* ------------------------------ USB-Protokoll ---------------------------- */
#define SES_FRAME_MAGIC0        0xA5u
#define SES_FRAME_MAGIC1        0x5Au

/* Host -> Geraet (1 Befehlsbyte + Little-Endian-Parameter)                   */
#define SES_CMD_VERSION         'V'            /* -                          */
#define SES_CMD_MEASURE         'M'            /* u16 nPulses, u32 nPairs    */
#define SES_CMD_STREAM_START    'A'            /* -                          */
#define SES_CMD_STOP            'S'            /* -                          */
#define SES_CMD_BOOST           'B'            /* u8 on/off                  */
#define SES_CMD_PDSTATUS        'G'            /* -                          */

/* Geraet -> Host Frame-Typen                                                 */
#define SES_RSP_VERSION         0x01u
#define SES_RSP_MEASUREMENT     0x02u
#define SES_RSP_STREAM_CHUNK    0x03u
#define SES_RSP_PDSTATUS        0x04u
#define SES_RSP_ERROR           0x7Fu

#endif /* SES_CONFIG_H */
