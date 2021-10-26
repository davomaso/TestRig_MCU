#include <Calibration.h>
#include "main.h"
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
#include "LatchTest.h"
#include "TestFunctions.h"

void handleIdle(TboardConfig *Board, TprocessState *State) {
	uint32 PreviousSerialNumber;
	switch (*State) {
	case psInitalisation:
		PrintHomeScreen(Board);												// Print Homescreen when the system returns to default case
		*State = psWaiting;
		break;
	case psWaiting:
		if (CheckLoom) {
			scanLoom(&BoardConnected); 										// Scan loom to determine which board is connected
			HAL_GPIO_WritePin(PIN2EN_GPIO_Port, PIN2EN_Pin, GPIO_PIN_SET);	// Enable 2Pin power
			PreviousSerialNumber = Board->SerialNumber;						// Store current serial number
			interrogateTargetBoard();										// Find if valid board is connected/determine serial number
			while ((BoardCommsReceiveState != RxGOOD) && (*State == psWaiting)) {
				if (BoardCommsReceiveState == RxBAD)						// Comms go bad, no board connected
					break;
				if (KP[1].Pressed || KP[3].Pressed || KP[6].Pressed || KP[hash].Pressed)	// button pressed start procedure associated with that button
					break;
			}
			if (BoardCommsReceiveState == RxGOOD)							// Comms good, update home screen
				communication_response(Board, &Data_Buffer[4], &Data_Buffer[5], strlen(Data_Buffer) - 5);
			if ((PreviousSerialNumber != Board->SerialNumber ))
				PrintHomeScreen(Board);		// When previous serial number doesnt match current re print home screen
			if ( (Board->SerialNumber == 0) && (TestRigMode == BatchMode) )
				*State = psComplete;
		}

		if (KP[1].Pressed || KP[3].Pressed || KP[6].Pressed || KP[hash].Pressed) {
			TestRig_Init();
			TargetBoardParamInit(0);										// Reinitialise variables
			clearTestStatusLED();
			if (KP[1].Pressed)
				TestRigMode = OldBoardMode;									// Test only, no serialise, no program
			else if (KP[hash].Pressed)
				TestRigMode = BatchMode;									// Program immediately
			else if (KP[6].Pressed)
				TestRigMode = VerboseMode;									// Display everything to terminal
			else if (KP[3].Pressed)
				TestRigMode = SerialiseMode;								// Update serial number only

			KP[1].Pressed = KP[3].Pressed = KP[6].Pressed = KP[hash].Pressed = false;
			LCD_Clear();													// Clear LCD screen
			*State = psComplete;											// Progress with the testing process
		}
		//Calibration Routine
		if ((KP[7].Pressed && KP[9].Pressed)) {								// Calibrate Test Rig
			KP[7].Pressed = KP[9].Pressed = false;
			Calibration();
			LCD_Clear();
			TestRig_Init();
			TestRig_MainMenu();
			printT((uns_ch*) "\n\n==========  Test Rig  ==========\n");
		}
		break;
	case psComplete:
		if (TestRigMode == OldBoardMode) {									// Test only, no serialise, no program
			SET_BIT(Board->BSR, BOARD_SERIALISED);							// Set flags so system does not update serial or program board
			SET_BIT(Board->BSR, BOARD_PROGRAMMED);
		}

		if (!READ_BIT(Board->BSR, BOARD_SERIALISED) || (TestRigMode == BatchMode)) {
			HAL_GPIO_WritePin(PIN2EN_GPIO_Port, PIN2EN_Pin, GPIO_PIN_RESET);
			TestRig_Init();
			PrintHomeScreen(Board);
			TargetBoardParamInit(0);										// Initialize targetboard variables, clear the board struct
			clearTestStatusLED();											// Clear previously set LEDs on the front panel
			Board->SerialNumber = read_serial();							// Read serial number in to be programmed into the board
			HAL_GPIO_WritePin(PIN2EN_GPIO_Port, PIN2EN_Pin, GPIO_PIN_SET);	// Enable power, to communicate with the board
		}
		if (Board->SerialNumber) {
			if (TestRigMode == SerialiseMode)
				CurrentState = csSerialise;									// Set the next state to serialise
			else
				CurrentState = csSolarCharger;								// Set the next state to test the solar charger
		}
		ProcessState = psInitalisation;
		break;
	case psFailed:
		retryCount++;
		if ((TestRigMode == BatchMode) && (retryCount > 2)) {
			HAL_GPIO_WritePin(PIN2EN_GPIO_Port, PIN2EN_Pin, GPIO_PIN_RESET);
			*State = psComplete;
		} else if (retryCount > 2) {
			if (Board->SerialNumber) {
				TargetBoardParamInit(0);
				Board->SerialNumber = 0x0;
				PrintHomeScreen(Board);
				TestRigMode = BatchMode;
			}
			*State = psWaiting;
		} else
			*State = psWaiting;

		break;
	}
}

