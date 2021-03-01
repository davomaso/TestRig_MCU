/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    stm32f4xx_it.c
 * @brief   Interrupt Service Routines.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "interogate_project.h"
#include "UART_Routine.h"
#include "Debounce.h"
#include "Asynchronous_pulse.h"
#include "Programming.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern PCD_HandleTypeDef hpcd_USB_OTG_FS;
extern DMA_HandleTypeDef hdma_adc1;
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim7;
extern TIM_HandleTypeDef htim10;
extern TIM_HandleTypeDef htim14;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart6;
/* USER CODE BEGIN EV */
extern ADC_HandleTypeDef hadc1;
/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles TIM1 update interrupt and TIM10 global interrupt.
  */
void TIM1_UP_TIM10_IRQHandler(void)
{
  /* USER CODE BEGIN TIM1_UP_TIM10_IRQn 0 */
	if (LatchSampling)
	{
		if (LatchCountTimer < latchCountTo) {
				HAL_ADC_Start(&hadc1);
					//ADC 1
				HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
				adc1.raw_Buffer[raw_adcCount] = HAL_ADC_GetValue(&hadc1);
					//ADC 2
				HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
				adc2.raw_Buffer[raw_adcCount] = HAL_ADC_GetValue(&hadc1);
					//Vfuse
				HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
				Vfuse.raw_Buffer[raw_adcCount] = HAL_ADC_GetValue(&hadc1);
					//Vin
				HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
				Vin.raw_Buffer[raw_adcCount] = HAL_ADC_GetValue(&hadc1);

					//ADC1
				adc1.total += adc1.raw_Buffer[raw_adcCount];
					//ADC2
				adc2.total += adc2.raw_Buffer[raw_adcCount];
					//Fuse Voltage
				Vfuse.total += Vfuse.raw_Buffer[raw_adcCount];
				//Vin Voltage
				Vin.total += Vin.raw_Buffer[raw_adcCount];

				if(LatchCountTimer > 10)
				{
					Vfuse.lowVoltage = Vfuse.raw_Buffer[raw_adcCount] < Vfuse.lowVoltage ? Vfuse.raw_Buffer[raw_adcCount]:Vfuse.lowVoltage;
					Vin.lowVoltage = Vin.raw_Buffer[raw_adcCount] < Vin.lowVoltage ? Vin.raw_Buffer[raw_adcCount]:Vin.lowVoltage;
				}

			if(raw_adcCount == 10){

				adc1.average = adc1.total / raw_adcCount;
				adc1.avg_Buffer[LatchCountTimer] = adc1.average;

				adc2.average = adc2.total / raw_adcCount;
				adc2.avg_Buffer[LatchCountTimer] = adc2.average;


				Vfuse.average = Vfuse.total / raw_adcCount;
				Vfuse.avg_Buffer[LatchCountTimer] = Vfuse.average;

				Vin.average = Vin.total/raw_adcCount;
				Vin.avg_Buffer[LatchCountTimer] = Vin.average;


				if(stableVoltageCount)
				{
					if(Vfuse.average > 2750 && Vfuse.average > 0.85*Vin.average) stableVoltageCount--;
					else stableVoltageCount++;
					LatchSampling = stableVoltageCount > 50 ? false:true;
				}

				if(LatchCountTimer == 25 && !stableVoltageCount){
					for(int i = 0; i <= LatchCountTimer;i++)
					{
						Vfuse.steadyState += Vfuse.avg_Buffer[i];
						Vin.steadyState += Vin.avg_Buffer[i];
					}
					Vfuse.steadyState /= LatchCountTimer;
					Vfuse.steadyState *= (16.17/4096);

					Vin.steadyState /= LatchCountTimer;
					Vin.steadyState *= (16.17/4096);
				}
				if (LatchCountTimer > 25  && !stableVoltageCount) {
						//Latch On Test
						if (adc1.average > 2400) {
							adc1.HighPulseWidth++;
							adc1.highVoltage += adc1.average;
							if (adc1.HighPulseWidth > 5 && adc1.HighPulseWidth < 45) {
								Vmos1.avg_Buffer[LatchCountTimer] = Vfuse.avg_Buffer[LatchCountTimer] - adc1.avg_Buffer[LatchCountTimer];
								Vmos1.highVoltage = Vmos1.highVoltage < Vmos1.avg_Buffer[LatchCountTimer] ? Vmos1.avg_Buffer[LatchCountTimer] : Vmos1.highVoltage;
								LatchCurrent.highVoltage += adc1.average - adc2.average;
							PulseCountDown = (adc1.HighPulseWidth > 45 || adc2.LowPulseWidth > 45) ? 10 : PulseCountDown;
							}
						}
						if (adc2.average < 260) {
							adc2.LowPulseWidth++;
							adc2.lowVoltage += adc2.average;
							if (adc2.LowPulseWidth > 5 && adc2.LowPulseWidth < 45) {
								Vmos2.avg_Buffer[LatchCountTimer] = adc2.avg_Buffer[LatchCountTimer];
								Vmos2.lowVoltage = Vmos2.lowVoltage > Vmos2.avg_Buffer[LatchCountTimer] ? Vmos2.avg_Buffer[LatchCountTimer] : Vmos2.lowVoltage;
							}
						}
						//Latch Off Test
						if (adc1.average < 260) {
							adc1.LowPulseWidth++;
							adc1.lowVoltage += adc1.average;
							if (adc1.LowPulseWidth > 5 && adc1.LowPulseWidth < 45) {
								Vmos1.avg_Buffer[LatchCountTimer] = adc1.avg_Buffer[LatchCountTimer];
								Vmos1.lowVoltage = Vmos1.lowVoltage > Vmos1.avg_Buffer[LatchCountTimer] ? Vmos1.avg_Buffer[LatchCountTimer] : Vmos1.lowVoltage;
							}
						}
						if (adc2.average > 2400) {
							adc2.HighPulseWidth++;
							adc2.highVoltage += adc2.average;
							if (adc2.HighPulseWidth > 5 && adc2.HighPulseWidth < 45) {
								Vmos2.avg_Buffer[LatchCountTimer] = Vfuse.avg_Buffer[LatchCountTimer] - adc2.avg_Buffer[LatchCountTimer];
								Vmos2.highVoltage = Vmos2.highVoltage < Vmos2.avg_Buffer[LatchCountTimer] ? Vmos2.avg_Buffer[LatchCountTimer] : Vmos2.highVoltage;
								LatchCurrent.lowVoltage += adc2.average - adc1.average;
							}
							PulseCountDown = (adc2.HighPulseWidth > 45 || adc1.LowPulseWidth > 45) ? 10 : PulseCountDown;
						}
						PulseCountDown--;
						LatchOnComplete = (adc1.HighPulseWidth > 47 && adc2.LowPulseWidth > 47) ? !PulseCountDown : false;
						LatchOffComplete = (adc2.HighPulseWidth > 47 || adc1.LowPulseWidth > 47) ? !PulseCountDown : false;
					}
				adc1.total = 0;
				adc2.total = 0;
				Vfuse.total = 0;
				Vin.total = 0;
				raw_adcCount = 0;

				LatchCountTimer++;
			}
			raw_adcCount++;
		}
		else{
			LatchSampling = false;
		}
	}
  /* USER CODE END TIM1_UP_TIM10_IRQn 0 */
  HAL_TIM_IRQHandler(&htim10);
  /* USER CODE BEGIN TIM1_UP_TIM10_IRQn 1 */

  /* USER CODE END TIM1_UP_TIM10_IRQn 1 */
}

