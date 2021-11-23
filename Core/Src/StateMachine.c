#include <Calibration.h>
#include "main.h"
#include "ADC.h"
#include "Global_Variables.h"
#include "SetVsMeasured.h"
#include "UART_Routine.h"
#include "Programming.h"
#include "File_Handling.h"
#include "Channel_Analysis.h"
#include "Test.h"
#include "LCD.h"
#include "Init.h"
#include "v1.h"
#include "ScanLoom.h"
#include "SerialNumber.h"
#include "SDcard.h"
#include "math.h"
#include "Calibration.h"
#include "Communication.h"
#include "ErrorHandle.h"
#include "LCD.h"
#include "LatchTest.h"
#include "TestFunctions.h"

void handleCheckLoom(TboardConfig *Board, TprocessState *State) {
	switch (*State) {
	case psInitalisation:
		HAL_GPIO_WritePin(PIN2EN_GPIO_Port, PIN2EN_Pin, GPIO_PIN_RESET);	// Reset power to board
		HAL_GPIO_WritePin(PIN5EN_GPIO_Port, PIN5EN_Pin, GPIO_PIN_RESET);
		TargetBoardParamInit(true);							// Fully erase BoardConnected struct and associated memory
		*State = psWaiting;
		break;
	case psWaiting:
		BoardResetTimer = 500;											// Allow short time for reset
		SetBoardType(Board, LoomState);									// Set board type depending on loom connected
		*State = psComplete;
		break;
	case psComplete:
		if (BoardResetTimer == 0)
			currentBoardConnected(Board);							// Set test struct with the boardtype variable
		CurrentState = csIDLE;										// Return to the IDLE state when a loom is connected
		*State = psInitalisation;
		break;
	case psFailed:
		*State = psInitalisation;
		break;
	}
}

void handleIdle(TboardConfig *Board, TprocessState *State) {
	uint32 PreviousSerialNumber;
	switch (*State) {
	case psInitalisation:
		LCD_init();
		if (!HAL_GPIO_ReadPin(PIN2EN_GPIO_Port, PIN2EN_Pin)) {
			HAL_GPIO_WritePin(PIN2EN_GPIO_Port, PIN2EN_Pin, GPIO_PIN_SET);		// Enable power
			BoardResetTimer = 1500;												// Timer for board to reset
		}
		PrintHomeScreen(Board);								// Print Homescreen when the system returns to default case
		*State = psWaiting;
		break;
	case psWaiting:
		if (Board->BoardType && BoardResetTimer == 0) {
			PreviousSerialNumber = Board->SerialNumber;						// Store current serial number
			interrogateTargetBoard(); 						// Find if valid board is connected/determine serial number
			while ((BoardCommsReceiveState == RxWaiting) && (*State == psWaiting)) {
				if (KP[1].Pressed || KP[3].Pressed || KP[6].Pressed || KP[hash].Pressed || KP[star].Pressed) // button pressed start procedure associated with that button
					break;
			}
			if (BoardCommsReceiveState == RxGOOD) {						// Comms good, update home screen
				communication_response(Board, &Data_Buffer[4], &Data_Buffer[5], strlen((char*) &Data_Buffer) - 5);
			} else if (*State == psFailed) {
				Board->SerialNumber = 0;// If comms are removed return the system to batch mode ready for another board/Serial input
				TestRigMode = BatchMode;
			}
			if ((PreviousSerialNumber != Board->SerialNumber))
				PrintHomeScreen(Board);			// When previous serial number doesnt match current re print home screen
			if ((Board->SerialNumber == 0) && (TestRigMode == BatchMode)) { // No serial number and in batch mode jump to the serial number entry to begin testing
				PrintHomeScreen(Board);
				CLEAR_BIT(Board->BSR, BOARD_SERIALISED); // Clear serialised reg if no serial number present and in batch mode
				*State = psComplete;
			}

			if (KP[1].Pressed || KP[3].Pressed || KP[6].Pressed || KP[hash].Pressed) {
				TestRig_Init();
				TargetBoardParamInit(0);									// Reinitialise variables
				clearTestStatusLED();
				if (KP[1].Pressed) {
					TestRigMode = OldBoardMode;								// Test only, no serialise, no program
					SET_BIT(Board->BSR, BOARD_SERIALISED);// Set flags so system does not update serial or program board
					SET_BIT(Board->BSR, BOARD_PROGRAMMED);
				} else if (KP[hash].Pressed) {
					if (Board->SerialNumber != 0 && (~Board->SerialNumber != 0))
						SET_BIT(Board->BSR, BOARD_SERIALISED);
					TestRigMode = BatchMode;								// Program immediately
				} else if (KP[6].Pressed)
					TestRigMode = VerboseMode;								// Display everything to terminal
				else if (KP[3].Pressed) {
					TestRigMode = SerialiseMode;							// Update serial number only
					CLEAR_BIT(Board->BSR, BOARD_SERIALISED);
				}

				KP[1].Pressed = KP[3].Pressed = KP[6].Pressed = KP[hash].Pressed = false;
				LCD_Clear();												// Clear LCD screen
				*State = psComplete;										// Progress with the testing process
			}
		}
		//Calibration Routine
		if ((KP[7].Pressed && KP[9].Pressed)) {								// Calibrate Test Rig
			KP[7].Pressed = KP[9].Pressed = false;
			Calibration();
			LCD_Clear();
			TestRig_Init();													// Return to initial state
			printT((uns_ch*) "\n\n==========  Test Rig  ==========\n");
		}
		break;
	case psComplete:
		if (!READ_BIT(Board->BSR, BOARD_SERIALISED))
			CurrentState = csSerialNumberEntry;
		else
			CurrentState = csSolarCharger;								// Set the next state to test the solar charger
		*State = psInitalisation;
		break;
	case psFailed:
		retryCount++;
		if ((TestRigMode == BatchMode) && (retryCount > 2)) {// After two failed retries in batch mode disable power to board
			HAL_GPIO_WritePin(PIN2EN_GPIO_Port, PIN2EN_Pin, GPIO_PIN_RESET);
			*State = psComplete;
		} else if (retryCount > 2) {// After two retries return the system to initialisation in batchmode for new testing to begin
			if (Board->SerialNumber) {
				TargetBoardParamInit(0);
				Board->SerialNumber = 0x0;
				PrintHomeScreen(Board);
				TestRigMode = BatchMode;
			}
			CurrentState = csSerialNumberEntry;					// Return to serial entry to begin new test on new board
			*State = psInitalisation;
		} else
			*State = psWaiting;

		break;
	}
}

