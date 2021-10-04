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
#include "Calibration.h"
#include "Global_Variables.h"
#include "UART_Routine.h"
#include "Programming.h"
#include "CRC16_Slow.h"
#include "Init.h"
#include "Communication.h"

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
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim7;
extern TIM_HandleTypeDef htim10;
extern TIM_HandleTypeDef htim11;
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
	HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
	printT((uns_ch*) "\n\n\nHard Fault Reset Device!!!!...\n");
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
	//500us timer for calculating ADC values for latching and power ports
	//Poll 5 times each ms to gain a higher accuracy of voltage
	if ((CurrentState == csCalibrating) && (ProcessState == psWaiting)) {
		/*
		 * Read the ADC to determine when the Port that the calibration is connected to falls to GND,
		 * When a ground is detected for 5ms switch to current putting 20mA on all ports to calibrate
		 * the current on all ports.
		 * In the event of a failure set the ProcessState to psFailed to halt process
		 */
		if (CalibratingTimer < CALIBRATION_TIMEOUT) {
			if (BoardConnected.BoardType == b401x)
				ADC_Ch5sel();
			else if (BoardConnected.BoardType == b402x)
				ADC_Ch3sel();
			else
				ADC_Ch0sel();

			HAL_ADC_Start(&hadc1);
			HAL_ADC_PollForConversion(&hadc1, 100);
			calibrateADCval.average = HAL_ADC_GetValue(&hadc1);
			HAL_ADC_Stop(&hadc1);
//			sprintf(debugTransmitBuffer, "%d", calibrateADCval.average);
//			printT(&debugTransmitBuffer);
			if (BoardConnected.BoardType == b401x || BoardConnected.BoardType == b402x) {
				if (((BoardConnected.BoardType == b401x)) && (calibrateADCval.average >= 3000)) {
					if (!(--CalibrationCountdown) && !READ_BIT(CalibrationStatusRegister, CALIBRATE_CURRENT_SET)) {
 						TargetBoardCalibration_Current(&BoardConnected);
					}
				} else if ((BoardConnected.BoardType == b402x) && calibrateADCval.average <= 500) {
					if (!(--CalibrationCountdown) && !READ_BIT(CalibrationStatusRegister, CALIBRATE_CURRENT_SET)) {
						TargetBoardCalibration_Current(&BoardConnected);
					}
				} else
					CalibrationCountdown = 50;
			} else if ((calibrateADCval.average <= 500)) {
				if (!(--CalibrationCountdown) && !READ_BIT(CalibrationStatusRegister, CALIBRATE_CURRENT_SET)) {
					TargetBoardCalibration_Current(&BoardConnected);
				}
			} else
				CalibrationCountdown = 10;
		}
	}

	if (READ_BIT(LatchTestStatusRegister, LATCH_SAMPLING)) {
		/*
		 * While the LatchSampling variable is true the system will complete the latching test process,
		 * if the systems fuse or input voltage is not stable it will fail the test.
		 *	While true, the ADC will poll every 200us averaging the results for 1ms then handle the data to determine,
		 *	pulse widths, high voltages, low voltages, fuse voltages and input voltages.
		 *	From this other factors can be determined such as current through the latch, and MOSFET voltages.
		 */
		//Poll ADC every 200us to take average across 1ms to get more accurate reading
		//ADC 1
		if (LatchCountTimer < 2048) {
			ADC_Ch0sel();
			HAL_ADC_Start(&hadc1);
			HAL_ADC_PollForConversion(&hadc1, 100);
			LatchPortA.currentValue = HAL_ADC_GetValue(&hadc1);
			HAL_ADC_Stop(&hadc1);
			//ADC 2
			ADC_Ch1sel();
			HAL_ADC_Start(&hadc1);
			HAL_ADC_PollForConversion(&hadc1, 100);
			LatchPortB.currentValue = HAL_ADC_GetValue(&hadc1);
			HAL_ADC_Stop(&hadc1);
//					//Vin
			ADC_Ch2sel();
			HAL_ADC_Start(&hadc1);
			HAL_ADC_PollForConversion(&hadc1, 100);
			Vin.currentValue = HAL_ADC_GetValue(&hadc1);
			HAL_ADC_Stop(&hadc1);
//					//Vfuse
			ADC_Ch3sel();
			HAL_ADC_Start(&hadc1);
			HAL_ADC_PollForConversion(&hadc1, 100);
			Vfuse.currentValue = HAL_ADC_GetValue(&hadc1);
			HAL_ADC_Stop(&hadc1);

			if (LatchCountTimer > 2048)
				LatchCountTimer = 2048;
			else {
				//Vin
				Vin.avg_Buffer[LatchCountTimer] = Vin.currentValue;
				//Vfuse
				Vfuse.avg_Buffer[LatchCountTimer] = Vfuse.currentValue;
				//Latch A
				LatchPortA.avg_Buffer[LatchCountTimer] = LatchPortA.currentValue;
				//Latch B
				LatchPortB.avg_Buffer[LatchCountTimer] = LatchPortB.currentValue;
			}

			if (LatchCountTimer > 100) {
				/*
				 *  Once voltage is stable find the lowest fuse and input voltage. If the calculated average voltage is less then the current minimum
				 *  set the minimum to the calculated average
				 *  */
				Vfuse.lowVoltage = Vfuse.currentValue < Vfuse.lowVoltage ? Vfuse.currentValue : Vfuse.lowVoltage;
				Vin.lowVoltage = Vin.currentValue < Vin.lowVoltage ? Vin.currentValue : Vin.lowVoltage;
			}

			if (stableVoltageCount) {
				/*
				 * Determine whether the input and fuse voltages are stable, if stableVoltageCount increments too high
				 * set LatchSampling to false so that the process is halted and the test fails
				 */
				if ((Vin.currentValue > 3000) && (Vfuse.currentValue > 0.75 * Vin.currentValue))
					stableVoltageCount--;
				else
					stableVoltageCount++;
				if (stableVoltageCount > 100)
					CLEAR_BIT(LatchTestStatusRegister, LATCH_SAMPLING);
				else if (stableVoltageCount == 0)
					SET_BIT(LatchTestStatusRegister, STABLE_INPUT_VOLTAGE);

			}

			if (LatchCountTimer == 100 && READ_BIT(LatchTestStatusRegister, STABLE_INPUT_VOLTAGE)) {
				/*
				 * Calculate the steady state/ average voltage of the fuse and input across the range determined
				 * if the fuse and input are determined to be stable
				 */
				Vfuse.total = Vin.total = 0;
				for (int i = 0; i <= LatchCountTimer; i++) {
					Vfuse.total += Vfuse.avg_Buffer[i];
					Vin.total += Vin.avg_Buffer[i];
				}
				Vfuse.average = Vfuse.total / LatchCountTimer;
				Vin.average = Vin.total / LatchCountTimer;
			} else if (LatchCountTimer > 100 && READ_BIT(LatchTestStatusRegister, STABLE_INPUT_VOLTAGE)) {
				/*
				 * If statement to determine the voltages of the latch and pulsewidths.
				 * If the pulse width is above 47ms the latchoff rountine can be run.
				 */

				//Latch On Test
				if (READ_BIT(LatchTestStatusRegister,
						LATCH_ON_SAMPLING) && !READ_BIT(LatchTestStatusRegister, LATCH_ON_COMPLETE)) {
					if (LatchPortA.currentValue > 2400) {
						LatchPortA.HighPulseWidth++;
						LatchPortA.highVoltage += LatchPortA.currentValue;
						if (LatchPortA.HighPulseWidth > 20 && LatchPortA.HighPulseWidth < 80) {
							MOSFETvoltageA.currentValue = Vfuse.currentValue - LatchPortA.currentValue;
							MOSFETvoltageA.total += MOSFETvoltageA.currentValue;
						}
						PulseCountDown =
								(LatchPortA.HighPulseWidth > 90 || LatchPortB.LowPulseWidth > 90) ? 10 : PulseCountDown;
					}
					if (LatchPortB.currentValue < 200) {
						LatchPortB.LowPulseWidth++;
						LatchPortB.lowVoltage += LatchPortB.currentValue;
						if (LatchPortB.LowPulseWidth > 20 && LatchPortB.LowPulseWidth < 80) {
							MOSFETvoltageB.currentValue = LatchPortB.currentValue;
							MOSFETvoltageB.total += MOSFETvoltageB.currentValue;
						}
					}
				}
				if (READ_BIT(LatchTestStatusRegister,
						LATCH_OFF_SAMPLING) && !READ_BIT(LatchTestStatusRegister, LATCH_OFF_COMPLETE)) {
					//Latch Off Test
					if (LatchPortA.currentValue < 200) {
						LatchPortA.LowPulseWidth++;
						LatchPortA.lowVoltage += LatchPortA.currentValue;
						if (LatchPortA.LowPulseWidth > 20 && LatchPortA.LowPulseWidth < 80) {
							MOSFETvoltageA.currentValue = LatchPortA.currentValue;
							MOSFETvoltageA.total += MOSFETvoltageA.currentValue;
						}
					}
					if (LatchPortB.currentValue > 2400) {
						LatchPortB.HighPulseWidth++;
						LatchPortB.highVoltage += LatchPortB.currentValue;
						if (LatchPortB.HighPulseWidth > 20 && LatchPortB.HighPulseWidth < 80) {
							MOSFETvoltageB.currentValue = Vfuse.currentValue - LatchPortB.currentValue;
							MOSFETvoltageB.total += MOSFETvoltageB.currentValue;
						}
						PulseCountDown =
								(LatchPortB.HighPulseWidth > 90 || LatchPortA.LowPulseWidth > 90) ? 10 : PulseCountDown;
					}
				}
				PulseCountDown--;

				//Set latch complete bits
				if (!PulseCountDown && ((LatchPortA.HighPulseWidth > 94) && (LatchPortB.LowPulseWidth > 94))) {	//|| (PulseCountDown == 0)
					SET_BIT(LatchTestStatusRegister, LATCH_ON_COMPLETE);
					LatchPortA.highVoltage /= 2;
					LatchPortB.lowVoltage /= 2;
					LatchPortA.HighPulseWidth /= 2;
					LatchPortB.LowPulseWidth /= 2;
					MOSFETvoltageA.highVoltage = MOSFETvoltageA.total / LatchPortA.HighPulseWidth;
					MOSFETvoltageB.lowVoltage = MOSFETvoltageB.total / LatchPortB.LowPulseWidth;
					MOSFETvoltageA.total = MOSFETvoltageB.total = 0;
				}
				if (!PulseCountDown && ((LatchPortB.HighPulseWidth > 94) || (LatchPortA.LowPulseWidth > 94))) {
					SET_BIT(LatchTestStatusRegister, LATCH_OFF_COMPLETE);
					LatchPortA.LowPulseWidth /= 2;
					LatchPortB.HighPulseWidth /= 2;
					LatchPortB.highVoltage /= 2;
					LatchPortA.lowVoltage /= 2;
					MOSFETvoltageB.highVoltage = MOSFETvoltageB.total / LatchPortB.HighPulseWidth;
					MOSFETvoltageA.lowVoltage = MOSFETvoltageA.total / LatchPortA.LowPulseWidth;
					MOSFETvoltageA.total = MOSFETvoltageB.total = 0;
				}

			}
			LatchCountTimer++;
		}
	} else
		CLEAR_BIT(LatchTestStatusRegister, LATCH_SAMPLING);
  /* USER CODE END TIM1_UP_TIM10_IRQn 0 */
  HAL_TIM_IRQHandler(&htim1);
  HAL_TIM_IRQHandler(&htim10);
  /* USER CODE BEGIN TIM1_UP_TIM10_IRQn 1 */

  /* USER CODE END TIM1_UP_TIM10_IRQn 1 */
}