/**
  * @brief This function handles USART2 global interrupt.
  */
void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */
	//Receive Interrupt
	if (USART2->SR & USART_SR_RXNE) {
		//Overflow check for Receive position, if the size of buffer is larger than 254 reset to 0
		if (UART2_RecPos > (LRGBUFFER))
			UART2_RecPos = 0;

		//load the data from USART 2 data direction register into DATA variable
		unsigned char data = USART2->DR;
		//if data is equal to the header and Receive data is not active, set the flag and begin storing data.
		//what will be allowed to pass: B2 Receive data not Active!
		//								21 Receive data Active!
		if ((data == 0xB2 && !UART2_Recdata)
				|| (data == 0x21 && UART2_RecPos == 1)) {
			//reset the position of Receive position to 0, length of buffer 254
			if (data == 0xB2) {
				UART2_RecPos = 0;
				UART2_Length[0] = 0;
				UART2_Recdata = true;
			}
			UART2_Receive[UART2_RecPos++] = data;
		} else if (UART2_Recdata) {
			if (UART2_RecPos == 2) {
				UART2_Length[0] = data;
				UART2_Receive[UART2_RecPos++] = UART2_Length[0];
			} else
				UART2_Receive[UART2_RecPos++] = data;

			//while the position is less than the length copy data into the buffer
			//else set receive data to false. to stop flow of data into the buffer
			if (UART2_RecPos == (UART2_Length[0] + 0x03)) {
				//UART3_transmit(&UART2_Receive[0], UART2_RecPos);
				UART2_Recdata = false;
				UART2_ReceiveComplete = true;
				HAL_GPIO_WritePin(MUX_A0_GPIO_Port, MUX_A0_Pin, GPIO_PIN_RESET);
				USART2->CR1  &= ~(USART_CR1_RXNEIE);
			}
		} else
			//catch all statement for anything that makes it this far.
			UART2_Recdata = false;
	}
  /* USER CODE END USART2_IRQn 0 */
  HAL_UART_IRQHandler(&huart2);
  /* USER CODE BEGIN USART2_IRQn 1 */

	//Transmit Interrupt for UART2
	if (USART2->SR & USART_SR_TXE) {
		if (UART2_TXpos == UART2_TXend) {
			//disable interrupts
			if( (LoomConnected != (b935x || b937x)) && (UART2_TXend > 0) )
				USART2->CR1 |= (USART_CR1_TCIE);
			USART2->CR1 &= ~(USART_CR1_TXEIE);
			UART2_TXend = UART2_TXpos = 0;
		} else {
			USART2->DR = UART2_TXbuffer[UART2_TXpos++];
		}
	}
	if(USART2->SR & USART_SR_TC){
		USART2->CR1 |= (USART_CR1_RE);
		USART2->CR1 &= ~(USART_CR1_TCIE);
		 HAL_GPIO_WritePin(RS485_EN_GPIO_Port, RS485_EN_Pin, GPIO_PIN_RESET);
	}
	/* USER CODE END USART2_IRQn 0 */
	HAL_UART_IRQHandler(&huart2);
	/* USER CODE BEGIN USART2_IRQn 1 */

  /* USER CODE END USART2_IRQn 1 */
}

