/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "main.h"
#include "crc.h"
#include "usart.h"
#include "gpio.h"
#include <string.h>
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);
void bootloader_handle_mem_write_cmd(uint8_t *pBuffer);
void bootloader_uart_read_data(void);
void bootloader_jump_to_user_app(void);
uint32_t combine_uint8(uint8_t arr[4]);
void erase_flash_sector(void);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define BlueButton_Pin GPIO_PIN_0
#define BlueButton_GPIO_Port GPIOA
#define greenLED_Pin GPIO_PIN_12
#define greenLED_GPIO_Port GPIOD
#define orangeLED_Pin GPIO_PIN_13
#define orangeLED_GPIO_Port GPIOD
#define redLED_Pin GPIO_PIN_14
#define redLED_GPIO_Port GPIOD
#define blueLED_Pin GPIO_PIN_15
#define blueLED_GPIO_Port GPIOD

#define FLASH_SECTOR_2_BASE_ADDRESS 0x08008000U
#define BL_RX_LEN 15

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