/**
  * @brief This function handles TIM1 trigger and commutation interrupts and TIM11 global interrupt.
  */
void TIM1_TRG_COM_TIM11_IRQHandler(void)
{
  /* USER CODE BEGIN TIM1_TRG_COM_TIM11_IRQn 0 */
	// 1ms Interrupt timer
	if (!SolarChargerStable && SolarChargerTimer) {
		ADC_Ch2sel();
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, 10);
		Vin.currentValue = HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);
		if (Vin.currentValue >= 3200) {
			Vin.total += Vin.currentValue;
			SolarChargerCounter++;
			if (SolarChargerCounter > 1000) {
				SolarChargerStable = true;
				float tempVal = Vin.total / SolarChargerCounter;
				BoardConnected.VoltageBuffer[V_SOLAR] =  tempVal * (15.25 / 4096);
				Vin.total = SolarChargerCounter = 0;
			}
		}
		SolarChargerTimer--;
	}

	if (!InputVoltageStable && InputVoltageTimer) {
		ADC_Ch2sel();
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, 10);
		Vin.currentValue = HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);
		if (Vin.currentValue >= 3000) {
			Vin.total += Vin.currentValue;
			InputVoltageCounter++;
		}
		InputVoltageTimer--;
		if ((InputVoltageCounter > 500) || InputVoltageTimer == 0) {
			if (InputVoltageCounter > 500)
				InputVoltageStable = true;
			float tempVal = Vin.total / (float)InputVoltageCounter;
			BoardConnected.VoltageBuffer[V_INPUT] =  tempVal * (15.25 / 4096);
			Vin.total = InputVoltageCounter = 0;
		}
	}
	if (samplesUploading) {
		/*
		 * Timer that is set when samples begin uploading to determine when to send
		 * the 0x18 Command to fetch results
		 * Set the processState to complete so that the next step can begin
		 */
		if (sampleCount >= sampleTime) {
			samplesUploading = false;
			samplesUploaded = true;
		} else {
			sampleCount++;
		}
	}

	if (timeOutEn) {
		/*
		 * Timeout routine to be run throughout the program,
		 * if the timeout is complete the ProcessState is set
		 * to Failed as a response or event did not occur as expected
		 */
		if ((--timeOutCount) == 0) {
			ProcessState = psFailed;
		}
	}

	if (latchTimeOutEn) {
		/*
		 * Latch timeout to wait for the 0x27 response as this can occur any time following the transmission of a 0x26
		 */
		if (--latchTimeOutCount) {
			LatchTimeOut = false;
		}
	}

	if (RelayPort_Enabled) {
		ADC_Ch3sel();
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, 10);
		Vfuse.currentValue = HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);

		if (Vfuse.currentValue < 300)
			RelayCount++;
		else
			RelayCount--;
	}

  /* USER CODE END TIM1_TRG_COM_TIM11_IRQn 0 */
  HAL_TIM_IRQHandler(&htim1);
  HAL_TIM_IRQHandler(&htim11);
  /* USER CODE BEGIN TIM1_TRG_COM_TIM11_IRQn 1 */

  /* USER CODE END TIM1_TRG_COM_TIM11_IRQn 1 */
}

