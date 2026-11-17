/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    main.h
 * @brief   Sonar Environmental Scanner - Hauptheader
 ******************************************************************************
 */
/* USER CODE END Header */

#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"

/* USER CODE BEGIN Includes */
#include "ses_config.h"
/* USER CODE END Includes */

void Error_Handler(void);
void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Private defines -----------------------------------------------------------*/
#define MIC_L_Pin GPIO_PIN_0
#define MIC_L_GPIO_Port GPIOA
#define MIC_R_Pin GPIO_PIN_1
#define MIC_R_GPIO_Port GPIOA
#define UART2_TX_Pin GPIO_PIN_2
#define UART2_TX_GPIO_Port GPIOA
#define UART2_RX_Pin GPIO_PIN_3
#define UART2_RX_GPIO_Port GPIOA
#define STATUS_LED_Pin GPIO_PIN_5
#define STATUS_LED_GPIO_Port GPIOA
#define PD_ALERT_Pin GPIO_PIN_6
#define PD_ALERT_GPIO_Port GPIOC
#define PD_ATTACH_Pin GPIO_PIN_7
#define PD_ATTACH_GPIO_Port GPIOC
#define BOOST_EN_Pin GPIO_PIN_8
#define BOOST_EN_GPIO_Port GPIOC
#define TX_PWM_Pin GPIO_PIN_8
#define TX_PWM_GPIO_Port GPIOA
#define TX_PWM_N_Pin GPIO_PIN_13
#define TX_PWM_N_GPIO_Port GPIOB

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