void handleSolarCharger(TboardConfig *Board, TprocessState *State) {
	switch (*State) {
	case psInitalisation:
		if ((Board->BoardType == b402x || Board->BoardType == b427x) && !READ_BIT(Board->BVR, BOARD_SOLAR_STABLE)) {
			testSolarCharger();
			setTimeOut(1250);
			*State = psWaiting;
		} else {
			SET_BIT(Board->BVR, BOARD_SOLAR_STABLE);
			*State = psComplete;
		}
		break;
	case psWaiting:
		if (SolarChargerStable) {
			HAL_GPIO_WritePin(SOLAR_CH_EN_GPIO_Port, SOLAR_CH_EN_Pin, GPIO_PIN_RESET);
			SolarChargerSampling = SolarChargerStable = false;
			SolarChargerTimer = SolarChargerCounter = 0;
			printT((uns_ch*) "Solar Charger Stable...\n");
			SET_BIT(Board->BVR, BOARD_SOLAR_STABLE);
			*State = psComplete;
		} else if (!SolarChargerTimer)
			*State = psFailed;
		break;
	case psComplete:
		CurrentState = csInputVoltage;
		*State = psInitalisation;
		break;
	case psFailed:
		HAL_GPIO_WritePin(SOLAR_CH_EN_GPIO_Port, SOLAR_CH_EN_Pin, GPIO_PIN_RESET);
		SolarChargerSampling = SolarChargerStable = false;
		HAL_GPIO_WritePin(FAIL_GPIO_Port, FAIL_Pin, GPIO_PIN_SET);
		printT((uns_ch*) "Solar Charger Failure...\n");
		CLEAR_BIT(Board->BVR, BOARD_SOLAR_STABLE);
		CurrentState = csInputVoltage;
		*State = psInitalisation;
		break;
	}
}