/**
  * @brief This function handles USART2 global interrupt.
  */
void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */
	if ((USART2->SR & USART_SR_RXNE) && (BoardCommsReceiveState == RxWaiting)) {
		/*
		 * Board Comms receive routine to handle the data from the RS485 or Radio
		 * The Routine should sort through the string of data determining whether the string
		 * is correct by first receiving either 0xB2 21 or 0xB2 0F, if this condition is met it will
		 * receive the specified amount of data determined by the length byte in the string.
		 * Once collected communication_response() will determine whether the CRC is correct and
		 * handle what the system does with the string and future processes.
		 */
		//Overflow check for Receive position, if the size of buffer is larger than 254 reset to 0
		if (UART2_RecPos > (LRGBUFFER))
			UART2_RecPos = 0;

		//load the data from USART 2 data direction register into DATA variable
		unsigned char data = USART2->DR;
		//if data is equal to the header and Receive data is not active, set the flag and begin storing data.
		//what will be allowed to pass: B2 Receive data not Active!
		//								21 Receive data Active!

		if ((data == 0xB2 && !UART2_Recdata) || (data == 0x21 && UART2_RecPos == 1)
				|| (data == 0x0F && UART2_RecPos == 1)) {
			//reset the position of Receive position to 0, length of buffer 254
			if (data == 0xB2) {
				UART2_RecPos = 0;
				UART2_Length = 0;
				UART2_Recdata = true;
				timeOutEn = false;
			}
			UART2_RXbuffer[UART2_RecPos++] = data;
		} else if (UART2_Recdata) {
			if (UART2_RecPos == 2) {
				UART2_Length = data;
				UART2_RXbuffer[UART2_RecPos++] = UART2_Length;
			} else
				UART2_RXbuffer[UART2_RecPos++] = data;

			//while the position is less than the length copy data into the buffer
			//else set receive data to false. to stop flow of data into the buffer
			if (UART2_RecPos == (UART2_Length + 3)) {
				UART2_Recdata = false;
				if (CRC_Check(&UART2_RXbuffer[0], UART2_RecPos)) {
					if (UART2_RXbuffer[1] == 0x0F) {
						Datalen = UART2_RecPos - 5;
						memcpy(&Data_Buffer[0], &UART2_RXbuffer[3], Datalen);
					} else {
						Datalen = UART2_RecPos - 16;
						memcpy(&Data_Buffer[0], &UART2_RXbuffer[14], (UART2_RecPos));
					}
					BoardCommsReceiveState = RxGOOD;
				} else {
					BoardCommsReceiveState = RxBAD;
					printT((uns_ch*) "CRC Error...\n");
				}
				USART2->CR1 &= ~(USART_CR1_RXNEIE);
			}
		} else
			UART2_Recdata = false;				//catch all statement for anything that makes it this far.
	}

	if ((USART2->SR & USART_SR_TXE) && UART2_TXcount) {
		/*
		 * Transmit Interrupt Routine
		 * Populate the UART transmit register with the data that is in the TXbuffer, if the end condition is met
		 * turn the interrupts off and continue as expected.
		 * Following the transmission of a string/interrupt turned off use settimeout() to determine whether communications are operating correctly
		 */

		if (UART2_TXpos == UART2_TXcount) {
			if (UART2_TXpos != 0)
				//disable interrupts
				if (UART2_TXcount > 0)
					USART2->CR1 |= (USART_CR1_TCIE);
			USART2->CR1 &= ~(USART_CR1_TXEIE);
			UART2_TXcount = UART2_TXpos = 0;
			BoardCommsReceiveState = RxWaiting;
			if ( CurrentState == csCalibrating )
				setTimeOut(4000);
			else
				setTimeOut(1500);

		} else {
			USART2->DR = UART2_TXbuffer[UART2_TXpos++];
		}
	}
	if (USART2->SR & USART_SR_TC) {
		USART2->CR1 |= (USART_CR1_RE);
		USART2->CR1 &= ~(USART_CR1_TCIE);
		HAL_GPIO_WritePin(RS485_EN_GPIO_Port, RS485_EN_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(Radio_EN_GPIO_Port, Radio_EN_Pin, GPIO_PIN_SET);
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
		/*
		 * Barcode Scanner Receive Interrupt
		 * Interrupt rountine to receive data from the barcode scanner.
		 * Check the data for a carridge return while false and data received,
		 * populate the barcodeBuffer to be handled by readSerial()
		 */
		uns_ch data;
		data = USART3->DR;
		if (((data >= 0x30) && (data <= 0x39)) || (data == 0x0D)) {
			if ((data == 0x0D) && (BarcodeCount > 0)) {
				BarcodeScanned = true;
				USART3->CR1 &= ~(USART_CR1_RXNEIE);
			} else {
				if (BarcodeCount >= 0x09) {	//Check for overrun of scan
					memmove(&BarcodeBuffer[0], &BarcodeBuffer[1], 8);
					BarcodeCount--;
				}
				BarcodeBuffer[BarcodeCount++] = data;

			}
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

	/*
	 * Interrupt Timer for Asynchronous Pulses
	 * This timer is required to send asynchronous pulses at varying lengths. 2 Counters are implemented
	 * to transmit a variety of fast and long pulses to determine whether the filtration is operating correctly
	 * on the digital ports of the target board.
	 */
	//LED Toggle
	for (uint8 i = Port_1; i <= Port_9; i++) {
		if (Port[i].Async.Active) { //max pulse count == 20
			if (Port[i].Async.fcount > 0) {
				if (HAL_GPIO_ReadPin(Port[i].Async.Port, Port[i].Async.Pin)) {
					if (!(--Port[i].Async.fcount)) {
						Port[i].Async.scount = 20;
						Port[i].Async.PulseState ^= 1;
					}
					HAL_GPIO_WritePin(Port[i].Async.Port, Port[i].Async.Pin, GPIO_PIN_RESET);
				} else
					HAL_GPIO_WritePin(Port[i].Async.Port, Port[i].Async.Pin, GPIO_PIN_SET);
			} else if (Port[i].Async.PulseState) {
				if (!(--Port[i].Async.scount)) {
					Port[i].Async.fcount = 5;
					HAL_GPIO_WritePin(Port[i].Async.Port, Port[i].Async.Pin, GPIO_PIN_RESET);
					Port[i].Async.Active = --Port[i].Async.PulseCount;
				}
				if (HAL_GPIO_ReadPin(Port[i].Async.Port, Port[i].Async.Pin))
					HAL_GPIO_WritePin(Port[i].Async.Port, Port[i].Async.Pin, GPIO_PIN_RESET);
			} else {
				if (!(--Port[i].Async.scount)) {
					Port[i].Async.fcount = 5;
					HAL_GPIO_WritePin(Port[i].Async.Port, Port[i].Async.Pin, GPIO_PIN_RESET);
				} else if (!HAL_GPIO_ReadPin(Port[i].Async.Port, Port[i].Async.Pin)) {
					HAL_GPIO_WritePin(Port[i].Async.Port, Port[i].Async.Pin, GPIO_PIN_SET);
				}
			}
		}
	}
	if (!Port[Port_1].Async.Active && !Port[Port_2].Async.Active && !Port[Port_3].Async.Active
			&& !Port[Port_4].Async.Active && !Port[Port_5].Async.Active && !Port[Port_6].Async.Active
			&& !Port[Port_7].Async.Active && !Port[Port_8].Async.Active && !Port[Port_9].Async.Active) //
		AsyncComplete = true;
  /* USER CODE END TIM8_TRG_COM_TIM14_IRQn 1 */
}

/**
  * @brief This function handles TIM6 global interrupt.
  */
void TIM6_IRQHandler(void)
{
  /* USER CODE BEGIN TIM6_IRQn 0 */
	/*
	 * Loom Select Timer
	 * Timer every 2s check the loom to determine whether a different loom has been
	 * connected to test a different variant of boards
	 */
	if (LEDcounter++ > 110) {
		HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
		LED1active = true;
		LEDcounter = 0;
	} else if (LED1active && (LEDcounter > 10)) {
		HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
		LED1active = false;
	}
	if (LoomChecking) {
		if (LoomCheckCount >= 200) {
			CheckLoom = true;
			LoomCheckCount = 0;
		} else
			LoomCheckCount++;
	}
  /* USER CODE END TIM6_IRQn 0 */
  HAL_TIM_IRQHandler(&htim6);
  /* USER CODE BEGIN TIM6_IRQn 1 */

  /* USER CODE END TIM6_IRQn 1 */
}

/**
  * @brief This function handles TIM7 global interrupt.
  */
void TIM7_IRQHandler(void)
{
  /* USER CODE BEGIN TIM7_IRQn 0 */
	/*
	 //       ======      KEYPAD Scan Interupt      ======      //
	 */

	// 1ms timer to scan keypad
  /* USER CODE END TIM7_IRQn 0 */
  HAL_TIM_IRQHandler(&htim7);
  /* USER CODE BEGIN TIM7_IRQn 1 */
	/*
	 * Timer to check which key presses have been made. A simple matrix scan functionality is run, with counters
	 * incremented to eliminate switch bounce and be assured the user actually pressed the button
	 */

	for (uint8 currentScan = 0; currentScan < 12; currentScan++) {
		HAL_GPIO_WritePin(KP_R1_GPIO_Port, KP_R1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(KP_R2_GPIO_Port, KP_R2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(KP_R3_GPIO_Port, KP_R3_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(KP_R4_GPIO_Port, KP_R4_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(KP[currentScan].RowPort, KP[currentScan].RowPin, GPIO_PIN_RESET);

		KP[currentScan].PreviousState = KP[currentScan].State;
		if (HAL_GPIO_ReadPin(KP[currentScan].ColPort, KP[currentScan].ColPin))
			(KP[currentScan].debounceCount)--;
		else
			(KP[currentScan].debounceCount)++;

		if ((KP[currentScan].debounceCount >= 50)) {
			KP[currentScan].debounceCount = 50;
			KP[currentScan].State = true;
		} else if ((KP[currentScan].debounceCount <= 5)) {
			KP[currentScan].debounceCount = 5;
			KP[currentScan].State = false;
		}
		if (KP[currentScan].State != KP[currentScan].PreviousState)
			KP[currentScan].Pressed = KP[currentScan].State;
	}
  /* USER CODE END TIM7_IRQn 1 */
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
	/*
	 * SDI-12 Interrupt Rountine
	 * If SDI-12 is enabled the USART6 Receive interrupt will be enabled.
	 * Until a '?' is received the system will be in an undefined state, once receive it will enter query
	 * The routine will respond with the addres
	 *
	 */
	if (SDIenabled) {
		uint16 UART6status = USART6->SR;
		unsigned char UART6data = (USART6->DR & 0X7F);
		if (UART6status & USART_ERROR_MASK) {
			SDSstate = SDSundef;
		} else if (UART6status & USART_SR_RXNE) {
			if (UART6data == '?') {
				SDSstate = SDSquery;
			} else if (SDSstate == SDSundef) {
				if (UART6data == (SDIAddress + 0x30))
					SDSstate = SDSaddress;
				else
					SDSstate = SDSundef;
			} else if (SDSstate == SDSaddress) {
				if (UART6data == 'C')
					SDSstate = SDSc;
				else if (UART6data == 'D')
					SDSstate = SDSdPending;
			} else if ((UART6data == '0') && (SDSstate == SDSdPending))
				SDSstate = SDSd;
			if (UART6data == '!') {
				if (SDSstate == SDSquery) {
					USART6->CR1 &= ~(USART_CR1_RE);
					//Set Address
					for (uint8 i = Port_1; i <= Port_6; i++) {
						if (Port[i].Sdi.Enabled) {
							Port[i].Sdi.Enabled = false;
							SDIAddress = Port[i].Sdi.Address;
							SDIMeasurement = Port[i].Sdi.setValue;
						}
					}
					HAL_GPIO_WritePin(Buffer_OE_GPIO_Port, Buffer_OE_Pin, GPIO_PIN_RESET);
					sprintf((char*) &debugTransmitBuffer[0], "%d\x0D\x0A", (SDIAddress));
					HAL_UART_Transmit(&SDI_UART, (uint8_t*) &debugTransmitBuffer[0],
							strlen((char*) debugTransmitBuffer), 10);
					HAL_GPIO_WritePin(Buffer_OE_GPIO_Port, Buffer_OE_Pin, GPIO_PIN_SET);
					USART6->CR1 |= (USART_CR1_RE);
					SDSstate = SDSundef;
				} else if (SDSstate == SDSc) {
					USART6->CR1 &= ~(USART_CR1_RE); //					Return Time till sample
					char *SDIrqMeasurements = malloc(4 * sizeof(char));
					sprintf(SDIrqMeasurements, "00001");
					HAL_GPIO_WritePin(Buffer_OE_GPIO_Port, Buffer_OE_Pin, GPIO_PIN_RESET);
					sprintf((char*) &debugTransmitBuffer[0], "%d%s\x0D\x0A", (SDIAddress), SDIrqMeasurements);
					HAL_UART_Transmit(&SDI_UART, (uint8_t*) &debugTransmitBuffer[0],
							strlen((char*) debugTransmitBuffer), HAL_MAX_DELAY);
					HAL_GPIO_WritePin(Buffer_OE_GPIO_Port, Buffer_OE_Pin, GPIO_PIN_SET);
					USART6->CR1 |= (USART_CR1_RE);
					free(SDIrqMeasurements);
				} else if (SDSstate == SDSd) {
					USART6->CR1 &= ~(USART_CR1_RE); //					Return Time till sample
					HAL_GPIO_WritePin(Buffer_OE_GPIO_Port, Buffer_OE_Pin, GPIO_PIN_RESET);
					sprintf((char*) &debugTransmitBuffer[0], "%d+%f\x0D\x0A", (SDIAddress), SDIMeasurement);
					HAL_UART_Transmit(&SDI_UART, (uint8_t*) &debugTransmitBuffer[0],
							strlen((char*) debugTransmitBuffer),
							HAL_MAX_DELAY);
					HAL_GPIO_WritePin(Buffer_OE_GPIO_Port, Buffer_OE_Pin, GPIO_PIN_SET);
					USART6->CR1 |= (USART_CR1_RE);

				} else
					SDSstate = SDSundef;
			}
		}
	}
	if (RS485enabled) {
		uint16 UART6status = USART6->SR;
		if (UART6status & USART_ERROR_MASK) {
			RSstate = RSundef;
		} else if (UART6status & USART_SR_RXNE) {
			uns_ch UART6data = (USART6->DR & 0X7F);
			if (UART6data == '\r') {
				if (RSstate == RSundef)
					RSstate = RSquery;
				else if (RSstate == RSM)
					RSstate = RSdPending;
			} else if (UART6data == '\n') {
				if (RSstate == RSquery)
					RSstate = RS9pending;
				else if (RSstate == RSdPending)
					RSstate = RSd;
			} else if (UART6data == '9') {
				if (RSstate == RS9pending)
					RSstate = RS9;
				else if (RSstate == RS9)
					RSstate = RSMpending;
			} else if (UART6data == 'M' && RSstate == RSMpending)
				RSstate = RSM;
		}
		if (RSstate == RSd) {
			//Put the RS485 data in here
			HAL_GPIO_WritePin(RS485_4011EN_GPIO_Port, RS485_4011EN_Pin, GPIO_PIN_SET);
			sprintf((char*) &RS485buffer[0], "1,1,%.3f,2,%.3f,3,%.3f,4,%.3f,5,%.3f,6,%.3f,7,%.3f,8,%.3f,9,%.3f\r\n", //
					RS_SENSOR_1, RS_SENSOR_2, RS_SENSOR_3, RS_SENSOR_4, RS_SENSOR_5, RS_SENSOR_6, RS_SENSOR_7,
					RS_SENSOR_8, RS_SENSOR_9);
			HAL_UART_Transmit(&SDI_UART, (uint8_t*) &RS485buffer[0], strlen((char*) &RS485buffer[0]), HAL_MAX_DELAY);
			HAL_GPIO_WritePin(RS485_4011EN_GPIO_Port, RS485_4011EN_Pin, GPIO_PIN_RESET);
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