void handleSerialNumberEntry(TboardConfig *Board, TprocessState *State) {
	uint8 tempdata;
	switch (*State) {
	case psInitalisation:
		HAL_GPIO_WritePin(PIN2EN_GPIO_Port, PIN2EN_Pin, GPIO_PIN_RESET);
		USART2->CR1 &= ~(USART_CR1_RXNEIE);								// Disable Board Comms while serial being read
		TestRig_Init();
		PrintHomeScreen(Board);
		TargetBoardParamInit(0);							// Initialize targetboard variables, clear the board struct
		Board->SerialNumber = 0;
		clearTestStatusLED();											// Clear previously set LEDs on the front panel
		LCD_printf((uns_ch*) "Enter Serial Number", 2, 0);
		LCD_printf((uns_ch*) " * - Esc    # - Ent ", 4, 0);
		memset(&SerialNumber, 0, 9);
		LCD_ClearLine(3);
		LCD_setCursor(3, 0);
		LCD_CursorOn_Off(true);
		SerialCount = 0;
		USART3->CR1 |= (USART_CR1_RXNEIE);							// Enable Receive interupt for the barcode scanner
		tempdata = 0;
		*State = psWaiting;
		break;
	case psWaiting:
		tempdata = ScanKeypad();
		if (tempdata) {
			SerialNumber[SerialCount++] = tempdata;
			if (SerialCount >= 9) {
				memmove(&SerialNumber[0], &SerialNumber[1], 8);
				SerialCount--;
			}
			LCD_printf((uns_ch*) " * - BckSpc # - Ent", 4, 0);
			LCD_ClearLine(3);
			LCD_setCursor(3, 0);
			LCD_displayString(&SerialNumber[0], SerialCount);
		}
		if (SerialCount)
			QuitEnabled = false;

		if (KP[star].Pressed) {
			KP[star].Pressed = false;
			if (SerialCount) {
				SerialNumber[SerialCount--] = 0x08;
				LCD_setCursor(3, SerialCount + 1);
				sprintf((char*) &debugTransmitBuffer[0], "  ");
				LCD_displayString((uns_ch*) &debugTransmitBuffer[0], strlen((char*) debugTransmitBuffer));
				LCD_setCursor(3, SerialCount + 1);
				sprintf((char*) &debugTransmitBuffer[0], "\x8 \x8");
				printT(&debugTransmitBuffer[0]);
				if (SerialCount == 0)
					LCD_printf((uns_ch*) " * - Esc    # - Ent ", 4, 0);
				LCD_setCursor(3, SerialCount + 1);
			} else {
				CurrentState = csIDLE;
				*State = psInitalisation;
				QuitEnabled = true;
			}
		}
		if (KP[hash].Pressed) {
			KP[hash].Pressed = false;
			Board->SerialNumber = 0;
			printT((uns_ch*) &SerialNumber);
			LCD_printf((uns_ch*) "Serial Number:", 2, 0);
			uint8 i = 0;
			while (SerialCount != i) {
				Board->SerialNumber = (Board->SerialNumber * 10 + (SerialNumber[i++] - 0x30));
			}
			HAL_GPIO_WritePin(PIN2EN_GPIO_Port, PIN2EN_Pin, GPIO_PIN_SET);
			LCD_CursorOn_Off(false);
			LCD_ClearLine(4);
			QuitEnabled = true;
			BoardResetTimer = 2000;
			*State = psComplete;
		}

		// Barcode Scanned
		if (BarcodeScanned == true) {
			BarcodeScanned = false;
			Board->SerialNumber = 0;
			LCD_ClearLine(3);	//
			LCD_printf((uns_ch*) "Serial Number:", 2, 0);
			LCD_printf((uns_ch*) &BarcodeBuffer, 3, 0);
			printT((uns_ch*) &BarcodeBuffer);
			uint8 i = 0;
			while (BarcodeCount != i) {
				Board->SerialNumber = (Board->SerialNumber * 10 + (BarcodeBuffer[i++] - 0x30));
			}
			SerialCount = BarcodeCount;
			BarcodeCount = 0;
			USART3->CR1 &= ~(USART_CR1_RXNEIE);
			USART2->CR1 |= (USART_CR1_RXNEIE);
			HAL_GPIO_WritePin(PIN2EN_GPIO_Port, PIN2EN_Pin, GPIO_PIN_SET);
			LCD_CursorOn_Off(false);
			LCD_ClearLine(4);
			QuitEnabled = true;
			BoardResetTimer = 2000;
			*State = psComplete;
			break;
		}
		break;
	case psComplete:
		// Enable power, to communicate with the board
		if (BoardResetTimer == 0) {
			LCD_ClearLine(3);
			if (TestRigMode == BatchMode)
				CurrentState = csSolarCharger;
			else
				CurrentState = csSerialise;
			*State = psInitalisation;
		}
		break;
	case psFailed:
		retryCount++;
		if (retryCount > 3) {
			retryCount = 0;
			CurrentState = csIDLE;
		}
		*State = psInitalisation;
		break;
	}
}