/**
  * @brief This function handles USART3 global interrupt.
  */
void USART3_IRQHandler(void)
{
  /* USER CODE BEGIN USART3_IRQn 0 */
	if (USART3->SR & USART_SR_RXNE) {
		uns_ch data;
		data = USART3->DR;
		if (data == 0x0D) {
			for (int i = 0;i < BarcodeCount;i++) {
				SerialNumber[i] = BarcodeBuffer[i];
			}
			BarcodeScanned = true;
		} else {
			BarcodeBuffer[BarcodeCount++] = data;
		}
	}
  /* USER CODE END USART3_IRQn 0 */
  HAL_UART_IRQHandler(&huart3);
  /* USER CODE BEGIN USART3_IRQn 1 */

  /* USER CODE END USART3_IRQn 1 */
}

/**
  * @brief This function handles TIM8 trigger and commutation interrupts and TIM14 global interrupt.
  */
void TIM8_TRG_COM_TIM14_IRQHandler(void)
{
  /* USER CODE BEGIN TIM8_TRG_COM_TIM14_IRQn 0 */

  /* USER CODE END TIM8_TRG_COM_TIM14_IRQn 0 */
  HAL_TIM_IRQHandler(&htim14);
  /* USER CODE BEGIN TIM8_TRG_COM_TIM14_IRQn 1 */
	//LED Toggle
  if(Async_Port1.Active)		//max pulse count == 20
    {
  	  if(Async_Port1.fcount > 0){
  		  if(HAL_GPIO_ReadPin(ASYNC1_GPIO_Port, ASYNC1_Pin)){
  			  if(!(--Async_Port1.fcount)){
  				  Async_Port1.scount = 20;
  				  Async_Port1.PulseState ^= 1;
  			  }
  			  HAL_GPIO_WritePin(ASYNC1_GPIO_Port, ASYNC1_Pin, GPIO_PIN_RESET);
  		  } else
  			  HAL_GPIO_WritePin(ASYNC1_GPIO_Port, ASYNC1_Pin, GPIO_PIN_SET);
  	  }else if(Async_Port1.PulseState){
  		  if(!(--Async_Port1.scount)){
  			  Async_Port1.fcount=5;
  			  HAL_GPIO_WritePin(ASYNC1_GPIO_Port, ASYNC1_Pin, GPIO_PIN_RESET);
  			  Async_Port1.Active =  --Async_Port1.PulseCount;
  		  }
  		  if(HAL_GPIO_ReadPin(ASYNC1_GPIO_Port, ASYNC1_Pin))
  			  HAL_GPIO_WritePin(ASYNC1_GPIO_Port, ASYNC1_Pin, GPIO_PIN_RESET);
  	  }else{
  		  if(!(--Async_Port1.scount)){
  			  Async_Port1.fcount = 5;
  			  HAL_GPIO_WritePin(ASYNC1_GPIO_Port, ASYNC1_Pin, GPIO_PIN_RESET);
  		  }
  		  else if(!HAL_GPIO_ReadPin(ASYNC1_GPIO_Port, ASYNC1_Pin)){
  			  HAL_GPIO_WritePin(ASYNC1_GPIO_Port, ASYNC1_Pin, GPIO_PIN_SET);
  		  }
  	  }
    }
  if(Async_Port2.Active)		//max pulse count == 20
  {
	  if(Async_Port2.fcount > 0){
		  if(HAL_GPIO_ReadPin(ASYNC2_GPIO_Port, ASYNC2_Pin)){
			  if(!(--Async_Port2.fcount)){
				  Async_Port2.scount = 20;
				  Async_Port2.PulseState ^= 1;
			  }
			  HAL_GPIO_WritePin(ASYNC2_GPIO_Port, ASYNC2_Pin, GPIO_PIN_RESET);
		  } else
			  HAL_GPIO_WritePin(ASYNC2_GPIO_Port, ASYNC2_Pin, GPIO_PIN_SET);
	  }else if(Async_Port2.PulseState){
		  if(!(--Async_Port2.scount)){
			  Async_Port2.fcount=5;
			  HAL_GPIO_WritePin(ASYNC2_GPIO_Port, ASYNC2_Pin, GPIO_PIN_RESET);
			  Async_Port2.Active =  --Async_Port2.PulseCount;
		  }
		  if(HAL_GPIO_ReadPin(ASYNC2_GPIO_Port, ASYNC2_Pin))
			  HAL_GPIO_WritePin(ASYNC2_GPIO_Port, ASYNC2_Pin, GPIO_PIN_RESET);
	  }else{
		  if(!(--Async_Port2.scount)){
			  Async_Port2.fcount = 5;
			  HAL_GPIO_WritePin(ASYNC2_GPIO_Port, ASYNC2_Pin, GPIO_PIN_RESET);
		  }
		  else if(!HAL_GPIO_ReadPin(ASYNC2_GPIO_Port, ASYNC2_Pin)){
			  HAL_GPIO_WritePin(ASYNC2_GPIO_Port, ASYNC2_Pin, GPIO_PIN_SET);
		  }
	  }
  }
  if(Async_Port3.Active)		//max pulse count == 20
    {
  	  if(Async_Port3.fcount > 0){
  		  if(HAL_GPIO_ReadPin(ASYNC3_GPIO_Port, ASYNC3_Pin)){
  			  if(!(--Async_Port3.fcount)){
  				  Async_Port3.scount = 20;
  				  Async_Port3.PulseState ^= 1;
  			  }
  			  HAL_GPIO_WritePin(ASYNC3_GPIO_Port, ASYNC3_Pin, GPIO_PIN_RESET);
  		  } else
  			  HAL_GPIO_WritePin(ASYNC3_GPIO_Port, ASYNC3_Pin, GPIO_PIN_SET);
  	  }else if(Async_Port1.PulseState){
  		  if(!(--Async_Port3.scount)){
  			  Async_Port3.fcount=5;
  			  HAL_GPIO_WritePin(ASYNC3_GPIO_Port, ASYNC3_Pin, GPIO_PIN_RESET);
  			  Async_Port3.Active =  --Async_Port3.PulseCount;
  		  }
  		  if(HAL_GPIO_ReadPin(ASYNC3_GPIO_Port, ASYNC3_Pin))
  			  HAL_GPIO_WritePin(ASYNC3_GPIO_Port, ASYNC3_Pin, GPIO_PIN_RESET);
  	  }else{
  		  if(!(--Async_Port3.scount)){
  			  Async_Port3.fcount = 5;
  			  HAL_GPIO_WritePin(ASYNC3_GPIO_Port, ASYNC3_Pin, GPIO_PIN_RESET);
  		  }
  		  else if(!HAL_GPIO_ReadPin(ASYNC3_GPIO_Port, ASYNC3_Pin)){
  			  HAL_GPIO_WritePin(ASYNC3_GPIO_Port, ASYNC3_Pin, GPIO_PIN_SET);
  		  }
  	  }
    }
  if(Async_Port4.Active)		//max pulse count == 20
  {
	  if(Async_Port4.fcount > 0){
		  if(HAL_GPIO_ReadPin(ASYNC4_GPIO_Port, ASYNC4_Pin)){
			  if(!(--Async_Port4.fcount)){
				  Async_Port4.scount = 20;
				  Async_Port4.PulseState ^= 1;
			  }
			  HAL_GPIO_WritePin(ASYNC4_GPIO_Port, ASYNC4_Pin, GPIO_PIN_RESET);
		  } else
			  HAL_GPIO_WritePin(ASYNC4_GPIO_Port, ASYNC4_Pin, GPIO_PIN_SET);
	  }else if(Async_Port4.PulseState){
		  if(!(--Async_Port4.scount)){
			  Async_Port4.fcount=5;
			  HAL_GPIO_WritePin(ASYNC4_GPIO_Port, ASYNC4_Pin, GPIO_PIN_RESET);
			  Async_Port4.Active =  --Async_Port4.PulseCount;
		  }
		  if(HAL_GPIO_ReadPin(ASYNC4_GPIO_Port, ASYNC2_Pin))
			  HAL_GPIO_WritePin(ASYNC4_GPIO_Port, ASYNC4_Pin, GPIO_PIN_RESET);
	  }else{
		  if(!(--Async_Port4.scount)){
			  Async_Port4.fcount = 5;
			  HAL_GPIO_WritePin(ASYNC4_GPIO_Port, ASYNC4_Pin, GPIO_PIN_RESET);
		  }
		  else if(!HAL_GPIO_ReadPin(ASYNC4_GPIO_Port, ASYNC4_Pin)){
			  HAL_GPIO_WritePin(ASYNC4_GPIO_Port, ASYNC4_Pin, GPIO_PIN_SET);
		  }
	  }
  }
  if(Async_Port5.Active)		//max pulse count == 20
    {
  	  if(Async_Port5.fcount > 0){
  		  if(HAL_GPIO_ReadPin(ASYNC5_GPIO_Port, ASYNC5_Pin)){
  			  if(!(--Async_Port5.fcount)){
  				  Async_Port5.scount = 20;
  				  Async_Port5.PulseState ^= 1;
  			  }
  			  HAL_GPIO_WritePin(ASYNC5_GPIO_Port, ASYNC5_Pin, GPIO_PIN_RESET);
  		  } else
  			  HAL_GPIO_WritePin(ASYNC5_GPIO_Port, ASYNC5_Pin, GPIO_PIN_SET);
  	  }else if(Async_Port5.PulseState){
  		  if(!(--Async_Port5.scount)){
  			  Async_Port5.fcount=5;
  			  HAL_GPIO_WritePin(ASYNC5_GPIO_Port, ASYNC5_Pin, GPIO_PIN_RESET);
  			  Async_Port5.Active =  --Async_Port5.PulseCount;
  		  }
  		  if(HAL_GPIO_ReadPin(ASYNC5_GPIO_Port, ASYNC5_Pin))
  			  HAL_GPIO_WritePin(ASYNC5_GPIO_Port, ASYNC5_Pin, GPIO_PIN_RESET);
  	  }else{
  		  if(!(--Async_Port5.scount)){
  			  Async_Port5.fcount = 5;
  			  HAL_GPIO_WritePin(ASYNC5_GPIO_Port, ASYNC5_Pin, GPIO_PIN_RESET);
  		  }
  		  else if(!HAL_GPIO_ReadPin(ASYNC5_GPIO_Port, ASYNC5_Pin)){
  			  HAL_GPIO_WritePin(ASYNC5_GPIO_Port, ASYNC5_Pin, GPIO_PIN_SET);
  		  }
  	  }
    }
  if(Async_Port6.Active)		//max pulse count == 20
  {
	  if(Async_Port6.fcount > 0){
		  if(HAL_GPIO_ReadPin(ASYNC6_GPIO_Port, ASYNC6_Pin)){
			  if(!(--Async_Port6.fcount)){
				  Async_Port6.scount = 20;
				  Async_Port6.PulseState ^= 1;
			  }
			  HAL_GPIO_WritePin(ASYNC6_GPIO_Port, ASYNC6_Pin, GPIO_PIN_RESET);
		  } else
			  HAL_GPIO_WritePin(ASYNC6_GPIO_Port, ASYNC6_Pin, GPIO_PIN_SET);
	  }else if(Async_Port6.PulseState){
		  if(!(--Async_Port6.scount)){
			  Async_Port6.fcount=5;
			  HAL_GPIO_WritePin(ASYNC6_GPIO_Port, ASYNC6_Pin, GPIO_PIN_RESET);
			  Async_Port6.Active =  --Async_Port6.PulseCount;
		  }
		  if(HAL_GPIO_ReadPin(ASYNC6_GPIO_Port, ASYNC6_Pin))
			  HAL_GPIO_WritePin(ASYNC6_GPIO_Port, ASYNC6_Pin, GPIO_PIN_RESET);
	  }else{
		  if(!(--Async_Port6.scount)){
			  Async_Port6.fcount = 5;
			  HAL_GPIO_WritePin(ASYNC6_GPIO_Port, ASYNC6_Pin, GPIO_PIN_RESET);
		  }
		  else if(!HAL_GPIO_ReadPin(ASYNC6_GPIO_Port, ASYNC6_Pin)){
			  HAL_GPIO_WritePin(ASYNC6_GPIO_Port, ASYNC6_Pin, GPIO_PIN_SET);
		  }
	  }
  }
  if(Async_Port7.Active)		//max pulse count == 20
  {
	  if(Async_Port7.fcount > 0){
		  if(HAL_GPIO_ReadPin(ASYNC7_GPIO_Port, ASYNC7_Pin)){
			  if(!(--Async_Port7.fcount)){
				  Async_Port7.scount = 20;
				  Async_Port7.PulseState ^= 1;
			  }
			  HAL_GPIO_WritePin(ASYNC7_GPIO_Port, ASYNC7_Pin, GPIO_PIN_RESET);
		  } else
			  HAL_GPIO_WritePin(ASYNC7_GPIO_Port, ASYNC7_Pin, GPIO_PIN_SET);
	  }else if(Async_Port7.PulseState){
		  if(!(--Async_Port7.scount)){
			  Async_Port7.fcount=5;
			  HAL_GPIO_WritePin(ASYNC7_GPIO_Port, ASYNC7_Pin, GPIO_PIN_RESET);
			  Async_Port7.Active =  --Async_Port7.PulseCount;
		  }
		  if(HAL_GPIO_ReadPin(ASYNC7_GPIO_Port, ASYNC7_Pin))
			  HAL_GPIO_WritePin(ASYNC7_GPIO_Port, ASYNC7_Pin, GPIO_PIN_RESET);
	  }else{
		  if(!(--Async_Port7.scount)){
			  Async_Port7.fcount = 5;
			  HAL_GPIO_WritePin(ASYNC7_GPIO_Port, ASYNC7_Pin, GPIO_PIN_RESET);
		  }
		  else if(!HAL_GPIO_ReadPin(ASYNC7_GPIO_Port, ASYNC7_Pin)){
			  HAL_GPIO_WritePin(ASYNC7_GPIO_Port, ASYNC7_Pin, GPIO_PIN_SET);
		  }
	  }
  }
  if(Async_Port8.Active)		//max pulse count == 20
  {
	  if(Async_Port8.fcount > 0){
		  if(HAL_GPIO_ReadPin(ASYNC8_GPIO_Port, ASYNC8_Pin)){
			  if(!(--Async_Port8.fcount)){
				  Async_Port8.scount = 20;
				  Async_Port8.PulseState ^= 1;
			  }
			  HAL_GPIO_WritePin(ASYNC8_GPIO_Port, ASYNC8_Pin, GPIO_PIN_RESET);
		  } else
			  HAL_GPIO_WritePin(ASYNC8_GPIO_Port, ASYNC8_Pin, GPIO_PIN_SET);
	  }else if(Async_Port8.PulseState){
		  if(!(--Async_Port8.scount)){
			  Async_Port8.fcount=5;
			  HAL_GPIO_WritePin(ASYNC8_GPIO_Port, ASYNC8_Pin, GPIO_PIN_RESET);
			  Async_Port8.Active =  --Async_Port8.PulseCount;
		  }
		  if(HAL_GPIO_ReadPin(ASYNC8_GPIO_Port, ASYNC8_Pin))
			  HAL_GPIO_WritePin(ASYNC8_GPIO_Port, ASYNC8_Pin, GPIO_PIN_RESET);
	  }else{
		  if(!(--Async_Port8.scount)){
			  Async_Port8.fcount = 5;
			  HAL_GPIO_WritePin(ASYNC8_GPIO_Port, ASYNC8_Pin, GPIO_PIN_RESET);
		  }
		  else if(!HAL_GPIO_ReadPin(ASYNC8_GPIO_Port, ASYNC8_Pin)){
			  HAL_GPIO_WritePin(ASYNC8_GPIO_Port, ASYNC8_Pin, GPIO_PIN_SET);
		  }
	  }
  }
  if(Async_Port9.Active)		//max pulse count == 20
  {
	  if(Async_Port9.fcount > 0){
		  if(HAL_GPIO_ReadPin(ASYNC9_GPIO_Port, ASYNC9_Pin)){
			  if(!(--Async_Port9.fcount)){
				  Async_Port9.scount = 20;
				  Async_Port9.PulseState ^= 1;
			  }
			  HAL_GPIO_WritePin(ASYNC9_GPIO_Port, ASYNC9_Pin, GPIO_PIN_RESET);
		  } else
			  HAL_GPIO_WritePin(ASYNC9_GPIO_Port, ASYNC9_Pin, GPIO_PIN_SET);
	  }else if(Async_Port9.PulseState){
		  if(!(--Async_Port9.scount)){
			  Async_Port9.fcount=5;
			  HAL_GPIO_WritePin(ASYNC9_GPIO_Port, ASYNC9_Pin, GPIO_PIN_RESET);
			  Async_Port9.Active =  --Async_Port9.PulseCount;
		  }
		  if(HAL_GPIO_ReadPin(ASYNC9_GPIO_Port, ASYNC9_Pin))
			  HAL_GPIO_WritePin(ASYNC9_GPIO_Port, ASYNC9_Pin, GPIO_PIN_RESET);
	  }else{
		  if(!(--Async_Port9.scount)){
			  Async_Port9.fcount = 5;
			  HAL_GPIO_WritePin(ASYNC9_GPIO_Port, ASYNC9_Pin, GPIO_PIN_RESET);
		  }
		  else if(!HAL_GPIO_ReadPin(ASYNC9_GPIO_Port, ASYNC9_Pin)){
			  HAL_GPIO_WritePin(ASYNC9_GPIO_Port, ASYNC9_Pin, GPIO_PIN_SET);
		  }
	  }
  }

  if(!Async_Port1.Active && !Async_Port2.Active && !Async_Port3.Active && !Async_Port4.Active && !Async_Port5.Active && !Async_Port6.Active && !Async_Port7.Active && !Async_Port8.Active && !Async_Port9.Active)//
	  AsyncComplete = true;

  /* USER CODE END TIM8_TRG_COM_TIM14_IRQn 1 */
}