void handleInputVoltage(TboardConfig *Board, TprocessState *State) {
	switch (*State) {
	case psInitalisation:
		if (!READ_BIT(Board->BVR, FUSE_V_STABLE)) {
			testInputVoltage();
			setTimeOut(1250);
			*State = psWaiting;
		} else
			*State = psComplete;
		break;
	case psWaiting:
		if (InputVoltageStable) {
			SET_BIT(Board->BVR, FUSE_V_STABLE);
			*State = psComplete;
		} else if (!InputVoltageTimer) {
			CLEAR_BIT(Board->BVR, FUSE_V_STABLE);
			*State = psComplete;
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
			if (Response == 0xCD)
				*State = psComplete;							// Board initialized correctly
			else
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
		} else {
			HAL_Delay(500);										//Allow board to reset, Required for electronic fuse
			CurrentState = csInterogating;
			*State = psInitalisation;
		}
		break;
	case psFailed:
		retryCount++;
		initialiseTargetBoard(Board);
		ProcessState = psWaiting;
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
				TestRig_MainMenu();
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
		retryCount = 0;
		HAL_GPIO_WritePin(TB_Reset_GPIO_Port, TB_Reset_Pin, GPIO_PIN_SET);
		SET_BIT(BoardConnected.BSR, BOARD_PROGRAMMED);
		printT((uns_ch*) "Programming Done\n");
		Close_File(&SDcard);
		SetSDclk(0);
		HAL_Delay(1200);	//Allow for board to power up
		LCD_ClearLine(3);
		LCD_ClearLine(4);
		CurrentState = csSerialise;
		*State = psInitalisation;
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
				for (uint8 i = 2; i < (Board->analogInputCount * sizeof(float) * 5); i+=4) {		// 5 different forms of calibration on each port
					memcpy(&CalibrationValue,&Data_Buffer[i], sizeof(float));				// IEEE 4 byte floating point values returned, check for 1.000 for uncalibrated ports
					if (CalibrationValue == 1.000)
						FailedCalibrationCount++;
				}
				if (FailedCalibrationCount > 2)												// If more than 2 ports are uncalibrated fail the calibration
					*State = psFailed;
				else
					*State = psComplete;
			} else
				*State = psFailed;
		} else if (BoardCommsReceiveState == RxBAD) {
			*State = psFailed;
		}
		break;
	case psComplete:
		SET_BIT(BoardConnected.BSR, BOARD_CALIBRATED);
		printT((uns_ch*) "=====     Board Calibrated     =====\n");
		CurrentState = csConfiguring;
		*State = psInitalisation;
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
			CurrentState = csInitialising;
			*State = psInitalisation;
		}
		break;

	case psFailed:
		if (!READ_BIT(BoardConnected.BSR, BOARD_PROGRAMMED) && (retryCount++ > 15)) {
			currentBoardConnected(&BoardConnected);
			LCD_ClearLine(1);
			sprintf((char*) &debugTransmitBuffer[0], "    Programming    ");
			LCD_displayString((uns_ch*) &debugTransmitBuffer[0], strlen((char*) debugTransmitBuffer));
			CurrentState = csProgramming;
			*State = psInitalisation;
		} else {
			BoardCommsReceiveState = RxWaiting;
			*State = psInitalisation;
		}
		break;
	}
}

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
				HAL_Delay(300);	//pause for board reset
				if (TestRigMode == VerboseMode)
					printT((uns_ch*) "===Board Configuration Successful===\n");
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
		CurrentState = csOutputTest;
		*State = psInitalisation;
		break;
	case psFailed:
		if (retryCount > 4) {
			CurrentState = csInterogating;
			*State = psInitalisation;
		} else {
			retryCount++;
			HAL_Delay(100);
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
		} else if (BoardCommsReceiveState == RxBAD){
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
		if (LatchCountTimer < latchCountTo) {
			if (READ_BIT(LatchTestStatusRegister, STABLE_INPUT_VOLTAGE)) {	//If input voltage is stable
				if (!READ_BIT(LatchTestStatusRegister, LATCH_ON_SAMPLING) && !READ_BIT(LatchTestStatusRegister, LATCH_ON_COMPLETE)) {
					SET_BIT(LatchTestStatusRegister, LATCH_ON_SAMPLING); //Begin Latch on sampling
					BoardCommsParameters[0] = LatchTestParam(LatchTestPort, 1);
					Data_Buffer[0] = 0;
					communication_array(0x26, &BoardCommsParameters[0], 1);
				} else if (READ_BIT(LatchTestStatusRegister, LATCH_ON_COMPLETE) && READ_BIT(LatchTestStatusRegister, LATCH_ON_SAMPLING)) {
					if (BoardCommsReceiveState != RxWaiting) { //Latch on sampling complete, reset voltage stability check
						if (BoardCommsReceiveState == RxGOOD) {
							Response = Data_Buffer[0];
							if (Response == 0x27) {
								CLEAR_BIT(LatchTestStatusRegister, LATCH_ON_SAMPLING);
								CLEAR_BIT(LatchTestStatusRegister, STABLE_INPUT_VOLTAGE);
								stableVoltageCount = 50;
							}
							BoardCommsReceiveState = RxWaiting;
						} else if (BoardCommsReceiveState == RxBAD)
							*State = psFailed;
					}
				} else if (READ_BIT(LatchTestStatusRegister,
						LATCH_ON_COMPLETE) && !READ_BIT(LatchTestStatusRegister, LATCH_OFF_SAMPLING) && !READ_BIT(LatchTestStatusRegister, LATCH_OFF_COMPLETE)) {
					SET_BIT(LatchTestStatusRegister, LATCH_OFF_SAMPLING); //Begin Latch off sampling
					BoardCommsParameters[0] = LatchTestParam(LatchTestPort, 0);
					Data_Buffer[0] = 0;
					communication_array(0x26, &BoardCommsParameters[0], 1);
				} else if (READ_BIT(LatchTestStatusRegister,
						LATCH_OFF_COMPLETE) && READ_BIT(LatchTestStatusRegister, LATCH_OFF_SAMPLING)) {
					if (BoardCommsReceiveState != RxWaiting) { //Latch off sampling complete, reset voltage stability check
						if (BoardCommsReceiveState == RxGOOD) {
							Response = Data_Buffer[0];
							if (Response == 0x27) {
								CLEAR_BIT(LatchTestStatusRegister, LATCH_ON_SAMPLING);
								CLEAR_BIT(LatchTestStatusRegister, STABLE_INPUT_VOLTAGE);
								stableVoltageCount = 50;
								runLatchTimeOut(2000);
							}
							BoardCommsReceiveState = RxWaiting;
						} else if (BoardCommsReceiveState == RxBAD)
							*State = psFailed;
					}
				}
				if (READ_BIT(LatchTestStatusRegister,
						LATCH_ON_COMPLETE) && READ_BIT(LatchTestStatusRegister, LATCH_OFF_COMPLETE))
					*State = psComplete;
			}
		} else
			*State = psComplete;
		if (!READ_BIT(LatchTestStatusRegister, LATCH_SAMPLING)) {
			*State = psComplete;
		}
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
				printT( (uns_ch*) "\n=======================          LATCH TEST PASSED         =======================\n\n");
				Board->TestResults[Board->GlobalTestNum][LatchTestPort*2] = 1000; 	// Base 1000 for easier sorting results															// Multiply by two to acount for 2ch of data
			} else {
				printLatchError(Board);
				printT(
						(uns_ch*) "\n=======================          LATCH TEST FAILED          =======================\n\n");
				Board->TestResults[Board->GlobalTestNum][LatchTestPort*2] = 0;
				if (Board->BoardType == b422x) { //TODO: Check if this needs to be here with code changes
					HAL_Delay(250);
					switch (LatchTestPort) {
					case Port_1:
						CLEAR_BIT(Board->TPR, TEST_ONE_PASSED);
						break;
					case Port_2:
						CLEAR_BIT(Board->TPR, TEST_TWO_PASSED);
						break;
					case Port_3:
						CLEAR_BIT(Board->TPR, TEST_THREE_PASSED);
						break;
					case Port_4:
						CLEAR_BIT(Board->TPR, TEST_FOUR_PASSED);
						break;
					}
				}
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
		if (retryCount > 2) {
			HAL_GPIO_WritePin(FAIL_GPIO_Port, FAIL_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(PIN2EN_GPIO_Port, PIN2EN_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(PIN5EN_GPIO_Port, PIN5EN_Pin, GPIO_PIN_RESET);
			CurrentState = csIDLE;
			ProcessState = psWaiting;
		} else {
			if (BoardCommsParameters[0] & 0x80) {
				communication_array(0x26, &BoardCommsParameters[0], 1);
			}
			retryCount++;
			*State = psInitalisation;
		}
		break;
	}
}

void handleAsyncTest(TboardConfig *Board, TprocessState *State) {
	switch (*State) {
	case psInitalisation:
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
		CurrentState = csSampling;
		*State = psInitalisation;
		break;
	case psFailed:
		retryCount++;
		HAL_GPIO_WritePin(FAIL_GPIO_Port, FAIL_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(PIN2EN_GPIO_Port, PIN2EN_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(PIN5EN_GPIO_Port, PIN5EN_Pin, GPIO_PIN_RESET);
		CurrentState = csIDLE;
		ProcessState = psWaiting;
		break;
	}
}

void handleSampling(TboardConfig *Board, TprocessState *State) {
	uns_ch Command;
	uns_ch Response;
	uint16 VuserSamples;
	uint8 Percentage;
	switch (*State) {
	case psInitalisation:
		LCD_printf((uns_ch*) "      Sampling      ", 2, 0);
		Command = 0x1A;
		SetPara(Board, Command);
		communication_array(Command, (uns_ch*) &BoardCommsParameters[0], BoardCommsParametersLength);
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
			Percentage = (uint8) ((sampleCount / (float) sampleTime) * 100);
			ProgressBar(Percentage);
		}

		if (samplesUploaded) {
			samplesUploading = false;
			samplesUploaded = false;
			*State = psComplete;
			BoardCommsReceiveState = RxWaiting;
		}
		break;
	case psComplete:
		if (VuserSamples < 100) {
			if (Board->GlobalTestNum == V_12output) {
				if ( (Board->BoardType == b401x) || (Board->BoardType == b402x) )
					ADC_Ch4sel();
				else
					ADC_Ch3sel();
			} else
				ADC_Ch5sel();
			HAL_ADC_Start(&hadc1);
			HAL_ADC_PollForConversion(&hadc1, 100);
			Vuser.avg_Buffer[VuserSamples] = HAL_ADC_GetValue(&hadc1);
			Vuser.total += Vuser.avg_Buffer[VuserSamples];
			VuserSamples++;
			HAL_ADC_Stop(&hadc1);
		}

		if (TestRigMode == VerboseMode) {
			printT((uns_ch*) "Samples Uploaded\n\n");
			printT((uns_ch*) "Requesting Results\n\n");
		}

		if (VuserSamples >= 100) {
			Vuser.total /=  VuserSamples;
			Vuser.average = Vuser.total * (15.25 / 4096);
			if (Board->GlobalTestNum <= V_trim)
				Board->VoltageBuffer[Board->GlobalTestNum] = Vuser.average;
			sampleCount = 0;
			Vuser.total = 0;
			if (Board->VoltageBuffer[Board->GlobalTestNum] > 0) {
				*State = psInitalisation;
				CurrentState = csUploading;
			} else
				VuserSamples = 0;
		}
		break;
	case psFailed:
		if (retryCount > 2) {
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

void handleUploading(TboardConfig *Board, TprocessState *State) {
	uns_ch Response;
	switch (*State) {
	case psInitalisation:
		LCD_printf((uns_ch*) "     Uploading      ", 2, 0);
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
				*State = psWaiting;
				CurrentState = csIDLE;
			}
		}
		break;
	case psFailed:
		retryCount++;
		HAL_GPIO_WritePin(FAIL_GPIO_Port, FAIL_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(PIN2EN_GPIO_Port, PIN2EN_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(PIN5EN_GPIO_Port, PIN5EN_Pin, GPIO_PIN_RESET);
		CurrentState = csIDLE;
		*State = psWaiting;
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
			if (Response == 0x11) {
				//Load Current Serial Number
				memcpy(&CurrentSerial, &tempSerial, 4);
				//Write New Serial Number
				Command = 0x12;
				communication_arraySerial(Command, CurrentSerial, Board->SerialNumber);
				*State = psInitalisation;
			} else if (Response == 0x13) {
				if (tempSerial != Board->SerialNumber) {
					Command = 0x10;
					communication_arraySerial(Command, 0, 0);
					*State = psInitalisation;
				} else {
					SET_BIT(Board->BSR, BOARD_SERIALISED);
					*State = psComplete;
				}
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
		if (retryCount >= 10) {
			HAL_GPIO_WritePin(FAIL_GPIO_Port, FAIL_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(PIN2EN_GPIO_Port, PIN2EN_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(PIN5EN_GPIO_Port, PIN5EN_Pin, GPIO_PIN_RESET);
			CurrentState = csIDLE;
		}
		*State = psInitalisation;
		break;
	}
}