void handleSolarCharger(TboardConfig *Board, TprocessState *State) {
	switch (*State) {
	case psInitalisation:
		if ((Board->BoardType == b402x || Board->BoardType == b427x) && !READ_BIT(Board->BVR, SOLAR_V_STABLE)) {
			testSolarCharger();
			*State = psWaiting;
		} else {
			SET_BIT(Board->BVR, SOLAR_V_STABLE);
			*State = psComplete;
		}
		break;
	case psWaiting:
		if (SolarChargerStable) {
			SolarChargerSampling = SolarChargerStable = false;
			SolarChargerTimer = SolarChargerCounter = 0;
			printT((uns_ch*) "Solar Charger Stable...\n");
			SET_BIT(Board->BVR, SOLAR_V_STABLE);
			*State = psComplete;
		} else if (!SolarChargerTimer)
			*State = psFailed;
		break;
	case psComplete:
		HAL_GPIO_WritePin(SOLAR_CH_EN_GPIO_Port, SOLAR_CH_EN_Pin, GPIO_PIN_RESET);
		CurrentState = csInputVoltage;
		*State = psInitalisation;
		break;
	case psFailed:
		HAL_GPIO_WritePin(SOLAR_CH_EN_GPIO_Port, SOLAR_CH_EN_Pin, GPIO_PIN_RESET);
		SolarChargerSampling = SolarChargerStable = false;
		HAL_GPIO_WritePin(FAIL_GPIO_Port, FAIL_Pin, GPIO_PIN_SET);
		printT((uns_ch*) "Solar Charger Failure...\n");
		CLEAR_BIT(Board->BVR, SOLAR_V_STABLE);
		CurrentState = csInputVoltage;
		*State = psInitalisation;
		break;
	}
}

void handleInputVoltage(TboardConfig *Board, TprocessState *State) {
	switch (*State) {
	case psInitalisation:
		if (!READ_BIT(Board->BVR, INPUT_V_STABLE)) {
			testInputVoltage();
			*State = psWaiting;
		} else
			*State = psComplete;

		sprintf((char*) &lcdBuffer[0], "%x", Board->BoardType);
		LCD_printf((uns_ch*) &lcdBuffer[0], 1, 0);
		sprintf((char*) &lcdBuffer, "S/N: %lu", Board->SerialNumber);
		LCD_setCursor(1, (21 - strlen((char*) &lcdBuffer)));
		LCD_displayString((uns_ch*) &lcdBuffer[0], strlen((char*) &lcdBuffer));
		break;
	case psWaiting:
		if (BoardResetTimer == 0 && !InputVoltageSampling) {
			InputVoltageSampling = true;
			InputVoltageTimer = 2000;
			InputVoltageCounter = 0;
		}
		if (InputVoltageSampling) {
			if (InputVoltageStable) {
				SET_BIT(Board->BVR, INPUT_V_STABLE);
				*State = psComplete;
			} else if (!InputVoltageTimer) {
				CLEAR_BIT(Board->BVR, INPUT_V_STABLE);
				*State = psComplete;
			}
		}
		break;
	case psComplete:
		InputVoltageSampling = InputVoltageStable = false;
		InputVoltageTimer = InputVoltageCounter = 0;
		printT((uns_ch*) "Input Voltage Stable...\n");
		if (READ_BIT(Board->BSR, BOARD_PROGRAMMED))
			CurrentState = csSerialise;
		else
			CurrentState = csProgramming;
		*State = psInitalisation;
		break;
	case psFailed:
		InputVoltageSampling = InputVoltageStable = false;
		HAL_GPIO_WritePin(FAIL_GPIO_Port, FAIL_Pin, GPIO_PIN_SET);
		printT((uns_ch*) "Input Voltage Failure...\n");
		CurrentState = csIDLE;
		*State = psInitalisation;
		break;
	}
}

void handleInitialising(TboardConfig *Board, TprocessState *State) {
	uint8 Response;
	switch (*State) {
	case psInitalisation:
		if (READ_BIT(Board->BSR, BOARD_TEST_PASSED) || !READ_BIT(Board->BSR, BOARD_INITIALISED)) {
			initialiseTargetBoard(Board);						// Initialize the target board
			*State = psWaiting;
		} else {
			*State = psComplete;
		}
		break;
	case psWaiting:
		if (BoardCommsReceiveState == RxGOOD) {
			Response = Data_Buffer[0];
			if (Response == 0xCD) {
				BoardResetTimer = 500;				//Allow board to reset, Required for electronic fuse
				*State = psComplete;				// Board initialized correctly
			} else
				*State = psFailed;								// Initialization failed
		} else if (BoardCommsReceiveState == RxBAD) {
			*State = psFailed;									// Board comms failed
		}
		break;
	case psComplete:
		SET_BIT(Board->BSR, BOARD_INITIALISED);
		if (READ_BIT(Board->BSR, BOARD_TEST_PASSED)) {			// If testing is complete re-run initialization
			TestComplete(Board);								// Set pass LED
			CurrentState = csIDLE;
			*State = psInitalisation;
		} else if (BoardResetTimer == 0) {
			CurrentState = csInterogating;
			*State = psInitalisation;
		}
		break;
	case psFailed:
		retryCount++;
		ProcessState = psInitalisation;
		break;
	}
}