/**
  * @brief This function handles TIM6 global interrupt.
  */
void TIM6_IRQHandler(void)
{
  /* USER CODE BEGIN TIM6_IRQn 0 */
  if (Programming) {
	  Programming = --Program_CountDown ? true : false;
	  if (Program_CountDown == 10) {
		  ComRep = 0x08;
		  communication_array(&ComRep,&Para[0], Paralen);
	  }
	}

  if(LoomChecking){
	  if(LoomCheckCount == 200){
		  CheckLoom = true;
		  LoomCheckCount = 0;
	  } else LoomCheckCount++;
  }
  /* USER CODE END TIM6_IRQn 0 */
  HAL_TIM_IRQHandler(&htim6);
  /* USER CODE BEGIN TIM6_IRQn 1 */
  if(samplesUploading)
  {
	  if(sampleCount == sampleTime)
	  {
		  samplesUploading = false;
		  sampleUploadComplete = true;
	  }	else
		  sampleCount++;

  }
  /* USER CODE END TIM6_IRQn 1 */
}

/**
  * @brief This function handles TIM7 global interrupt.
  */
void TIM7_IRQHandler(void)
{
  /* USER CODE BEGIN TIM7_IRQn 0 */

  /* USER CODE END TIM7_IRQn 0 */
  HAL_TIM_IRQHandler(&htim7);
  /* USER CODE BEGIN TIM7_IRQn 1 */
	HAL_GPIO_WritePin(KP_R1_GPIO_Port, KP_R1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(KP_R2_GPIO_Port, KP_R2_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(KP_R3_GPIO_Port, KP_R3_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(KP_R4_GPIO_Port, KP_R4_Pin, GPIO_PIN_SET);

	if( ! (HAL_GPIO_ReadPin(KP_C1_GPIO_Port, KP_C1_Pin))){
			KP_1.Pressed = ++KP_1.Count >= 250 ? true:false;
	} else if (KP_1.Count > 0){
			KP_1.Pressed = --KP_1.Count ? true:false;
	}
	if( ! (HAL_GPIO_ReadPin(KP_C2_GPIO_Port, KP_C2_Pin))){
			KP_2.Pressed = ++KP_2.Count >= 250 ? true:false;
	} else if (KP_2.Count > 0){
			KP_2.Pressed = --KP_2.Count ? true:false;
	}
	if( ! (HAL_GPIO_ReadPin(KP_C3_GPIO_Port, KP_C3_Pin))){
			KP_3.Pressed = ++KP_3.Count >= 250 ? true:false;
	} else if (KP_3.Count > 0){
			KP_3.Pressed = --KP_3.Count ? true:false;
	}
		//Check ROW Two
	HAL_GPIO_WritePin(KP_R1_GPIO_Port, KP_R1_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(KP_R2_GPIO_Port, KP_R2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(KP_R3_GPIO_Port, KP_R3_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(KP_R4_GPIO_Port, KP_R4_Pin, GPIO_PIN_SET);

	if( ! (HAL_GPIO_ReadPin(KP_C1_GPIO_Port, KP_C1_Pin))){
			KP_4.Pressed = ++KP_4.Count >= 250 ? true:false;
	} else if (KP_4.Count > 0){
			KP_4.Pressed = --KP_4.Count ? true:false;
	}
	if( ! (HAL_GPIO_ReadPin(KP_C2_GPIO_Port, KP_C2_Pin))){
			KP_5.Pressed = ++KP_5.Count >= 250 ? true:false;
	} else if (KP_5.Count > 0){
			KP_5.Pressed = --KP_5.Count ? true:false;
	}
	if( ! (HAL_GPIO_ReadPin(KP_C3_GPIO_Port, KP_C3_Pin))){
			KP_6.Pressed = ++KP_6.Count >= 250 ? true:false;
	} else if (KP_6.Count > 0){
			KP_6.Pressed = --KP_6.Count ? true:false;
	}
		//Check ROW Three
	HAL_GPIO_WritePin(KP_R1_GPIO_Port, KP_R1_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(KP_R2_GPIO_Port, KP_R2_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(KP_R3_GPIO_Port, KP_R3_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(KP_R4_GPIO_Port, KP_R4_Pin, GPIO_PIN_SET);

	if( ! (HAL_GPIO_ReadPin(KP_C1_GPIO_Port, KP_C1_Pin))){
			KP_7.Pressed = ++KP_7.Count >= 250 ? true:false;
	} else if (KP_7.Count > 0){
			KP_7.Pressed = --KP_7.Count ? true:false;
	}
	if( ! (HAL_GPIO_ReadPin(KP_C2_GPIO_Port, KP_C2_Pin))){
			KP_8.Pressed = ++KP_8.Count >= 250 ? true:false;
	} else if (KP_8.Count > 0){
			KP_8.Pressed = --KP_8.Count ? true:false;
	}
	if( ! (HAL_GPIO_ReadPin(KP_C3_GPIO_Port, KP_C3_Pin))){
			KP_9.Pressed = ++KP_9.Count >= 250 ? true:false;
	} else if (KP_9.Count > 0){
			KP_9.Pressed = --KP_9.Count ? true:false;
	}
		//Check ROW Four
	HAL_GPIO_WritePin(KP_R1_GPIO_Port, KP_R1_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(KP_R2_GPIO_Port, KP_R2_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(KP_R3_GPIO_Port, KP_R3_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(KP_R4_GPIO_Port, KP_R4_Pin, GPIO_PIN_RESET);

	if( ! (HAL_GPIO_ReadPin(KP_C1_GPIO_Port, KP_C1_Pin))){
			KP_star.Pressed = ++KP_star.Count >= 250 ? true:false;
	} else if (KP_star.Count > 0){
			KP_star.Pressed = --KP_star.Count ? true:false;
	}
	if( ! (HAL_GPIO_ReadPin(KP_C2_GPIO_Port, KP_C2_Pin))){

		KP_0.Pressed = ++KP_0.Count >= 250 ? true:false;
	} else if (KP_0.Count > 0){
			KP_0.Pressed = --KP_0.Count ? true:false;
	}
	if( ! (HAL_GPIO_ReadPin(KP_C3_GPIO_Port, KP_C3_Pin))){
			KP_hash.Pressed = ++KP_hash.Count >= 250 ? true:false;
	} else if (KP_hash.Count > 0){
			KP_hash.Pressed = --KP_hash.Count ? true:false;
	}


  /* USER CODE END TIM7_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream0 global interrupt.
  */
void DMA2_Stream0_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream0_IRQn 0 */

  /* USER CODE END DMA2_Stream0_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_adc1);
  /* USER CODE BEGIN DMA2_Stream0_IRQn 1 */

  /* USER CODE END DMA2_Stream0_IRQn 1 */
}

/**
  * @brief This function handles USB On The Go FS global interrupt.
  */
void OTG_FS_IRQHandler(void)
{
  /* USER CODE BEGIN OTG_FS_IRQn 0 */

  /* USER CODE END OTG_FS_IRQn 0 */
  HAL_PCD_IRQHandler(&hpcd_USB_OTG_FS);
  /* USER CODE BEGIN OTG_FS_IRQn 1 */

  /* USER CODE END OTG_FS_IRQn 1 */
}

/**
  * @brief This function handles USART6 global interrupt.
  */
void USART6_IRQHandler(void)
{
  /* USER CODE BEGIN USART6_IRQn 0 */
	uint16 UART6status = USART6->SR;
	unsigned char UART6data = (USART6->DR & 0X7F);
	if(UART6status & USART_ERROR_MASK){
		SDSstate = SDSundef;
	}
	else if (UART6status & USART_SR_RXNE) {
		if( (UART6data == '?')){
			SDSstate = SDSquery;
		} else if(SDSstate == SDSundef){
			if(UART6data == SDIAddress)
				SDSstate = SDSaddress;
			else
				SDSstate = SDSundef;
			}
		else if(SDSstate == SDSaddress){
			if (UART6data == 'C')
				SDSstate = SDSc;
			else if(UART6data == 'D')
				SDSstate = SDSdPending;
		}
		else if((UART6data == '0') && (SDSstate == SDSdPending))
			SDSstate = SDSd;
		if(UART6data == '!'){
			if((SDSstate == SDSquery))
				{
					USART6->CR1 &= ~(USART_CR1_RE);
						//Set Address
					if(SDI_Port1.Enabled){
						SDI_Port1.Enabled = false;
						SDIAddress = SDI_Port1.Address;
						SDIMeasurement = SDI_Port1.setValue;
					}else if(SDI_Port2.Enabled){
						SDI_Port2.Enabled = false;
						SDIAddress = SDI_Port2.Address;
						SDIMeasurement = SDI_Port2.setValue;
					}else if(SDI_Port3.Enabled){
						SDI_Port3.Enabled = false;
						SDIAddress = SDI_Port3.Address;
						SDIMeasurement = SDI_Port3.setValue;
					}else if(SDI_Port4.Enabled){
						SDI_Port4.Enabled = false;
						SDIAddress = SDI_Port4.Address;
						SDIMeasurement = SDI_Port4.setValue;
					}else if(SDI_Port5.Enabled){
						SDI_Port5.Enabled = false;
						SDIAddress = SDI_Port5.Address;
						SDIMeasurement = SDI_Port5.setValue;
					}else if(SDI_Port6.Enabled){
						SDI_Port6.Enabled = false;
						SDIAddress = SDI_Port6.Address;
						SDIMeasurement = SDI_Port6.setValue;
					}
					HAL_GPIO_WritePin(Buffer_OE_GPIO_Port, Buffer_OE_Pin, GPIO_PIN_RESET);
					sprintf(Buffer,"%d\x0D\x0A", (SDIAddress-0x30));
					HAL_UART_Transmit(&SDI_UART, &Buffer[0], strlen(Buffer), 10);
					HAL_GPIO_WritePin(Buffer_OE_GPIO_Port, Buffer_OE_Pin, GPIO_PIN_SET);
					USART6->CR1 |= (USART_CR1_RE);
					SDSstate = SDSundef;
				}
			else if((SDSstate == SDSc))
				{
					USART6->CR1 &= ~(USART_CR1_RE);
//					Return Time till sample
					uns_ch *SDIrqMeasurements = "00001";
					HAL_GPIO_WritePin(Buffer_OE_GPIO_Port, Buffer_OE_Pin,GPIO_PIN_RESET);
					sprintf(Buffer, "%d%s\x0D\x0A", (SDIAddress-0x30),SDIrqMeasurements);
					HAL_UART_Transmit(&SDI_UART, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);
					HAL_GPIO_WritePin(Buffer_OE_GPIO_Port, Buffer_OE_Pin,GPIO_PIN_SET);
					USART6->CR1 |= (USART_CR1_RE);
				}
			else if((SDSstate == SDSd))
				{
				USART6->CR1 &= ~(USART_CR1_RE);
//					Return Time till sample
				HAL_GPIO_WritePin(Buffer_OE_GPIO_Port, Buffer_OE_Pin,GPIO_PIN_RESET);
				sprintf(Buffer, "%d+%f\x0D\x0A", (SDIAddress-0x30), SDIMeasurement);
				HAL_UART_Transmit(&SDI_UART, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);
				HAL_GPIO_WritePin(Buffer_OE_GPIO_Port, Buffer_OE_Pin,GPIO_PIN_SET);
				USART6->CR1 |= (USART_CR1_RE);
				} else
					SDSstate = SDSundef;
				sampleTime = 15;
				sampleCount = 0;
			}
	}
  /* USER CODE END USART6_IRQn 0 */
  HAL_UART_IRQHandler(&huart6);
  /* USER CODE BEGIN USART6_IRQn 1 */

  /* USER CODE END USART6_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
