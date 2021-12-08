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
#include "ADC.h"
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
void NMI_Handler(void) {
	/* USER CODE BEGIN NonMaskableInt_IRQn 0 */

	/* USER CODE END NonMaskableInt_IRQn 0 */
	/* USER CODE BEGIN NonMaskableInt_IRQn 1 */

	/* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
 * @brief This function handles Hard fault interrupt.
 */
void HardFault_Handler(void) {
	/* USER CODE BEGIN HardFault_IRQn 0 */
	HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
	printT((uns_ch*) "\n\n\nHard Fault Reset Device!!!!...\n");
	/* USER CODE END HardFault_IRQn 0 */
	while (1) {
		/* USER CODE BEGIN W1_HardFault_IRQn 0 */
		/* USER CODE END W1_HardFault_IRQn 0 */
	}
}

/**
 * @brief This function handles Memory management fault.
 */
void MemManage_Handler(void) {
	/* USER CODE BEGIN MemoryManagement_IRQn 0 */

	/* USER CODE END MemoryManagement_IRQn 0 */
	while (1) {
		/* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
		/* USER CODE END W1_MemoryManagement_IRQn 0 */
	}
}

/**
 * @brief This function handles Pre-fetch fault, memory access fault.
 */
void BusFault_Handler(void) {
	/* USER CODE BEGIN BusFault_IRQn 0 */

	/* USER CODE END BusFault_IRQn 0 */
	while (1) {
		/* USER CODE BEGIN W1_BusFault_IRQn 0 */
		/* USER CODE END W1_BusFault_IRQn 0 */
	}
}

/**
 * @brief This function handles Undefined instruction or illegal state.
 */
void UsageFault_Handler(void) {
	/* USER CODE BEGIN UsageFault_IRQn 0 */

	/* USER CODE END UsageFault_IRQn 0 */
	while (1) {
		/* USER CODE BEGIN W1_UsageFault_IRQn 0 */
		/* USER CODE END W1_UsageFault_IRQn 0 */
	}
}

/**
 * @brief This function handles System service call via SWI instruction.
 */
void SVC_Handler(void) {
	/* USER CODE BEGIN SVCall_IRQn 0 */

	/* USER CODE END SVCall_IRQn 0 */
	/* USER CODE BEGIN SVCall_IRQn 1 */

	/* USER CODE END SVCall_IRQn 1 */
}

/**
 * @brief This function handles Debug monitor.
 */
void DebugMon_Handler(void) {
	/* USER CODE BEGIN DebugMonitor_IRQn 0 */

	/* USER CODE END DebugMonitor_IRQn 0 */
	/* USER CODE BEGIN DebugMonitor_IRQn 1 */

	/* USER CODE END DebugMonitor_IRQn 1 */
}

/**
 * @brief This function handles Pendable request for system service.
 */
void PendSV_Handler(void) {
	/* USER CODE BEGIN PendSV_IRQn 0 */

	/* USER CODE END PendSV_IRQn 0 */
	/* USER CODE BEGIN PendSV_IRQn 1 */

	/* USER CODE END PendSV_IRQn 1 */
}

/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler(void) {
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
void TIM1_UP_TIM10_IRQHandler(void) {
	/* USER CODE BEGIN TIM1_UP_TIM10_IRQn 0 */
	//500us timer for calculating ADC values for latching and power ports
	//Poll 5 times each ms to gain a higher accuracy of voltage
	/*
	 * Read the ADC to determine when the Port that the calibration is connected to falls to GND,
	 * When a ground is detected for 5ms switch to current putting 20mA on all ports to calibrate
	 * the current on all ports.
	 * In the event of a failure set the ProcessState to psFailed to halt process
	 */
	if ((CurrentState == csCalibrating) && (ProcessState == psWaiting)) {							// only check if system is in the calibration state
		if (CalibratingTimer < CALIBRATION_TIMEOUT) {												// Ensure the calibration is occuring during a valid timeframe
			if (BoardConnected.BoardType == b401x)													// Change channel depending on the board connected
				ADC_Ch5sel();
			else if (BoardConnected.BoardType == b402x)
				ADC_Ch3sel();
			else
				ADC_Ch0sel();

			HAL_ADC_Start(&hadc1);																	// Start ADC
			HAL_ADC_PollForConversion(&hadc1, 10);													// Poll ADC
			calibrateADCval.average = HAL_ADC_GetValue(&hadc1);										// Get sample
			HAL_ADC_Stop(&hadc1);																	// Stop ADC
//			sprintf(debugTransmitBuffer, "%d\n", calibrateADCval.average);
//			printT(&debugTransmitBuffer);
			if (BoardConnected.BoardType == b401x || BoardConnected.BoardType == b402x) {		// TODO: Change calibration references to defines opposed to hard codes
				if ( (BoardConnected.BoardType == b401x) && (calibrateADCval.average >= 3000) ) {	// 4010 only board with a high pulse to switch to current calibration
					if (!(--CalibrationCountdown) && !READ_BIT(CalibrationStatusRegister, CALIBRATE_CURRENT_SET)) {
						TargetBoardCalibration_Current(&BoardConnected);
					}
				} else if ((BoardConnected.BoardType == b402x) && calibrateADCval.average <= 500) {	// TODO: Change this to fit in with the conditional statement below
					if (!(--CalibrationCountdown) && !READ_BIT(CalibrationStatusRegister, CALIBRATE_CURRENT_SET)) {
						TargetBoardCalibration_Current(&BoardConnected);
					}
				} else
					CalibrationCountdown = 50;
			} else if ((calibrateADCval.average <= 500)) {											// See if the calibration port has been pulled to ground
				if ((CalibrationCountdown == 0) && !READ_BIT(CalibrationStatusRegister, CALIBRATE_CURRENT_SET)) {	// If calibration count is zero then set the current calibration
					TargetBoardCalibration_Current(&BoardConnected);
				}
				CalibrationCountdown--;																// decrement calibration countdown
			} else
				CalibrationCountdown = 5;															// Reset the calibration countdown if condition is not met
		}
	}

	/*
	 * While the LatchSampling variable is true the system will complete the latching test process,
	 * if the systems fuse or input voltage is not stable it will fail the test.
	 *	While true, the ADC will poll every 200us averaging the results for 1ms then handle the data to determine,
	 *	pulse widths, high voltages, low voltages, fuse voltages and input voltages.
	 *	From this other factors can be determined such as current through the latch, and MOSFET voltages.
	 */
	if (READ_BIT(LatchTestStatusRegister, LATCH_SAMPLING)) {
		if (!LatchADCflag) {			//Poll ADC every 500us to take average across 1ms to get more accurate reading
				// Latchport A
			ADC_Ch0sel();
			HAL_ADC_Start(&hadc1);
			HAL_ADC_PollForConversion(&hadc1, 100);
			LatchPortA.currentValue = HAL_ADC_GetValue(&hadc1);
			HAL_ADC_Stop(&hadc1);
				// Latchport B
			ADC_Ch1sel();
			HAL_ADC_Start(&hadc1);
			HAL_ADC_PollForConversion(&hadc1, 100);
			LatchPortB.currentValue = HAL_ADC_GetValue(&hadc1);
			HAL_ADC_Stop(&hadc1);
				// Vin
			ADC_Ch2sel();
			HAL_ADC_Start(&hadc1);
			HAL_ADC_PollForConversion(&hadc1, 100);
			Vin.currentValue = HAL_ADC_GetValue(&hadc1);
			HAL_ADC_Stop(&hadc1);
				// Vfuse
			ADC_Ch3sel();
			HAL_ADC_Start(&hadc1);
			HAL_ADC_PollForConversion(&hadc1, 100);
			Vfuse.currentValue = HAL_ADC_GetValue(&hadc1);
			HAL_ADC_Stop(&hadc1);
			LatchADCflag = true;			// Set flag to true once samples complete
		}
	}

	/* USER CODE END TIM1_UP_TIM10_IRQn 0 */
	HAL_TIM_IRQHandler(&htim1);
	HAL_TIM_IRQHandler(&htim10);
	/* USER CODE BEGIN TIM1_UP_TIM10_IRQn 1 */

	/* USER CODE END TIM1_UP_TIM10_IRQn 1 */
}

/**
 * @brief This function handles TIM1 trigger and commutation interrupts and TIM11 global interrupt.
 */
void TIM1_TRG_COM_TIM11_IRQHandler(void) {
	/* USER CODE BEGIN TIM1_TRG_COM_TIM11_IRQn 0 */
	// 1ms Interrupt timer
	if (!SolarChargerStable && SolarChargerTimer) {
		ADC_Ch2sel();													// select the Vin adc channel
		HAL_ADC_Start(&hadc1);											// Start ADC
		HAL_ADC_PollForConversion(&hadc1, 10);							// Poll ADC for conversion
		Vin.currentValue = HAL_ADC_GetValue(&hadc1);					// Get the ADC sample
		HAL_ADC_Stop(&hadc1);											// Stop the ADC
		if (Vin.currentValue >= 3200) {									// Check if it is above threshold 		// TODO: Set the Threshold as a define, remove hard code definition
			Vin.total += Vin.currentValue;								// TODO: Change to rolling average for voltage calculation
			SolarChargerCounter++;										// Increment counter
			if (SolarChargerCounter > 1000) {							// Check counter
				SolarChargerStable = true;								// Set stable flag to true
				float tempVal = Vin.total / SolarChargerCounter;		// get average value
				BoardConnected.VoltageBuffer[V_SOLAR] = tempVal * (15.25 / 4096);		// Pass the average to the voltage buffer
				Vin.total = SolarChargerCounter = 0;					// reinitialise the counter and total
			}
		}
		SolarChargerTimer--;
	}

	// Sample the input voltages
	if (InputVoltageSampling) {
		if (!InputVoltageStable && InputVoltageTimer) {					// Ensure the input voltage is not stable
			ADC_Ch2sel();												// Select the input voltage channel
			HAL_ADC_Start(&hadc1);										// Start the ADC
			HAL_ADC_PollForConversion(&hadc1, 100);						// Poll the ADC for the conversion
			Vin.currentValue = HAL_ADC_GetValue(&hadc1);				// Store the value in currentvalue
			HAL_ADC_Stop(&hadc1);										// Stop the ADC
			if (Vin.currentValue >= 3000) {								// If current value is larger than xxxx increase counter
//				Vin.average = (Vin.currentValue - Vin.average) / InputVoltageCounter++; // TODO: Change this to consistant average calculation
				Vin.total += Vin.average;
				InputVoltageCounter++;
			}
			InputVoltageTimer--;										// Decrease input voltage timer
			if ((InputVoltageCounter > 250) || InputVoltageTimer == 0) {
				if (InputVoltageCounter > 250)							// Input voltage counter greater than 250
					InputVoltageStable = true;
				float tempVal = Vin.total / (float) InputVoltageCounter;
				BoardConnected.VoltageBuffer[V_INPUT] = tempVal * (15.25 / 4096);
				Vin.total = InputVoltageCounter = 0;
			}
		}
	}

/*
 * Timer that is set when samples begin uploading to determine when to send
 * the 0x18 Command to fetch results
 * Set the processState to complete so that the next step can begin
 */
	if (samplesUploading) {										// Check to see if samples are uploading
		if (sampleCount >= sampleTime) {						// Ensure samples are occuring during a valid time frame
			samplesUploading = false;							// Set uploading to false to stop sampling
			samplesUploaded = true;								// Set samples uploaded to complete to move states
		} else {
			sampleCount++;
		}
	}

/*
 * Timeout routine to be run during communications to targetboard, if count reaches zero, set state to psFailed
 */
	if (timeOutEn) {											// Only proceed if time out is enabled
		if (timeOutCount) {
			if ((--timeOutCount) == 0) {						// if the countdown reaches 0 set the state to failed
				timeOutEn = false;
				ProcessState = psFailed;						// Board comms response was not received so move to failed state
			}
		}
	}

	/*
	 * General Timers for various board functionalities
	 */
// =========================================================== //
	if (BoardCommsTimeout)										// Timeout for board comms, wait for wake transmission to finish
		BoardCommsTimeout--;

	if (BoardResetTimer)										// Timer for board to reset
		BoardResetTimer--;

	if (ProgrammingTimeOut)										// Timeout for programming
		ProgrammingTimeOut--;

/*
 * Loom Select Timer
 * Timer every 250ms check the loom to determine whether a different loom has been
 */
	if (CheckLoomTimer)											// Timeout for loom scanning
		CheckLoomTimer--;
// =========================================================== //

/*
 * Timeout routine to be run throughout the program,
 * if the timeout is complete the ProcessState is set
 * to Failed as a response or event did not occur as expected
 */
	if (terminalTimeOutEn) {									// Timeout for the terminal, given CDC transmit does not fill a buffer but poll until it is ready, timeout created to determine whether comms are ready
		if (terminalTimeOutCount) {								// Check if timeoutcount still populated
			if ((--terminalTimeOutCount) == 0) {
				terminalTimeOutEn = false;						// Disable timeout
			}
		}
	}

/*
 * Latch timeout to wait for the 0x27 response as this can occur any time following the transmission of a 0x26
 */
	if (latchTimeOutEn) {
		if (latchTimeOutCount) {
			if (--latchTimeOutCount == 0) {
				latchTimeOutEn = false;				// Disable Latchtimeout & Latch timeout enable
				LatchTimeOut = false;
			}
		}
	}

	if (RelayPort_Enabled) {
		ADC_Ch3sel();											// Select the ch that the relay output is on
		HAL_ADC_Start(&hadc1);									// Start ADC
		HAL_ADC_PollForConversion(&hadc1, 10);					// Poll to get sample
		Vfuse.currentValue = HAL_ADC_GetValue(&hadc1);			// get sample
		HAL_ADC_Stop(&hadc1);									// stop adc

		if (Vfuse.currentValue < 300)							// check that the relay pulsed low
			RelayCount++;										// increment counter if pulled to ground
		else if (RelayCount > 0)								// decrease counter if not pulled to ground
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
/*
 * Board Comms receive routine to handle the data from the RS485 or Radio
 * The Routine should sort through the string of data determining whether the string
 * is correct by first receiving either 0xB2 21 or 0xB2 0F, if this condition is met it will
 * receive the specified amount of data determined by the length byte in the string.
 * Once collected communication_response() will determine whether the CRC is correct and
 * handle what the system does with the string and future processes.
 */
void USART2_IRQHandler(void) {
	/* USER CODE BEGIN USART2_IRQn 0 */
	if ((USART2->SR & USART_SR_RXNE) && (BoardCommsReceiveState == RxWaiting)) {	// Only receive data if receive is enabled and comms receive state in waiting
		if (UART2_RecPos > (LRGBUFFER))												//Overflow check for Receive position, if the size of buffer is larger than 254 reset to 0
			UART2_RecPos = 0;														// Overflow, if receive position exceeds size of buffer, set pos back to 0

		unsigned char data = USART2->DR;											//load the data from USART 2 into data

		if ((data == 0xB2 && !UART2_Recdata) || (data == 0x21 && UART2_RecPos == 1)
				|| (data == 0x0F && UART2_RecPos == 1)) { 							// if data is equal to the header and Receive data is not active, set the flag and begin storing data.
			if (data == 0xB2) {														// B2 start of data transmission header
				UART2_RecPos = 0;													// Set the receive position to zero
				UART2_Length = 0;
				UART2_Recdata = true;												// Set the receive data flag
			}
			UART2_RXbuffer[UART2_RecPos++] = data;									// Store the received data into the RXbuffer
		} else if (UART2_Recdata) {
			if (UART2_RecPos == 2) {
				UART2_Length = data;
				UART2_RXbuffer[UART2_RecPos++] = UART2_Length;
			} else
				UART2_RXbuffer[UART2_RecPos++] = data;

			if (UART2_RecPos == (UART2_Length + 3)) {								// while the position is less than the length copy data into the buffer else set receive data to false. to stop flow of data into the buffer
				UART2_Recdata = false;
				if (CRC_Check(&UART2_RXbuffer[0], UART2_RecPos)) {					// Check CRC of received string
					if (UART2_RXbuffer[1] == 0x0F) {								// Copy the Data received into the Databuffer from 3rd pos if second byte is 0x0F
						Datalen = UART2_RecPos - 5;
						memcpy(&Data_Buffer[0], &UART2_RXbuffer[3], Datalen);		// Allows for the serial number in the 4 bytes prior to the command byte
					} else {														// Copy the Data received into the Databuffer from 14th pos if second byte is 0x21
						Datalen = UART2_RecPos - 16;
						memcpy(&Data_Buffer[0], &UART2_RXbuffer[14], (UART2_RecPos));
					}
					BoardCommsReceiveState = RxGOOD;								// Set the receive state flag to Good once successful transmission
				} else {
					BoardCommsReceiveState = RxBAD;									// Set flag to bad if the CRC fails
					printT((uns_ch*) "CRC Error...\n");								// Print error to terminal
				}
				timeOutEn = false;													// Disable timeout once length is met
				USART2->CR1 &= ~(USART_CR1_RXNEIE);									// Disable Receive interrupt
			}
		} else
			UART2_Recdata = false;													//catch all statement for anything that makes it this far.
	}

/*
 * Transmit Interrupt Routine
 * Populate the UART transmit register with the data that is in the TXbuffer, if the end condition is met
 * turn the interrupts off and continue as expected.
 * Following the transmission of a string/interrupt turned off use settimeout() to determine whether communications are operating correctly
 */
	if ((USART2->SR & USART_SR_TXE) && UART2_TXcount) {								// Check if Transmit is Enabled, Transmit Count > 0
		if (BoardCommsTimeout == 0) {												// Board Comms timout reached zero, wake commands transmission should be complete from target board
			if (UART2_TXpos == UART2_TXcount) {										// TxPos reached TxCount, then disable interupt enable and get the system ready for response
				if (UART2_TXpos != 0)
					//disable interrupts
					if (UART2_TXcount > 0)											// Enable transmission complete interrupt if transmission has length greater than 0
						USART2->CR1 |= (USART_CR1_TCIE);
				USART2->CR1 &= ~(USART_CR1_TXEIE);									//Disable transmission interrupt
				UART2_TXcount = UART2_TXpos = 0;									// Return Count & Pos to zero	//TODO: Move this to the transmission complete part of the interrupt
				BoardCommsReceiveState = RxWaiting;
				setTimeOut(2000);
			} else {
				USART2->DR = UART2_TXbuffer[UART2_TXpos++];
			}
		}
	}
	if (READ_BIT(USART2->SR, USART_SR_TC) && UART2_TXcount == 0) {					// Check if transmission complete TODO: Remove the dependancy on count equal to zero
		USART2->CR1 &= ~(USART_CR1_TCIE);											// Disable Transmission complete interrupt
		USART2->CR1 &= ~(USART_CR1_TXEIE);											// Disable Transmission interrupt enable
		USART2->CR1 |= (USART_CR1_RE);												// Enable Receive
		CLEAR_BIT(USART2->SR, USART_SR_TC);											// Clear the transmission complete flag in the status register
		HAL_GPIO_WritePin(RS485_EN_GPIO_Port, RS485_EN_Pin, GPIO_PIN_RESET);		// Disable transmission enable pins on board comms outputs
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
/*
 * Barcode Scanner Receive Interrupt
 * Interrupt rountine to receive data from the barcode scanner.
 * Check the data for a carridge return while false and data received,1
 */
void USART3_IRQHandler(void) {
	/* USER CODE BEGIN USART3_IRQn 0 */
	if (USART3->SR & USART_SR_RXNE) {												// Check if receive data register is populated
		uns_ch data;
		data = USART3->DR;															// get the data from the register
		if (((data >= 0x30) && (data <= 0x39)) || (data == 0x0D)) {					// check if it is a valid ascii number, or carridge return
			if ((data == 0x0D) && (BarcodeCount > 0)) {								// Check if carridge return and barcode count exists
				BarcodeScanned = true;												// Set scanned flag to true
				USART3->CR1 &= ~(USART_CR1_RXNEIE);									// Disable receive interrupt
			} else {
				if (BarcodeCount >= 0x09) {											// Check for overrun of scan
					memmove(&BarcodeBuffer[0], &BarcodeBuffer[1], 8);				// Move the buffer down a position
					BarcodeCount--;
				}
				BarcodeBuffer[BarcodeCount++] = data;								// Populate the barcode buffer
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
/*
 * Interrupt Timer for Asynchronous Pulses
 * This timer is required to send asynchronous pulses at varying lengths. 2 Counters are implemented
 * to transmit a variety of fast and long pulses to determine whether the filtration is operating correctly
 * on the digital ports of the target board.
 */
void TIM8_TRG_COM_TIM14_IRQHandler(void) {
	/* USER CODE BEGIN TIM8_TRG_COM_TIM14_IRQn 0 */
	/* USER CODE END TIM8_TRG_COM_TIM14_IRQn 0 */
	HAL_TIM_IRQHandler(&htim14);
	/* USER CODE BEGIN TIM8_TRG_COM_TIM14_IRQn 1 */
	for (uint8 i = Port_1; i <= Port_9; i++) {										// Scan ports 1-9 to check if active and what kind of testing is required
		if (Port[i].Async.Active) {
			if (Port[i].Async.FilterEnabled) {										// Filter Enabled async test
				if (Port[i].Async.scount == 0) {
					if (--Port[i].Async.fcount == 0) {								// check if noise length is depleted then toggle the gpio
						Port[i].Async.fcount =
								AsyncDebounceBuffer[(Port[i].Async.PulseCount + i) % 5][Port[i].Async.FilteredCount++];	// next noise value
						HAL_GPIO_TogglePin(Port[i].Async.Port, Port[i].Async.Pin);	// toggle pin
						if (Port[i].Async.FilteredCount > 15) {						// if end of buffer reached set slow count for longer pulse
							Port[i].Async.FilteredCount = 0;
							Port[i].Async.scount = 250;
						}
					}
				} else {
					if (Port[i].Async.scount-- == 250) {
						if (Port[i].Async.PulseState) {
							--Port[i].Async.PulseCount;								// decrement pulse count
							HAL_GPIO_WritePin(Port[i].Async.Port, Port[i].Async.Pin, GPIO_PIN_SET);	// pulse high
						} else {
							Port[i].Async.Active = Port[i].Async.PulseCount;		// if pulse count async active, zero pulse count async deactivated
							HAL_GPIO_WritePin(Port[i].Async.Port, Port[i].Async.Pin, GPIO_PIN_RESET);	// reset the pin state
						}
						Port[i].Async.PulseState ^= 1;								// invert the pinstate
					}
				}
			} else if (++Port[i].Async.UnfilteredClkDiv == 16) {					// Unfiltered async test, clk dividing required to get slower pulses
				if (Port[i].Async.fcount > 0) {
					if (HAL_GPIO_ReadPin(Port[i].Async.Port, Port[i].Async.Pin)) {
						if (!(--Port[i].Async.fcount)) {							// When fcount depletes populate scount
							Port[i].Async.scount = 10;
							Port[i].Async.PulseState ^= 1;							// flip pin state
						}
						HAL_GPIO_WritePin(Port[i].Async.Port, Port[i].Async.Pin, GPIO_PIN_RESET);	// reset pin
					} else
						HAL_GPIO_WritePin(Port[i].Async.Port, Port[i].Async.Pin, GPIO_PIN_SET);		// set pin
				} else if (Port[i].Async.PulseState) {								// Pulse state high
					if (!(--Port[i].Async.scount)) {								// Decrement scount and check for depletion
						Port[i].Async.fcount = 5;									// Set the fcount
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
				Port[i].Async.UnfilteredClkDiv = 0;
			}
		}
	}
	if (!Port[Port_1].Async.Active && !Port[Port_2].Async.Active && !Port[Port_3].Async.Active
			&& !Port[Port_4].Async.Active && !Port[Port_5].Async.Active && !Port[Port_6].Async.Active
			&& !Port[Port_7].Async.Active && !Port[Port_8].Async.Active && !Port[Port_9].Async.Active) // Check if any of the async ports are still active
		AsyncComplete = true;
	/* USER CODE END TIM8_TRG_COM_TIM14_IRQn 1 */
}

/**
 * @brief This function handles TIM6 global interrupt.
 */
void TIM6_IRQHandler(void) {
	/* USER CODE BEGIN TIM6_IRQn 0 */
		// Flash LED to signify board operating correctly
	if (LEDcounter++ > 110) {														// Check LED count for long off timer
		HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);					// Set LED to on, set flag, reset counter
		LED1active = true;
		LEDcounter = 0;
	} else if (LED1active && (LEDcounter > 10)) {									// Check LED counter and active state
		HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);				// Turn LED off
		LED1active = false;															// disable LED flag
	}
	/* USER CODE END TIM6_IRQn 0 */
	HAL_TIM_IRQHandler(&htim6);
	/* USER CODE BEGIN TIM6_IRQn 1 */

	/* USER CODE END TIM6_IRQn 1 */
}

/**
 * @brief This function handles TIM7 global interrupt.
 */
/*
 //       ======      KEYPAD Scan Interupt      ======      //
  *		 1ms timer to scan keypad
 */
void TIM7_IRQHandler(void) {
	/* USER CODE BEGIN TIM7_IRQn 0 */
	/* USER CODE END TIM7_IRQn 0 */
	HAL_TIM_IRQHandler(&htim7);
	/* USER CODE BEGIN TIM7_IRQn 1 */
	/*
	 * Timer to check which key presses have been made. A simple matrix scan functionality is run, with counters
	 * incremented to eliminate switch bounce and be assured the user actually pressed the button
	 */
	for (uint8 currentScan = 0; currentScan < 12; currentScan++) {					// Scan keypad
		HAL_GPIO_WritePin(KP_R1_GPIO_Port, KP_R1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(KP_R2_GPIO_Port, KP_R2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(KP_R3_GPIO_Port, KP_R3_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(KP_R4_GPIO_Port, KP_R4_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(KP[currentScan].RowPort, KP[currentScan].RowPin, GPIO_PIN_RESET);	// Pull the current row being scanned low

		KP[currentScan].PreviousState = KP[currentScan].State;
		if (HAL_GPIO_ReadPin(KP[currentScan].ColPort, KP[currentScan].ColPin))
			(KP[currentScan].debounceCount)--;
		else
			(KP[currentScan].debounceCount)++;										// Increment counter, if ground is established

		if ((KP[currentScan].debounceCount >= 50)) {
			KP[currentScan].debounceCount = 50;										// Set pressed to true, if counter exceeds 50
			KP[currentScan].State = true;
		} else if ((KP[currentScan].debounceCount <= 5)) {							// hysteresis for pin so it can decrease and be set to false
			KP[currentScan].debounceCount = 5;
			KP[currentScan].State = false;
		}
		if (KP[currentScan].State != KP[currentScan].PreviousState) 				// change state if state does not match previous state
			KP[currentScan].Pressed = KP[currentScan].State;
	}
	/* USER CODE END TIM7_IRQn 1 */
}

/**
 * @brief This function handles USB On The Go FS global interrupt.
 */
void OTG_FS_IRQHandler(void) {
	/* USER CODE BEGIN OTG_FS_IRQn 0 */
	/* USER CODE END OTG_FS_IRQn 0 */
	HAL_PCD_IRQHandler(&hpcd_USB_OTG_FS);
	/* USER CODE BEGIN OTG_FS_IRQn 1 */
	/* USER CODE END OTG_FS_IRQn 1 */
}

/**
 * @brief This function handles USART6 global interrupt.
 */
/*
 * SDI-12 Interrupt Rountine
 * If SDI-12 is enabled the USART6 Receive interrupt will be enabled.
 * Until a '?' is received the system will be in an undefined state, once receive it will enter query
 * The routine will respond with the addres
 * SDSundef: return with the address of sensor
 * SDSc: 	respond with address and amount of measurements
 * SDSd:	respond with address and the measurement at that address
 */
void USART6_IRQHandler(void) {
	/* USER CODE BEGIN USART6_IRQn 0 */
	if (SDIenabled) {
		uint16 UART6status = USART6->SR;														// Read the status register
		unsigned char UART6data = (USART6->DR & 0X7F);											// Read data register, only 7 bytes of data LSB
		if (UART6status & USART_ERROR_MASK) {													//	Check the status register for errors
			SDSstate = SDSundef;																// Shift the state to undefined if any errors present
		} else if (UART6status & USART_SR_RXNE) {												// If the receive interrupt triggered
			if (UART6data == '?') {																// set the state to query if the first character of sensor measurement is received
				SDSstate = SDSquery;
			} else if (SDSstate == SDSundef) {
				if (UART6data == (SDIAddress + 0x30))											//if in undefined state ensure the response is the address of the sensor in ascii form
					SDSstate = SDSaddress;
				else
					SDSstate = SDSundef;														// return to undef if it is not the correct address
			} else if (SDSstate == SDSaddress) {												// When in the address state
				if (UART6data == 'C')
					SDSstate = SDSc;
				else if (UART6data == 'D')
					SDSstate = SDSdPending;
			} else if ((UART6data == '0') && (SDSstate == SDSdPending))
				SDSstate = SDSd;

			if (UART6data == '!') {																// handle the response following an !
				if (SDSstate == SDSquery) {
					USART6->CR1 &= ~(USART_CR1_RE);												// Disable interrupt
					//Set Address
					for (uint8 i = Port_1; i <= Port_6; i++) {									// Check sensor port 1-6 to see which port has SDI-12 active
						if (Port[i].Sdi.Enabled) {
							Port[i].Sdi.Enabled = false;
							SDIAddress = Port[i].Sdi.Address;
							SDIMeasurement = Port[i].Sdi.setValue;
						}
					}
					HAL_GPIO_WritePin(Buffer_OE_GPIO_Port, Buffer_OE_Pin, GPIO_PIN_RESET);		// Disable buffer for transmission
					sprintf((char*) &debugTransmitBuffer[0], "%d\x0D\x0A", (SDIAddress));		// transmit the address
					HAL_UART_Transmit(&SDI_UART, (uint8_t*) &debugTransmitBuffer[0],
							strlen((char*) debugTransmitBuffer), 10);
					HAL_GPIO_WritePin(Buffer_OE_GPIO_Port, Buffer_OE_Pin, GPIO_PIN_SET);		// reenable the buffer
					USART6->CR1 |= (USART_CR1_RE);												// reset the interrupt
					SDSstate = SDSundef;														// return to undef state
				} else if (SDSstate == SDSc) {
					USART6->CR1 &= ~(USART_CR1_RE); 											// Disable receive interrupt
					char *SDIrqMeasurements = malloc(4 * sizeof(char));
					sprintf(SDIrqMeasurements, "00001");										// amount of measurements to be returned
					HAL_GPIO_WritePin(Buffer_OE_GPIO_Port, Buffer_OE_Pin, GPIO_PIN_RESET);		// disable buffer so data can be transmit out
					sprintf((char*) &debugTransmitBuffer[0], "%d%s\x0D\x0A", (SDIAddress), SDIrqMeasurements);	// transmit the sensor address, measurements followed by caridge return line feed
					HAL_UART_Transmit(&SDI_UART, (uint8_t*) &debugTransmitBuffer[0],
							strlen((char*) debugTransmitBuffer), HAL_MAX_DELAY);
					HAL_GPIO_WritePin(Buffer_OE_GPIO_Port, Buffer_OE_Pin, GPIO_PIN_SET);		// reenable buffer to receive data
					USART6->CR1 |= (USART_CR1_RE);
					free(SDIrqMeasurements);
				} else if (SDSstate == SDSd) {
					USART6->CR1 &= ~(USART_CR1_RE); 											// Disable receive interrupt
					HAL_GPIO_WritePin(Buffer_OE_GPIO_Port, Buffer_OE_Pin, GPIO_PIN_RESET);
					sprintf((char*) &debugTransmitBuffer[0], "%d+%f\x0D\x0A", (SDIAddress), SDIMeasurement);
					HAL_UART_Transmit(&SDI_UART, (uint8_t*) &debugTransmitBuffer[0],
							strlen((char*) debugTransmitBuffer),
							HAL_MAX_DELAY);														// return the measurement
					HAL_GPIO_WritePin(Buffer_OE_GPIO_Port, Buffer_OE_Pin, GPIO_PIN_SET);
					USART6->CR1 |= (USART_CR1_RE);

				} else
					SDSstate = SDSundef;
			}
		}
	}
	/*
	 * States behave as below
	 * undef 		=> '\r' received 	=> RSquery
	 * RSquery 		=> '\n' received 	=> RS9pending
	 * RS9pending 	=> '9' received 	=> RS9
	 * RS9 			=> '9' received		=> RSMpending
	 * RSMpending	=> 'M' received		=> RSM
	 * RSM 			=> '\r' received	=> RSdPending
	 * RSdPending	=> '\n' received	=> RSd
	 * RSd 			=> return sensor values
 	 */
	if (RS485enabled) {
		uint16 UART6status = USART6->SR;
		if (UART6status & USART_ERROR_MASK) {
			RSstate = RSundef;								// Any error present reset the state back to undefined
		} else if (UART6status & USART_SR_RXNE) {
			uns_ch UART6data = USART6->DR;					// read data register
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
			HAL_GPIO_WritePin(RS485_4011EN_GPIO_Port, RS485_4011EN_Pin, GPIO_PIN_SET);						// Enable RS485 transmit
			sprintf((char*) &RS485buffer[0],
					"1,1,%.3f,2,%.3f,3,%.3f,4,%.3f,5,%.3f,6,%.3f,7,%.3f,8,%.3f,9,%.3f\r\n", //
					RS485sensorBuffer[0], RS485sensorBuffer[1], RS485sensorBuffer[2], RS485sensorBuffer[3],
					RS485sensorBuffer[4], RS485sensorBuffer[5], RS485sensorBuffer[6], RS485sensorBuffer[7],
					RS485sensorBuffer[8]);
			HAL_UART_Transmit(&SDI_UART, (uint8_t*) &RS485buffer[0], strlen((char*) &RS485buffer[0]),		// Transmit sensor data to target board
			HAL_MAX_DELAY);
			HAL_GPIO_WritePin(RS485_4011EN_GPIO_Port, RS485_4011EN_Pin, GPIO_PIN_RESET);					// Disable RS485 transmit
			RSstate = RSundef;																				// Return the RSstate to undefined
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