void handleProgramming(TboardConfig *Board, TprocessState *State) {
	uint8 data[4];
	uint8 response[4];
	response[2] = 0;
	uns_ch tempLine[100];
	FRESULT res;
	switch (*State) {
	case psInitalisation:
		if (!READ_BIT(Board->BPR, PROG_INITIALISED)) {
			printT((uns_ch*) "Programming\n");
			SPI3->CR1 &= ~(SPI_BAUDRATEPRESCALER_32);
			SPI3->CR1 |= (0xFF & SPI_BAUDRATEPRESCALER_256);

			HAL_GPIO_WritePin(TB_Reset_GPIO_Port, TB_Reset_Pin, GPIO_PIN_RESET);
			HAL_Delay(20);
			SET_BIT(Board->BPR, PROG_INITIALISED);
			response[2] = 0xFF;
		} else if (!READ_BIT(Board->BPR, PROG_ENABLED)) {
			data[0] = 0xAC;
			data[1] = 0x53;
			data[2] = 0x00;
			data[3] = 0x00;
			HAL_SPI_TransmitReceive(&hspi3, &data[0], &response[0], 4, HAL_MAX_DELAY);
			HAL_Delay(20);
			if (response[2] == 0x53)
				SET_BIT(Board->BPR, PROG_ENABLED);
			else
				*State = psFailed;
		} else if (!READ_BIT(Board->BPR, CHIP_ERASED)) {
			data[0] = 0xAC;	//	Chip Erase
			data[1] = 0x80;
			data[2] = 0x00;
			data[3] = 0x00;
			HAL_SPI_Transmit(&hspi3, &data[0], 4, HAL_MAX_DELAY);
			HAL_Delay(10);	// Chip erase delay
			SET_BIT(Board->BPR, CHIP_ERASED);
		} else if (!READ_BIT(Board->BPR, CLOCK_SET)) {
			//Poll RDY
			PollReady();
			//Set Clk to 8Mhz
			data[0] = 0xAC;
			data[1] = 0xA0;		//Fuse Low Byte
			data[2] = 0x00;
			data[3] = HIGH_CLK_LOW_FUSE;
			HAL_SPI_TransmitReceive(&hspi3, &data[0], &response[0], 4, HAL_MAX_DELAY);
			HAL_Delay(20);
			data[1] = 0xA8;		//Fuse High Byte
			data[3] = HIGH_CLK_HIGH_FUSE;
			HAL_SPI_TransmitReceive(&hspi3, &data[0], &response[0], 4, HAL_MAX_DELAY);
			HAL_Delay(20);
			data[1] = 0xA4;		//Fuse Extended Byte
			data[3] = HIGH_CLK_EXT_FUSE;
			HAL_SPI_TransmitReceive(&hspi3, &data[0], &response[0], 4, HAL_MAX_DELAY);

			HAL_Delay(20);
			SPI3->CR1 &= ~(SPI_BAUDRATEPRESCALER_256);
			SPI3->CR1 |= (0xFF & SPI_BAUDRATEPRESCALER_32);
			ProgrammingCount = 0;
			HAL_GPIO_WritePin(TB_Reset_GPIO_Port, TB_Reset_Pin, GPIO_PIN_SET);
			SET_BIT(Board->BPR, CLOCK_SET);
		} else if (!READ_BIT(Board->BPR, FUSE_VALIDATED)) {
			EnableProgramming();
			// Read Fuse Bits
			data[0] = 0x50;	//	Read Low Byte
			data[1] = 0x00;
			data[2] = 0x00;
			data[3] = 0x00;
			HAL_SPI_TransmitReceive(&hspi3, &data[0], &response[0], 4, HAL_MAX_DELAY);
			if (response[3] == HIGH_CLK_LOW_FUSE) {
				data[1] = 0x08;
				HAL_SPI_TransmitReceive(&hspi3, &data[0], &response[0], 4, HAL_MAX_DELAY);
				if (response[3] == HIGH_CLK_EXT_FUSE) {
					data[0] = 0x58;
					HAL_SPI_TransmitReceive(&hspi3, &data[0], &response[0], 4, HAL_MAX_DELAY);
					if (response[3] == HIGH_CLK_HIGH_FUSE)
						SET_BIT(Board->BPR, FUSE_VALIDATED);
				}
			}

		} else if (!READ_BIT(Board->BPR, FILE_FOUND_OPEN)) {
			//Find file
			if (FindBoardFile(Board, &SDcard)) {
				res = OpenFile(&SDcard);
				if (res != FR_OK) {
					*State = psFailed;
					break;
				}
				LCD_printf((uns_ch*) "    Programming    ", 2, 0);
				ProgressBarTarget = round((float) ((fileSize / 2) / MAX_PAGE_LENGTH) * 0.7);
				SetSDclk(1);
				page = PageBufferPosition = LineBufferCount = 0;
				CLEAR_REG(Board->BPR);
				*State = psWaiting;
			} else {
				printT((uns_ch*) "Could not find hex file...\n");
				CurrentState = csProgramming;
				*State = psFailed;
			}
		}
		break;
	case psWaiting:
		if (f_gets((TCHAR*) &tempLine, sizeof(tempLine), &(SDcard.file))) {
			sortLine((uns_ch*) &tempLine[0], &LineBuffer[0], &LineBufferCount);
			if (populatePageBuffer(&PageBuffer[PageBufferPosition], &PageBufferPosition, (uns_ch*) &LineBuffer,
					&LineBufferCount))
				SET_BIT(Board->BPR, PAGE_WRITE_READY);
		} else {
			while (PageBufferPosition < MAX_PAGE_LENGTH) {
				PageBuffer[PageBufferPosition++] = 0xFF;
			}
			SET_BIT(Board->BPR, PAGE_WRITE_READY);
			SET_BIT(Board->BPR, FINAL_PAGE_WRITE);
		}

		if (READ_BIT(Board->BPR, PAGE_WRITE_READY)) {
			uns_ch data[4];
			if (page == 0) {
				data[0] = 0x4D;
				data[1] = 0x00;
				data[2] = 0x00;
				data[3] = 0x00;
				HAL_SPI_Transmit(&hspi3, (uint8_t*) &data, 4, HAL_MAX_DELAY);
			}
			PageWrite(&PageBuffer[0], MAX_PAGE_LENGTH / 2, page);
			if (!VerifyPage(page, &PageBuffer[0])) {
				PageWrite(&PageBuffer[0], MAX_PAGE_LENGTH / 2, page);
				if (!VerifyPage(page, &PageBuffer[0])) {
					*State = psFailed;
				}
			}
			CLEAR_BIT(Board->BPR, PAGE_WRITE_READY);
			PageBufferPosition = 0;
			if (READ_BIT(Board->BPR, FINAL_PAGE_WRITE)) {
				*State = psComplete;
				PageBufferPosition = page = 0;
				SPI3->CR1 &= ~(SPI_BAUDRATEPRESCALER_32);
				SPI3->CR1 |= (0xFF & SPI_BAUDRATEPRESCALER_256);
				HAL_Delay(10);
				SetClkAndLck(Board);
				printT((uns_ch*) "Programming Done\n");
				HAL_GPIO_WritePin(TB_Reset_GPIO_Port, TB_Reset_Pin, GPIO_PIN_SET);
				BoardResetTimer = 1500;	//Allow for board to power up
			} else {
				if (LineBufferCount)
					populatePageBuffer(&PageBuffer[PageBufferPosition], &PageBufferPosition, &LineBuffer[0],
							&LineBufferCount);
				Percentage = (uint8) ((page / (float) ProgressBarTarget) * 100);
				ProgressBar(Percentage);
			}
			page++; //increment page counter
		}
		break;
	case psComplete:
		if (BoardResetTimer == 0) {
			retryCount = 0;
			SET_BIT(BoardConnected.BSR, BOARD_PROGRAMMED);
			Close_File(&SDcard);
			SetSDclk(0);
			LCD_ClearLine(3);
			LCD_ClearLine(4);
			CurrentState = csSerialise;
			*State = psInitalisation;
		}
		break;
	case psFailed:
		printT((uns_ch*) "Programming Failed\n");
		Close_File(&SDcard);
		retryCount++;
		HAL_GPIO_WritePin(TB_Reset_GPIO_Port, TB_Reset_Pin, GPIO_PIN_SET);
		*State = psInitalisation;
		if (retryCount >= 3) {
			printT((uns_ch*) "Cannot Program Target Board\n");
			printT((uns_ch*) "Attempted 3 times....\n");
			CurrentState = csIDLE;
			*State = psWaiting;
		}
		break;
	}
}

