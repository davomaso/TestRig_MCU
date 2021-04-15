/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;
PCD_HandleTypeDef hpcd_USB_OTG_FS;
I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim7;
TIM_HandleTypeDef htim10;
TIM_HandleTypeDef htim11;
TIM_HandleTypeDef htim13;
TIM_HandleTypeDef htim14;

//Peripheral Defines
#define D_UART huart1	//Debug UART
#define R_UART huart2	//Radio UART
#define B_UART huart3	//Board Comms UART
#define SDI_UART huart6 //SDI-12/RS485 UART
#define USART_ERROR_MASK ((USART_SR_ORE_Msk) | (USART_SR_NE_Msk) | (USART_SR_FE_Msk) |(USART_SR_PE_Msk))

UART_HandleTypeDef D_UART;
UART_HandleTypeDef R_UART;
UART_HandleTypeDef B_UART;
UART_HandleTypeDef SDI_UART;

#define DAC_SPI hspi3	//DAC SPI
SPI_HandleTypeDef DAC_SPI;

#define EEPROM_W_ADDRESS 0x50 //EEPROM I2C Write Address
#define EEPROM_R_ADDRESS 0x51 //EEPROM I2C Read Address
#define LCD_ADR 0x3C	//LCD I2C Address

//==============	 CURRENT BOARD VERSIONS 	==============//
#define BOARD_935x 0x51
#define BOARD_937x 0x51
#define BOARD_401x 0x51
#define BOARD_402x 0x51
#define BOARD_422x 0x51
#define BOARD_427x 0x51

//===============	 OUTPUT FUNCTIONALITY 	===============//
#define TWO_WIRE_LATCHING 0x10	//One Pulse- Top 4 Bits, Two Wire Latching-Bottom 4 bits

//===============	 INPUT FUNCTIONALITY 	===============//
#define DIGITAL_OUTPUT 0x01
#define DIGITAL_INPUT 0x02
#define	ONE_VOLT 0x03
#define TWOFIVE_VOLT 0x04
#define THREE_VOLT 0x05
#define TWENTY_AMP 0x06
#define FIFTY_AMP 0x07
#define PULSE_WIDTH 0x08
#define ASYNC_PULSE 0x09
#define SDI_TWELVE 0x0A
#define ENVIROPRO 0x0B
#define AQUASPY 0x0C
#define NOTEST 0x00

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef unsigned char uns_ch;
typedef _Bool bool;
typedef signed char int8;
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef enum {bNone = 0, b935x = 0x9350, b937x = 0x9370, b401x = 0x4011,  b402x = 0x4020, b422x = 0x4220, b427x = 0x4270}TloomConnected;

typedef enum {Port_1 = 0, Port_2 = 1, Port_3 = 2, Port_4 = 3, Port_5 = 4, Port_6 = 5, Port_7 = 6, Port_8 = 7, Port_9 = 8}TcalPortConfig;
TcalPortConfig calPort;

typedef enum {csIDLE = 0, csInitialising = 1, csProgramming = 2, csCalibrating = 3, csInterogating = 4, csConfiguring = 5, csSampling = 6, csUploading = 7, csSortResults = 8, csSerialise = 9}TcurrentState;
TcurrentState CurrentState;