void handleCalibrating(TboardConfig *Board, TprocessState *State) {
	uint8 Response;
	switch (*State) {
	case psInitalisation:
		if (READ_BIT(Board->BSR, BOARD_CALIBRATED) || (Board->BoardType == b422x)) {
			*State = psComplete;
		} else {
			sprintf((char*) &lcdBuffer, "    Calibrating");
			LCD_printf((uns_ch*) &lcdBuffer, 2, 0);
			CLEAR_REG(CalibrationStatusRegister);
			CalibratingTimer = 0;
			TargetBoardCalibration_Voltage(Board);
			*State = psWaiting;
		}
		break;
	case psWaiting:
		if (BoardCommsReceiveState == RxGOOD) {
			Response = Data_Buffer[0];
			if (Response == 0xC1) {
				uint8 FailedCalibrationCount = 0;
				float CalibrationValue;
				for (uint8 i = 2; i < (Board->analogInputCount * sizeof(float) * 5); i += 4) { // 5 different forms of calibration on each port
					memcpy(&CalibrationValue, &Data_Buffer[i], sizeof(float)); // IEEE 4 byte floating point values returned, check for 1.000 for uncalibrated ports
					if (CalibrationValue == 1.000)
						FailedCalibrationCount++;
				}
				if (FailedCalibrationCount > 2)			// If more than 2 ports are uncalibrated fail the calibration
					*State = psFailed;
				else {
					BoardResetTimer = 300;
					*State = psComplete;
				}
			} else
				*State = psFailed;
		} else if (BoardCommsReceiveState == RxBAD) {
			*State = psFailed;
		}
		break;
	case psComplete:
		if (BoardResetTimer == 0) {
			SET_BIT(BoardConnected.BSR, BOARD_CALIBRATED);
			printT((uns_ch*) "=====     Board Calibrated     =====\n");
			CurrentState = csConfiguring;
			*State = psInitalisation;
		}
		break;
	case psFailed:
		retryCount++;
		if (retryCount < 3) {
			printT((uns_ch*) "Calibration Failed Recalibrating Device\n");
			CLEAR_REG(CalibrationStatusRegister);
			ProcessState = psInitalisation;
			BoardCommsReceiveState = RxWaiting;
		} else {
			*State = psInitalisation;
			CurrentState = csIDLE;
		}
		break;
	}
}

void handleInterogating(TboardConfig *Board, TprocessState *State) {
	uns_ch Response;
	switch (*State) {
	case psInitalisation:
		interrogateTargetBoard();
		*State = psWaiting;
		break;
	case psWaiting:
		if (BoardCommsReceiveState == RxGOOD) {
			Response = Data_Buffer[4];
			if (Response == 0x11)
				*State = psComplete;
			else
				*State = psFailed;
		} else if (BoardCommsReceiveState == RxBAD) {
			*State = psFailed;
		}
		break;
	case psComplete:
		if (READ_BIT(Board->BSR, BOARD_CALIBRATED)) {
			communication_response(Board, &Response, &Data_Buffer[5], strlen((char*) Data_Buffer));
			CurrentState = csConfiguring;
			*State = psInitalisation;
		} else if (READ_BIT(Board->BSR, BOARD_INITIALISED)) {
			communication_response(Board, &Response, &Data_Buffer[5], strlen((char*) Data_Buffer));
			retryCount = 0;
			CurrentState = csCalibrating;
			*State = psInitalisation;
		} else {
			communication_response(Board, &Response, &Data_Buffer[5], strlen((char*) Data_Buffer));
			currentBoardConnected(Board);
			if (Board->Version > getCurrentVersion(Board->BoardType)) {	// If files on the SD card are outdated warn the user.
				LCD_printf((uns_ch*) "Update Firmware", 2, 0);
				LCD_printf((uns_ch*) "Files Immediately", 3, 0);
				while (!KP[hash].Pressed) {

				}
				LCD_Clear();
			}
			CurrentState = csInitialising;
			*State = psInitalisation;
		}
		break;

	case psFailed:
		if (retryCount > 3) {
			if (!READ_BIT(BoardConnected.BSR, BOARD_PROGRAMMED)) {
				currentBoardConnected(&BoardConnected);
				CurrentState = csProgramming;
			} else
				CurrentState = csIDLE;
			*State = psInitalisation;
		} else {
			retryCount++;
			BoardCommsReceiveState = RxWaiting;
			*State = psInitalisation;
		}
		break;
	}
}

_Bool OutputsSet;

void handleConfiguring(TboardConfig *Board, TprocessState *State) {
	uns_ch Response;
	switch (*State) {
	case psInitalisation:
		configureTargetBoard(Board);
		if (Board->BoardType == b422x) {	//Given all ports latch test just skip to latch test
			TestFunction(Board);
			*State = psComplete;
		} else
			*State = psWaiting;
		break;
	case psWaiting:
		if (BoardCommsReceiveState == RxGOOD) {
			if (!OutputsSet)	//only set the outputs once
				TestFunction(Board);
			Response = Data_Buffer[0];
			if (Response == 0x57) {
				//pause for board reset
				if (TestRigMode == VerboseMode)
					printT((uns_ch*) "===Board Configuration Successful===\n");
				BoardResetTimer = 500;
				*State = psComplete;
			} else
				*State = psFailed;
		} else if (BoardCommsReceiveState == RxBAD) {
			*State = psFailed;
		} else if (!OutputsSet) {
			TestFunction(Board);
		}
		break;
	case psComplete:
		if (BoardResetTimer == 0) {
			CurrentState = csOutputTest;
			*State = psInitalisation;
		}
		break;
	case psFailed:
		if (retryCount > 4) {
			CurrentState = csInterogating;
			*State = psInitalisation;
		} else {
			retryCount++;
//			HAL_Delay(100);
			configureTargetBoard(Board);
			*State = psWaiting;
			BoardCommsReceiveState = RxWaiting;
		}
		break;
	}
}

void handleOutputTest(TboardConfig *Board, TprocessState *State) {
	uns_ch Response;
	switch (*State) {
	case psInitalisation:
		if (RelayPort_Enabled) {  // Output test for the 402x Boards, tests port 10 pulls low
			BoardCommsParameters[0] = 0x89;
			BoardCommsParameters[1] = 0x05;
			BoardCommsParameters[2] = 0x00;
			BoardCommsParameters[3] = 0x00;
			BoardCommsParameters[4] = 0x00;
			BoardCommsParameters[5] = 0x05;
			BoardCommsParameters[6] = 0x00;
			communication_array(0x26, &BoardCommsParameters[0], 5);
			RelayCount = 125;
			*State = psWaiting;
		} else
			*State = psComplete;
		break;
	case psWaiting:
		if (BoardCommsReceiveState == RxGOOD) {
			Response = Data_Buffer[0];
			if (Response == 0x27) {
				if (RelayCount > 250) {
					*State = psComplete;
				} else if (RelayCount == 0) {
					*State = psFailed;
				}
			}
		} else if (BoardCommsReceiveState == RxBAD) {
			*State = psFailed;
		}
		break;
	case psComplete:
		RelayPort_Enabled = false;
		CurrentState = csLatchTest;
		*State = psInitalisation;
		break;
	case psFailed:
		SET_BIT(Board->TPR, (1 << Board->GlobalTestNum));
		*State = psComplete;
		break;
	}
}