typedef enum {psFailed = 0, psWaiting = 1, psComplete = 2}TprocessState;
TprocessState ProcessState;

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
uns_ch debugTransmitBuffer[256];
uns_ch lcdBuffer[80];

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define V12fuseEN_Pin GPIO_PIN_2
#define V12fuseEN_GPIO_Port GPIOE
#define PASS_FAIL_Pin GPIO_PIN_15
#define PASS_FAIL_GPIO_Port GPIOC
#define KP_R1_Pin GPIO_PIN_0
#define KP_R1_GPIO_Port GPIOF
#define KP_R2_Pin GPIO_PIN_1
#define KP_R2_GPIO_Port GPIOF
#define KP_R3_Pin GPIO_PIN_2
#define KP_R3_GPIO_Port GPIOF
#define KP_R4_Pin GPIO_PIN_3
#define KP_R4_GPIO_Port GPIOF
#define KP_C1_Pin GPIO_PIN_4
#define KP_C1_GPIO_Port GPIOF
#define KP_C2_Pin GPIO_PIN_5
#define KP_C2_GPIO_Port GPIOF
#define KP_C3_Pin GPIO_PIN_6
#define KP_C3_GPIO_Port GPIOF
#define ADC_MUX_A_Pin GPIO_PIN_0
#define ADC_MUX_A_GPIO_Port GPIOC
#define ADC_MUX_B_Pin GPIO_PIN_1
#define ADC_MUX_B_GPIO_Port GPIOC
#define ADC_PortA_Pin GPIO_PIN_0
#define ADC_PortA_GPIO_Port GPIOA
#define ADC_PortB_Pin GPIO_PIN_1
#define ADC_PortB_GPIO_Port GPIOA
#define VinOut_Pin GPIO_PIN_2
#define VinOut_GPIO_Port GPIOA
#define VfuseOut_Pin GPIO_PIN_3
#define VfuseOut_GPIO_Port GPIOA
#define V12out_Pin GPIO_PIN_4
#define V12out_GPIO_Port GPIOA
#define VuOut_Pin GPIO_PIN_5
#define VuOut_GPIO_Port GPIOA
#define CurrentSense_Pin GPIO_PIN_7
#define CurrentSense_GPIO_Port GPIOA
#define LED1_Pin GPIO_PIN_0
#define LED1_GPIO_Port GPIOB
#define LED2_Pin GPIO_PIN_1
#define LED2_GPIO_Port GPIOB
#define MUX_WRodd1_Pin GPIO_PIN_14
#define MUX_WRodd1_GPIO_Port GPIOF
#define MUX_WReven1_Pin GPIO_PIN_15
#define MUX_WReven1_GPIO_Port GPIOF
#define MUX_WRodd2_Pin GPIO_PIN_0
#define MUX_WRodd2_GPIO_Port GPIOG
#define MUX_WReven2_Pin GPIO_PIN_1
#define MUX_WReven2_GPIO_Port GPIOG
#define MUX_WRodd3_Pin GPIO_PIN_7
#define MUX_WRodd3_GPIO_Port GPIOE
#define MUX_WReven3_Pin GPIO_PIN_8
#define MUX_WReven3_GPIO_Port GPIOE
#define MUX_A0_Pin GPIO_PIN_10
#define MUX_A0_GPIO_Port GPIOE
#define MUX_A1_Pin GPIO_PIN_11
#define MUX_A1_GPIO_Port GPIOE
#define MUX_RS_Pin GPIO_PIN_12
#define MUX_RS_GPIO_Port GPIOE
#define MUX_EN_Pin GPIO_PIN_13
#define MUX_EN_GPIO_Port GPIOE
#define DAC_CS3_Pin GPIO_PIN_12
#define DAC_CS3_GPIO_Port GPIOB
#define DAC_CS2_Pin GPIO_PIN_13
#define DAC_CS2_GPIO_Port GPIOB
#define DAC_CS1_Pin GPIO_PIN_14
#define DAC_CS1_GPIO_Port GPIOB
#define ASYNC1_Pin GPIO_PIN_14
#define ASYNC1_GPIO_Port GPIOD
#define ASYNC2_Pin GPIO_PIN_15
#define ASYNC2_GPIO_Port GPIOD
#define ASYNC3_Pin GPIO_PIN_2
#define ASYNC3_GPIO_Port GPIOG
#define ASYNC4_Pin GPIO_PIN_3
#define ASYNC4_GPIO_Port GPIOG
#define ASYNC5_Pin GPIO_PIN_4
#define ASYNC5_GPIO_Port GPIOG
#define ASYNC6_Pin GPIO_PIN_5
#define ASYNC6_GPIO_Port GPIOG
#define ASYNC7_Pin GPIO_PIN_6
#define ASYNC7_GPIO_Port GPIOG
#define ASYNC8_Pin GPIO_PIN_7
#define ASYNC8_GPIO_Port GPIOG
#define ASYNC9_Pin GPIO_PIN_8
#define ASYNC9_GPIO_Port GPIOG
#define Buffer_OE_Pin GPIO_PIN_9
#define Buffer_OE_GPIO_Port GPIOC
#define RS485_4011EN_Pin GPIO_PIN_8
#define RS485_4011EN_GPIO_Port GPIOA
#define TB_Reset_Pin GPIO_PIN_0
#define TB_Reset_GPIO_Port GPIOD
#define res_LoomSel_Pin GPIO_PIN_1
#define res_LoomSel_GPIO_Port GPIOD
#define Radio_EN_Pin GPIO_PIN_4
#define Radio_EN_GPIO_Port GPIOD
#define RS485_EN_Pin GPIO_PIN_7
#define RS485_EN_GPIO_Port GPIOD
#define V12fuseLatch_Pin GPIO_PIN_9
#define V12fuseLatch_GPIO_Port GPIOG
#define Loom_Sel_Pin GPIO_PIN_12
#define Loom_Sel_GPIO_Port GPIOG
#define SD_Det_Pin GPIO_PIN_1
#define SD_Det_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