void handleLatchTest(TboardConfig *Board, TprocessState *State) {
	uns_ch Response;
	switch (*State) {
	case psInitalisation:
		LatchTestInit();
		if (LatchingSolenoidDriverTest(Board)) {
			LCD_printf((uns_ch*) "   Latch Testing    ", 2, 0);
			*State = psWaiting;
		} else
			*State = psComplete;
		break;
	case psWaiting:
		if (LatchADCflag) {
			if (LatchCountTimer < 2048) {
				HandleLatchSample();
			} else
				CLEAR_BIT(LatchTestStatusRegister, LATCH_SAMPLING);
			LatchADCflag = false;

			if (!READ_BIT(LatchTestStatusRegister, LATCH_SAMPLING))
				*State = psComplete;
			break;
			case psComplete:
			if (READ_REG(Board->LatchTestPort)) {
				HAL_TIM_Base_Stop(&htim10);
				//Print Results & Error Messages
				TransmitResults(Board);
				normaliseLatchResults();
				PrintLatchResults();
				LatchErrorCheck(Board);
				if (Board->LTR == 0) {
					printT(
							(uns_ch*) "\n=======================          LATCH TEST PASSED         =======================\n\n");
					Board->TestResults[Board->GlobalTestNum][LatchTestPort * 2] = 1000; // Base 1000 for easier sorting results															// Multiply by two to acount for 2ch of data
				} else {
					printLatchError(Board);
					printT(
							(uns_ch*) "\n=======================          LATCH TEST FAILED          =======================\n\n");
					Board->TestResults[Board->GlobalTestNum][LatchTestPort * 2] = 0;
				}
				CLEAR_REG(Board->LatchTestPort);
			}
			if (Board->BoardType == b422x)
				CurrentState = csSampling;
			else
				CurrentState = csAsyncTest;
			*State = psInitalisation;
			break;
			case psFailed:
			retryCount++;
			if (retryCount > 3) {
				HAL_GPIO_WritePin(PIN2EN_GPIO_Port, PIN2EN_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(PIN5EN_GPIO_Port, PIN5EN_Pin, GPIO_PIN_RESET);
				CurrentState = csIDLE;
			} else {
				if (BoardCommsParameters[0] & 0x80) {
					communication_array(0x26, &BoardCommsParameters[0], 1);
				}
				retryCount++;
			}
			*State = psInitalisation;
			break;
		}
	}
}

void handleAsyncTest(TboardConfig *Board, TprocessState *State) {
	switch (*State) {
	case psInitalisation:
		HAL_TIM_Base_Start(&htim14);
		AsyncComplete = false;
		for (uint8 i = Port_1; i <= Port_9; i++) {
			Port[i].Async.Active = Port[i].Async.PulseCount ? true : false;
		}
		*State = psWaiting;
		break;
	case psWaiting:
		if (AsyncComplete)
			*State = psComplete;
		break;
	case psComplete:
		HAL_TIM_Base_Stop(&htim14);
		CurrentState = csSampling;
		*State = psInitalisation;
		break;
	case psFailed:
		retryCount++;
		if (retryCount > 3) {
			HAL_GPIO_WritePin(PIN2EN_GPIO_Port, PIN2EN_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(PIN5EN_GPIO_Port, PIN5EN_Pin, GPIO_PIN_RESET);
			HAL_TIM_Base_Stop(&htim14);
			CurrentState = csIDLE;
		}
		*State = psInitalisation;
		break;
	}
}

void handleSampling(TboardConfig *Board, TprocessState *State) {
	uns_ch Response;
	volatile uint16 VuserSamples;
	uint8 Percentage;
	switch (*State) {
	case psInitalisation:
		sampleTargetBoard(Board);
		VuserSamples = 0;
		*State = psWaiting;
		break;
	case psWaiting:
		if (BoardCommsReceiveState == RxGOOD) {
			Response = Data_Buffer[0];
			if (Response == 0x1B || Response == 0x03)	//TODO: change this to wait until samples uploaded!
				communication_response(Board, &Response, &Data_Buffer[1], Datalen);
			BoardCommsReceiveState = RxWaiting;
		} else if (BoardCommsReceiveState == RxBAD) {
			*State = psFailed;
			BoardCommsReceiveState = RxWaiting;
		}
		if (samplesUploading && (sampleTime >= 1000)) {	// Print percentage bar to LCD if the sample wait time is greater than 1000
			if (sampleCount == 0)
				LCD_ClearLine(3);
			Percentage = (uint8) ((sampleCount / ((float) sampleTime * 0.9)) * 100);// Reduce SampleTime range to ensure progress bar reaches 100/ holds 100 for a short time
			ProgressBar(Percentage);					// Print the progress bar and percentage
		}

		if (samplesUploaded) {
			samplesUploading = false;
			samplesUploaded = false;// Once timer times out ensure the system stops waiting and sample upload the results from the target board
			*State = psComplete;
			BoardCommsReceiveState = RxWaiting;
		}
		break;
	case psComplete:
		if (Board->GlobalTestNum == V_12output) {
			if ((Board->BoardType == b401x) || (Board->BoardType == b402x))
				Vuser.total += getOutputVoltage();
			else
				Vuser.total += getFuseVoltage();
		} else
			Vuser.total += getSampleVoltage();
		VuserSamples++;
		if (TestRigMode == VerboseMode) {		// print what is occuring if the system is in verbose mode
			printT((uns_ch*) "Samples Uploaded\n\n");
			printT((uns_ch*) "Requesting Results\n\n");
		}

		if (VuserSamples >= 100) {
			Vuser.total /= VuserSamples;
			Vuser.average = Vuser.total * (15.25 / 4096);		// calculate the average sample voltage
			if (compareSampleVoltage(Board, &(Vuser.average), &(setSampleVoltages[Board->GlobalTestNum]))) {// Check to see if the average
				*State = psInitalisation;
				CurrentState = csUploading;
			}
			VuserSamples = Vuser.total = sampleCount = 0;
		}
		break;
	case psFailed:
		retryCount++;
		if (retryCount > 3) {
			HAL_GPIO_WritePin(PIN2EN_GPIO_Port, PIN2EN_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(PIN5EN_GPIO_Port, PIN5EN_Pin, GPIO_PIN_RESET);
			CurrentState = csIDLE;
		}
		*State = psInitalisation;
		break;
	}
}

void handleUploading(TboardConfig *Board, TprocessState *State) {
	uns_ch Response;
	switch (*State) {
	case psInitalisation:
		uploadSamplesTargetBoard(Board);
		*State = psWaiting;
		break;
	case psWaiting:
		if (BoardCommsReceiveState == RxGOOD) {
			Response = Data_Buffer[0];
			if (Response == 0x19)
				*State = psComplete;
			if (Response == 0x03)
				communication_response(Board, &Response, (uns_ch*) &Data_Buffer[0], Datalen);
			BoardCommsReceiveState = RxWaiting;
		} else if (BoardCommsReceiveState == RxBAD) {
			*State = psFailed;
		}
		break;
	case psComplete:
		communication_response(Board, &Response, &Data_Buffer[1], Datalen - 1);
		*State = psInitalisation;
		CurrentState = csSortResults;
		break;
	case psFailed:
		BoardCommsReceiveState = RxWaiting;
		if (retryCount > 16) {
			HAL_GPIO_WritePin(FAIL_GPIO_Port, FAIL_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(PIN2EN_GPIO_Port, PIN2EN_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(PIN5EN_GPIO_Port, PIN5EN_Pin, GPIO_PIN_RESET);
			CurrentState = csIDLE;
			*State = psWaiting;
		} else {
			retryCount++;
			*State = psInitalisation;
		}
		break;
	}
}

void handleSortResults(TboardConfig *Board, TprocessState *State) {
	switch (*State) {
	case psInitalisation:
		Decompress_Channels(Board, (uns_ch*) &sampleBuffer[0]); //Returns the ammount of channels sampled
		*State = psWaiting;
		break;
	case psWaiting:
		sprintf(SDcard.FILEname, "/TEST_RESULTS/%lu_%x/%lu.CSV", Board->SerialNumber, Board->BoardType,
				Board->SerialNumber);
		HandleResults(Board, &CHval[Board->GlobalTestNum][0]);
		*State = psComplete;
		break;
	case psComplete:
		Board->GlobalTestNum++; //Increment Test Number
		if (CheckTestNumber(Board)) {
			// change this to skip to latch test if board 4220 is connected
			*State = psInitalisation;
			CurrentState = csConfiguring;
		} else {
			WriteVoltages(Board, &SDcard);
			if (READ_BIT(Board->BSR, BOARD_TEST_PASSED)) {
				CurrentState = csInitialising;
				*State = psInitalisation;
			} else {
				TestComplete(Board);
				LCD_printf((uns_ch*) "# - Cont", 4, 13);
				while (!KP[hash].Pressed) {

				}
				KP[hash].Pressed = false;
				LCD_Clear();
				*State = psInitalisation;
				CurrentState = csIDLE;
			}
		}
		break;
	case psFailed:
		retryCount++;
		if (retryCount > 3) {
			HAL_GPIO_WritePin(PIN2EN_GPIO_Port, PIN2EN_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(PIN5EN_GPIO_Port, PIN5EN_Pin, GPIO_PIN_RESET);
			CurrentState = csIDLE;
		}
		*State = psInitalisation;
		break;
	}
}

void handleSerialise(TboardConfig *Board, TprocessState *State) {
	uint8 Command;
	uint8 Response;
	uint32 tempSerial;
	uint32 CurrentSerial;
	switch (*State) {
	case psInitalisation:
		if (READ_BIT(Board->BSR, BOARD_SERIALISED))
			*State = psComplete;
		else {
			interrogateTargetBoard();
			*State = psWaiting;
		}
		break;
	case psWaiting:
		if (BoardCommsReceiveState == RxGOOD) {
			tempSerial = ReadSerialNumber(&Response, &Data_Buffer[0], Datalen);
			if (tempSerial == Board->SerialNumber) {
				SET_BIT(Board->BSR, BOARD_SERIALISED);
				*State = psComplete;
			} else if (Response == 0x11) {
				memcpy(&CurrentSerial, &tempSerial, 4); 		//Load Current Serial Number
				Command = 0x12;
				communication_arraySerial(Command, CurrentSerial, Board->SerialNumber);	//Write New Serial Number
				BoardCommsReceiveState = RxWaiting;
			} else if (Response == 0x13) {
				if (tempSerial != Board->SerialNumber) {
					Command = 0x10;
					communication_arraySerial(Command, 0, 0);
					*State = psInitalisation;
				} else {
					SET_BIT(Board->BSR, BOARD_SERIALISED);
					*State = psComplete;
				}
				BoardCommsReceiveState = RxWaiting;
			}
		} else if (BoardCommsReceiveState == RxBAD)
			*State = psComplete;
		break;
	case psComplete:
		if (READ_BIT(Board->BSR, BOARD_SERIALISED)) {
			CreateResultsFile(&SDcard, Board);
			Close_File(&SDcard);
			printT((uns_ch*) "=====     Board Serialised     =====\n");
			if (TestRigMode == SerialiseMode)
				CurrentState = csIDLE;
			else
				CurrentState = csInterogating;
			*State = psInitalisation;
		} else
			*State = psFailed;
		break;
	case psFailed:
		retryCount++;
		if (retryCount > 3) {
			HAL_GPIO_WritePin(PIN2EN_GPIO_Port, PIN2EN_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(PIN5EN_GPIO_Port, PIN5EN_Pin, GPIO_PIN_RESET);
			CurrentState = csIDLE;
		}
		*State = psInitalisation;
		break;
	}
}

