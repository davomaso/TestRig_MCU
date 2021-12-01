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
		HAL_GPIO_WritePin(PIN2EN_GPIO_Port, PIN2EN_Pin, GPIO_PIN_RESET);		// Reset power to board
		HAL_GPIO_WritePin(PIN5EN_GPIO_Port, PIN5EN_Pin, GPIO_PIN_RESET);
		TargetBoardParamInit(true);												// Fully erase BoardConnected struct and associated memory
		*State = psWaiting;
		break;
	case psWaiting:
		BoardResetTimer = 500;													// Allow short time for reset
		SetBoardType(Board, LoomState);											// Set board type depending on loom connected
		*State = psComplete;
		break;
	case psComplete:
		if (BoardResetTimer == 0)
			currentBoardConnected(Board);										// Set test struct with the boardtype variable
		CurrentState = csIDLE;													// Return to the IDLE state when a loom is connected
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
		PrintHomeScreen(Board);													// Print Home screen when the system returns to default case
		*State = psWaiting;
		break;
	case psWaiting:
		if (Board->BoardType && BoardResetTimer == 0) {
			PreviousSerialNumber = Board->SerialNumber;							// Store current serial number
			interrogateTargetBoard(); 											// Find if valid board is connected/determine serial number
			while ((BoardCommsReceiveState == RxWaiting) && (*State == psWaiting)) {
				if (KP[1].Pressed || KP[3].Pressed || KP[6].Pressed || KP[hash].Pressed || KP[star].Pressed) // button pressed start procedure associated with that button
					break;
			}
			if (BoardCommsReceiveState == RxGOOD) {								// Comms good, update home screen
				communication_response(Board, &Data_Buffer[4], &Data_Buffer[5], strlen((char*) &Data_Buffer) - 5);
			} else if (*State == psFailed) {
				Board->SerialNumber = 0;										// If comms are removed return the system to batch mode ready for another board/Serial input
				TestRigMode = BatchMode;
			}
			if ((PreviousSerialNumber != Board->SerialNumber))
				PrintHomeScreen(Board);											// When previous serial number doesnt match current re print home screen
			if ((Board->SerialNumber == 0) && (TestRigMode == BatchMode)) { 	// No serial number and in batch mode jump to the serial number entry to begin testing
				PrintHomeScreen(Board);
				CLEAR_BIT(Board->BSR, BOARD_SERIALISED); 						// Clear serialised reg if no serial number present and in batch mode
				*State = psComplete;
			}

			if (KP[1].Pressed || KP[3].Pressed || KP[6].Pressed || KP[hash].Pressed) {
				TestRig_Init();
				TargetBoardParamInit(0);										// Reinitialise variables
				clearTestStatusLED();											// remove status led if it is enabled
				if (KP[1].Pressed) {
					TestRigMode = OldBoardMode;									// Test only, no serialise, no program
					SET_BIT(Board->BSR, BOARD_SERIALISED);						// Set flags so system does not update serial or program board
					SET_BIT(Board->BSR, BOARD_PROGRAMMED);
				} else if (KP[hash].Pressed) {
					if (Board->SerialNumber != 0 && (~Board->SerialNumber != 0))// Check serial number is not 0 or 0xFFFFFFFF
						SET_BIT(Board->BSR, BOARD_SERIALISED);
					TestRigMode = BatchMode;									// Program immediately
				} else if (KP[6].Pressed)
					TestRigMode = VerboseMode;									// Display everything to terminal
				else if (KP[3].Pressed) {
					TestRigMode = SerialiseMode;								// Update serial number only
					CLEAR_BIT(Board->BSR, BOARD_SERIALISED);
				}

				KP[1].Pressed = KP[3].Pressed = KP[6].Pressed = KP[hash].Pressed = false;
				LCD_Clear();													// Clear LCD screen
				*State = psComplete;											// Progress with the testing process
			}
		}
		//TestRig_Calibration Routine
		if ((KP[7].Pressed && KP[9].Pressed)) {									// Calibrate Test Rig
			KP[7].Pressed = KP[9].Pressed = false;
			TestRig_Calibration();
			LCD_Clear();
			TestRig_Init();														// Return to initial state
			printT((uns_ch*) "\n\n==========  Test Rig  ==========\n");
		}
		break;
	case psComplete:
		if (!READ_BIT(Board->BSR, BOARD_SERIALISED))
			CurrentState = csSerialNumberEntry;									// If board is not serialised enter serial number
		else
			CurrentState = csSolarCharger;										// Set the next state to test the solar charger
		*State = psInitalisation;
		break;
	case psFailed:
		retryCount++;
		if ((TestRigMode == BatchMode) && (retryCount > 2)) {					// After two failed retries in batch mode disable power to board
//			HAL_GPIO_WritePin(PIN2EN_GPIO_Port, PIN2EN_Pin, GPIO_PIN_RESET);
//			*State = psComplete;
			*State = psInitalisation;
			CurrentState = csSerialNumberEntry;									// TODO: Changed to serial number entry to see if IDLE bug is removed
		} else if (retryCount > 2) {											// After two retries return the system to initialisation in batchmode for new testing to begin
			if (Board->SerialNumber) {											// Check if the board wass serialised
				TargetBoardParamInit(0);										// Erase most data associated to board, keep boardtype
				Board->SerialNumber = 0x0;										// Erase serial number
//				PrintHomeScreen(Board);											// Print the homescreen // Not 100% required as serial entry should update the screen
				TestRigMode = BatchMode;										// Put board into batch mode, to allow for multiple boards to be programmed
			}
			CurrentState = csSerialNumberEntry;									// Return to serial entry to begin new test on new board
			*State = psInitalisation;
		} else
			*State = psWaiting;													// Put the board back into waiting state if the retry count was less than 3

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
		TargetBoardParamInit(0);										// Initialize targetboard variables, clear the board struct
		Board->SerialNumber = 0;
		clearTestStatusLED();											// Clear previously set LEDs on the front panel
		LCD_printf((uns_ch*) "Enter Serial Number", 2, 0);				// Print serial entry home screen
		LCD_printf((uns_ch*) " * - Esc    # - Ent ", 4, 0);
		bzero(&SerialNumber, 9);										// Set serial number to zero
//		memset(&SerialNumber, 0, 9);
		LCD_ClearLine(3);												// Clear line 3 of LCD for serial entry to be placed on
		LCD_setCursor(3, 0);											// Set LCD cursor to start of line 3
		LCD_CursorOn_Off(true);											// Enable cursor if user is going to use keypad
		SerialCount = 0;												// return the serial entry count to zero
		USART3->CR1 |= (USART_CR1_RXNEIE);								// Enable Receive interupt for the barcode scanner
		tempdata = 0;
		*State = psWaiting;
		break;
	case psWaiting:
		tempdata = ScanKeypad(); //value returned in ascii				// scan keypad to see if character has been entered storing the returned value to tempdata
		if (tempdata) {
			SerialNumber[SerialCount++] = tempdata;						// populate the serialnumber array with the returned value
			if (SerialCount >= 9) {										// shift the data down if length is greater than 9 bytes
				memmove(&SerialNumber[0], &SerialNumber[1], 8);
				SerialCount--;
			}
			LCD_printf((uns_ch*) " * - BckSpc # - Ent", 4, 0);			// Print back space if serial number is greater than 0
			LCD_ClearLine(3);
			LCD_setCursor(3, 0);
			LCD_displayString(&SerialNumber[0], SerialCount);
		}
		if (SerialCount)												// disable quit enabled if serial number is present
			QuitEnabled = false;

		if (KP[star].Pressed) {											// Handle backspace or quit
			KP[star].Pressed = false;
			if (SerialCount) {											// if serial number populated
				SerialNumber[SerialCount--] = 0x08;						// replace the charcter with a space
				LCD_setCursor(3, SerialCount + 1);						// Set the cursor to the last place
				sprintf((char*) &debugTransmitBuffer[0], "  ");			// populate the space in the string
				LCD_displayString((uns_ch*) &debugTransmitBuffer[0], strlen((char*) debugTransmitBuffer));
				LCD_setCursor(3, SerialCount + 1);						// After deleting the character, go back one space for next character to be entered
				sprintf((char*) &debugTransmitBuffer[0], "\x8 \x8");	// backspace-space-backspace remove last character
				printT(&debugTransmitBuffer[0]);						// print to terminal
				if (SerialCount == 0)
					LCD_printf((uns_ch*) " * - Esc    # - Ent ", 4, 0); // if serial count is zero then esc
				LCD_setCursor(3, SerialCount + 1);						// return cursor to the last position
			} else {
				CurrentState = csIDLE;									// return to idle state if serial count is zero
				*State = psInitalisation;
				QuitEnabled = true;
			}
		}
		if (KP[hash].Pressed) {											// If hash is pressed begin testing process
			KP[hash].Pressed = false;
			Board->SerialNumber = 0;									// set board serial number to zero
			printT((uns_ch*) &SerialNumber);							// print the serial number that was entered to terminal
			LCD_printf((uns_ch*) "Serial Number:", 2, 0);				// Print the serial number entered on line 2 to let the user know serial was entered correctly
			uint8 i = 0;
			while (SerialCount != i) {
				Board->SerialNumber = (Board->SerialNumber * 10 + (SerialNumber[i++] - 0x30));	// Change the serial number from ascii to uint32
			}
			HAL_GPIO_WritePin(PIN2EN_GPIO_Port, PIN2EN_Pin, GPIO_PIN_SET);		// Enable power to board
			LCD_CursorOn_Off(false);									// Disable the cursor
			LCD_ClearLine(4);											// Clear line 4
			QuitEnabled = true;											// renable quit flap
			BoardResetTimer = 2000;										// set board reset timer to 2000
			*State = psComplete;										// Set state to complete
		}

		// Barcode Scanned
		if (BarcodeScanned == true) {									// If serial number is entered through the barcode scanner
			BarcodeScanned = false;										// disable the barcode scanned flag
			Board->SerialNumber = 0;
			LCD_ClearLine(3);											// Clear line 3 to reprint the serial number entered
			LCD_printf((uns_ch*) "Serial Number:", 2, 0);
			LCD_printf((uns_ch*) &BarcodeBuffer, 3, 0);
			printT((uns_ch*) &BarcodeBuffer);
			uint8 i = 0;
			while (BarcodeCount != i) {									// Convert barcode serial entry from ascii to uint32
				Board->SerialNumber = (Board->SerialNumber * 10 + (BarcodeBuffer[i++] - 0x30));
			}
			SerialCount = BarcodeCount;									// Set the serialcount to the barcode count
			BarcodeCount = 0;
			USART3->CR1 &= ~(USART_CR1_RXNEIE);							// Disable barcode receive interrupt
			USART2->CR1 |= (USART_CR1_RXNEIE);
			HAL_GPIO_WritePin(PIN2EN_GPIO_Port, PIN2EN_Pin, GPIO_PIN_SET);// reenable power to the target board
			LCD_CursorOn_Off(false);									// Disable the cursor
			LCD_ClearLine(4);											// Clear the last line
			QuitEnabled = true;											// reenable the ability to quit
			BoardResetTimer = 2000;										// 2s board reset time to allow power to stabilise
			*State = psComplete;
			break;
		}
		break;
	case psComplete:
		if (BoardResetTimer == 0) {
			LCD_ClearLine(3);											// Clear line 3 before moving to solar charger or serialising
			if (TestRigMode == BatchMode)								// If in batch mode move to solar charger state
				CurrentState = csSolarCharger;
			else
				CurrentState = csSerialise;								// Serialise board
			*State = psInitalisation;
		}
		break;
	case psFailed:
		retryCount++;													// attempt to retry serial entry 4 times, then return to IDLE
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
			testSolarCharger();											// Begin solar charger testing
			*State = psWaiting;											// Move to waiting once solar testing begins
		} else {
			SET_BIT(Board->BVR, SOLAR_V_STABLE);						// If board isnt 4270 or 4020 move to next state and enable the solar charger stable bit
			*State = psComplete;
		}
		break;
	case psWaiting:
		if (SolarChargerStable) {										// Once solar charger stable established
			SolarChargerSampling = SolarChargerStable = false;			// Disable solar charger sampling amd stable flags
			SolarChargerTimer = SolarChargerCounter = 0;				// Reset the timer and counter to zero
			printT((uns_ch*) "Solar Charger Stable...\n");
			SET_BIT(Board->BVR, SOLAR_V_STABLE);						// Set bit if stable
			*State = psComplete;										// Move to complete state
		} else if (!SolarChargerTimer)									// If timer reaches zero, then move to fail state
			*State = psFailed;
		break;
	case psComplete:
		HAL_GPIO_WritePin(SOLAR_CH_EN_GPIO_Port, SOLAR_CH_EN_Pin, GPIO_PIN_RESET);		// Remove 16v on solar charger connecter, switch relay to original state
		CurrentState = csInputVoltage;									// Test input voltage
		*State = psInitalisation;
		break;
	case psFailed:
		HAL_GPIO_WritePin(SOLAR_CH_EN_GPIO_Port, SOLAR_CH_EN_Pin, GPIO_PIN_RESET);		// Remove 16v on solar charger connecter, switch relay to original state
		SolarChargerSampling = SolarChargerStable = false;				// Disable solar charger sampling amd stable flags
		HAL_GPIO_WritePin(FAIL_GPIO_Port, FAIL_Pin, GPIO_PIN_SET);		// Set fail LED
		printT((uns_ch*) "Solar Charger Failure...\n");
		CLEAR_BIT(Board->BVR, SOLAR_V_STABLE);							// Clear solar charger stable flag
		CurrentState = csInputVoltage;									// Move to input voltage test
		*State = psInitalisation;
		break;
	}
}

void handleInputVoltage(TboardConfig *Board, TprocessState *State) {
	switch (*State) {
	case psInitalisation:
		if (!READ_BIT(Board->BVR, INPUT_V_STABLE)) {					// Check if input voltage stable bit set
			testInputVoltage();											// Begin testing the input voltage
			*State = psWaiting;
		} else
			*State = psComplete;										// Move to complete if the flag is already set

		sprintf((char*) &lcdBuffer[0], "%x", Board->BoardType);
		LCD_printf((uns_ch*) &lcdBuffer[0], 1, 0);						// Print board type in top left corner of LCD
		sprintf((char*) &lcdBuffer, "S/N: %lu", Board->SerialNumber);
		LCD_setCursor(1, (21 - strlen((char*) &lcdBuffer)));			// Print the serial number in the top right corner of the LCD moving the serial number back the lenght of it
		LCD_displayString((uns_ch*) &lcdBuffer[0], strlen((char*) &lcdBuffer));
		break;
	case psWaiting:
		if (BoardResetTimer == 0 && !InputVoltageSampling) {			// if reset timer is zero and not input voltage sampling, populate the timer, reset counter and enable the sampling flag
			InputVoltageSampling = true;
			InputVoltageTimer = 2000;
			InputVoltageCounter = 0;
		}
		if (InputVoltageSampling) {
			if (InputVoltageStable) {									// Once sampling voltage is stable or timer times out
				SET_BIT(Board->BVR, INPUT_V_STABLE);					// Enable the input voltage stable flag
				*State = psComplete;									// move to complete
			} else if (!InputVoltageTimer) {
				CLEAR_BIT(Board->BVR, INPUT_V_STABLE);					// Disable the voltage stable flag if the conditions were not met
				*State = psComplete;									// move to complete
			}
		}
		break;
	case psComplete:
		InputVoltageSampling = InputVoltageStable = false;				// disable flags and reset timers
		InputVoltageTimer = InputVoltageCounter = 0;
		printT((uns_ch*) "Input Voltage Stable...\n");					// print status to terminal
		if (READ_BIT(Board->BSR, BOARD_PROGRAMMED))
			CurrentState = csSerialise;									// if the board programmed flag is set move to serialising else program the target board
		else
			CurrentState = csProgramming;
		*State = psInitalisation;
		break;
	case psFailed:
		InputVoltageSampling = InputVoltageStable = false;				// Disable voltage sampling flags
		HAL_GPIO_WritePin(FAIL_GPIO_Port, FAIL_Pin, GPIO_PIN_SET);		// Enable the failed test LED
		printT((uns_ch*) "Input Voltage Failure...\n");
		CurrentState = csIDLE;											// Return to idle state
		*State = psInitalisation;
		break;
	}
}

void handleInitialising(TboardConfig *Board, TprocessState *State) {
	uint8 Response;
	switch (*State) {
	case psInitalisation:
		if (READ_BIT(Board->BSR, BOARD_TEST_PASSED) || !READ_BIT(Board->BSR, BOARD_INITIALISED)) {	// test passed or hasnt been initialised
			initialiseTargetBoard(Board);								// Initialize the target board
			*State = psWaiting;
		} else {
			*State = psComplete;										// Move to complete if board has previously been initialised
		}
		break;
	case psWaiting:
		if (BoardCommsReceiveState == RxGOOD) {
			Response = Data_Buffer[0];
			if (Response == 0xCD) {
				BoardResetTimer = 500;									//Allow board to reset, Required for electronic fuse
				*State = psComplete;									// Board initialized correctly
			} else
				*State = psFailed;										// Initialization failed
		} else if (BoardCommsReceiveState == RxBAD) {
			*State = psFailed;											// Board comms failed
		}
		break;
	case psComplete:
		SET_BIT(Board->BSR, BOARD_INITIALISED);
		if (READ_BIT(Board->BSR, BOARD_TEST_PASSED)) {			// If testing is complete re-run initialization
			TestComplete(Board);								// Set pass LED
			CurrentState = csIDLE;								// Move to idle state if board test complete and board passed
			*State = psInitalisation;
		} else if (BoardResetTimer == 0) {
			CurrentState = csInterogating;						// Once board reset move to interogating
			*State = psInitalisation;
		}
		break;
	case psFailed:
		retryCount++;
		if (retryCount > 2) {
			CurrentState = csIDLE;
		}
		*State = psInitalisation;
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
			SPI3->CR1 |= (0xFF & SPI_BAUDRATEPRESCALER_256);						// Ensure the baud rate is set to slower speed to begin communications with board

			HAL_GPIO_WritePin(TB_Reset_GPIO_Port, TB_Reset_Pin, GPIO_PIN_RESET);	// Pull the target board reset low
			HAL_Delay(20);															// Wait 20ms for target board to allow to be programmed
			SET_BIT(Board->BPR, PROG_INITIALISED);
			response[2] = 0xFF;														// Set response to 0xFF to ensure programming enabled can be successfully confirmed
		} else if (!READ_BIT(Board->BPR, PROG_ENABLED)) {
			data[0] = 0xAC;															// enable programming string
			data[1] = 0x53;
			data[2] = 0x00;
			data[3] = 0x00;
			HAL_SPI_TransmitReceive(&hspi3, &data[0], &response[0], 4, HAL_MAX_DELAY);
			HAL_Delay(20);
			if (response[2] == 0x53)
				SET_BIT(Board->BPR, PROG_ENABLED);									// Set the board programming enabled bit
			else {
				*State = psFailed;
						// TODO: Inform the user to connect the programming lead
			}
		} else if (!READ_BIT(Board->BPR, CHIP_ERASED)) {
			data[0] = 0xAC;															//	Chip Erase string
			data[1] = 0x80;
			data[2] = 0x00;
			data[3] = 0x00;
			HAL_SPI_Transmit(&hspi3, &data[0], 4, HAL_MAX_DELAY);
			HAL_Delay(10);															// Chip erase delay
			SET_BIT(Board->BPR, CHIP_ERASED);										// Set board erased bit
		} else if (!READ_BIT(Board->BPR, CLOCK_SET)) {
			PollReady();															//Poll RDY
			data[0] = 0xAC;															//Set Clk to 8Mhz
			data[1] = 0xA0;															// 0xA0 Low byte command
			data[2] = 0x00;
			data[3] = HIGH_CLK_LOW_FUSE;											//Fuse Low Byte
			HAL_SPI_TransmitReceive(&hspi3, &data[0], &response[0], 4, HAL_MAX_DELAY);
			HAL_Delay(20);
			data[1] = 0xA8;															// 0xA8 High byte command
			data[3] = HIGH_CLK_HIGH_FUSE;											//Fuse High Byte
			HAL_SPI_TransmitReceive(&hspi3, &data[0], &response[0], 4, HAL_MAX_DELAY);
			HAL_Delay(20);
			data[1] = 0xA4;															// 0xA4 Ext byte command
			data[3] = HIGH_CLK_EXT_FUSE;											//Fuse Extended Byte
			HAL_SPI_TransmitReceive(&hspi3, &data[0], &response[0], 4, HAL_MAX_DELAY);

			HAL_Delay(20);
			SPI3->CR1 &= ~(SPI_BAUDRATEPRESCALER_256);
			SPI3->CR1 |= (0xFF & SPI_BAUDRATEPRESCALER_32);							// Move baud rate to higher speed, then reset board to allow communications again
			ProgrammingCount = 0;													// Reset the count to zero
			HAL_GPIO_WritePin(TB_Reset_GPIO_Port, TB_Reset_Pin, GPIO_PIN_SET);		// Pull target board reset high to ensure fuse bytes are set
			SET_BIT(Board->BPR, CLOCK_SET);											// Set clk set bit
		} else if (!READ_BIT(Board->BPR, FUSE_VALIDATED)) {
			EnableProgramming();													// Enable programming
			// Read Fuse Bits
			data[0] = 0x50;															//	Read Low Byte
			data[1] = 0x00;
			data[2] = 0x00;
			data[3] = 0x00;
			HAL_SPI_TransmitReceive(&hspi3, &data[0], &response[0], 4, HAL_MAX_DELAY);
			if (response[3] == HIGH_CLK_LOW_FUSE) {									// Read high byte
				data[1] = 0x08;
				HAL_SPI_TransmitReceive(&hspi3, &data[0], &response[0], 4, HAL_MAX_DELAY);
				if (response[3] == HIGH_CLK_EXT_FUSE) {
					data[0] = 0x58;													// Read Ext Byte
					HAL_SPI_TransmitReceive(&hspi3, &data[0], &response[0], 4, HAL_MAX_DELAY);
					if (response[3] == HIGH_CLK_HIGH_FUSE)
						SET_BIT(Board->BPR, FUSE_VALIDATED);						// Set fuse bytes validated bit
				}
			}
		} else if (!READ_BIT(Board->BPR, FILE_FOUND_OPEN)) {
			if (FindBoardFile(Board, &SDcard)) {									//Find file
				res = OpenFile(&SDcard);											// Open file from SDcard
				if (res != FR_OK) {													// If file isnt found set state to failed
					*State = psFailed;
					break;
				}
				LCD_printf((uns_ch*) "    Programming    ", 2, 0);
				ProgressBarTarget = round((float) ((fileSize / 2) / MAX_PAGE_LENGTH) * 0.7);	// Determine the progress bar length
				SetSDclk(1);														// Speed up SDcard clk for faster programming
				page = PageBufferPosition = LineBufferCount = 0;					// Initialisation page, page and line position variables
				CLEAR_REG(Board->BPR);												// Clear the board progress register
				*State = psWaiting;
			} else {
				printT((uns_ch*) "Could not find hex file...\n");
//				CurrentState = csProgramming;										// TODO: Line possibly not needed
				*State = psFailed;
			}
		}
		break;
	case psWaiting:
		if (f_gets((TCHAR*) &tempLine, sizeof(tempLine), &(SDcard.file))) {			// Get next line in file
			sortLine((uns_ch*) &tempLine[0], &LineBuffer[0], &LineBufferCount);		// Sort the line to remove the colon and other unwanted data from the intel hex format
			if (populatePageBuffer(&PageBuffer[PageBufferPosition], &PageBufferPosition, (uns_ch*) &LineBuffer,
					&LineBufferCount))												// Populate page buffer, return true if page ready to be written
				SET_BIT(Board->BPR, PAGE_WRITE_READY);
		} else {
			while (PageBufferPosition < MAX_PAGE_LENGTH) {							// When the EOF is reached fill the rest of the page buffer with 0xFFs
				PageBuffer[PageBufferPosition++] = 0xFF;
			}
			SET_BIT(Board->BPR, PAGE_WRITE_READY);									// Enable the page write ready and final page write flags
			SET_BIT(Board->BPR, FINAL_PAGE_WRITE);
		}

		if (READ_BIT(Board->BPR, PAGE_WRITE_READY)) {								// Write page to board
			uns_ch data[4];
			if (page == 0) {														// if initial page
				data[0] = 0x4D;														// begin programming command
				data[1] = 0x00;
				data[2] = 0x00;
				data[3] = 0x00;
				HAL_SPI_Transmit(&hspi3, (uint8_t*) &data, 4, HAL_MAX_DELAY);
			}
			PageWrite(&PageBuffer[0], MAX_PAGE_LENGTH / 2, page);					// write page
			if (!VerifyPage(page, &PageBuffer[0])) {								// check verification to see if board programmed successfully
				PageWrite(&PageBuffer[0], MAX_PAGE_LENGTH / 2, page);
				if (!VerifyPage(page, &PageBuffer[0])) {
					*State = psFailed;												// fail board if the verification fails twice
				}
			}
			CLEAR_BIT(Board->BPR, PAGE_WRITE_READY);								// clear page write ready bit for next page to be written
			PageBufferPosition = 0;													// Reset the position
			if (READ_BIT(Board->BPR, FINAL_PAGE_WRITE)) {							// if final page write
				*State = psComplete;
				PageBufferPosition = page = 0;
				SPI3->CR1 &= ~(SPI_BAUDRATEPRESCALER_32);							// set the baud rate back to the slower speed
				SPI3->CR1 |= (0xFF & SPI_BAUDRATEPRESCALER_256);
				HAL_Delay(10);														// 10ms delay
				SetClkAndLck(Board);												// Set fuse and clk bits back to correct
				printT((uns_ch*) "Programming Done\n");
				HAL_GPIO_WritePin(TB_Reset_GPIO_Port, TB_Reset_Pin, GPIO_PIN_SET);	// Pull reset high
				BoardResetTimer = 1500;												// Allow for board to power up
			} else {
				if (LineBufferCount)												// Check if data is remaining in the line buffer
					populatePageBuffer(&PageBuffer[PageBufferPosition], &PageBufferPosition, &LineBuffer[0],
							&LineBufferCount);										// Populate the page with the rest of the data
				Percentage = (uint8) ((page / (float) ProgressBarTarget) * 100);	// update percentage for the progress bar
				ProgressBar(Percentage);											// Write progress bar
			}
			page++; 																//increment page counter
		}
		break;
	case psComplete:
		if (BoardResetTimer == 0) {													// Once reset timer is 0
			retryCount = 0;															// Set retry count to 0
			SET_BIT(BoardConnected.BSR, BOARD_PROGRAMMED);							// Set the board programmed bit following successful programming
			Close_File(&SDcard);													// Close the file
			SetSDclk(0);															// Set the SD card clk speed to the slower speed
			LCD_ClearLine(3);														// Clear the bottom two lines
			LCD_ClearLine(4);
			CurrentState = csSerialise;												// Move to serialisation state
			*State = psInitalisation;
		}
		break;
	case psFailed:
		printT((uns_ch*) "Programming Failed\n");
		Close_File(&SDcard);														// Close file upon failure
		retryCount++;																// increase retry count
		HAL_GPIO_WritePin(TB_Reset_GPIO_Port, TB_Reset_Pin, GPIO_PIN_SET);			// Pull target board reset high
		*State = psInitalisation;													// return to initialisation
		if (retryCount >= 3) {														// if retry count greater than 3 return to IDLE state
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
			*State = psComplete;													// Complete if board is already calibrated or board is the 4220
		} else {
//			sprintf((char*) &lcdBuffer, "    Calibrating");
//			LCD_printf((uns_ch*) &lcdBuffer, 2, 0);
			CLEAR_REG(CalibrationStatusRegister);									// Clear the calibration status register
			CalibratingTimer = 0;													// initialise calibration timer
			TargetBoardCalibration_Voltage(Board);									// Begin target board calibration
			calibrateTargetBoard(Board);
			*State = psWaiting;
		}
		break;
	case psWaiting:
		if (BoardCommsReceiveState == RxGOOD) {										// Board comms receive state good, successful transmission of data
			Response = Data_Buffer[0];
			if (Response == 0xC1) {													// Check the response is the calibration command
				uint8 FailedCalibrationCount = 0;									// initialise the failed calibration count
				float CalibrationValue;
				for (uint8 i = 2; i < (Board->analogInputCount * sizeof(float) * 5); i += 4) { // 5 different forms of calibration on each port
					memcpy(&CalibrationValue, &Data_Buffer[i], sizeof(float)); 		// IEEE 4 byte floating point values returned, check for 1.000 for uncalibrated ports
					if (CalibrationValue == 1.000)
						FailedCalibrationCount++;									// increse the failed count
				}
				if (FailedCalibrationCount > 2)										// If more than 2 ports are uncalibrated fail the calibration
					*State = psFailed;
				else {
					BoardResetTimer = 300;											// 300ms for stablisation following calibration
					*State = psComplete;
				}
			} else
				*State = psFailed;
		} else if (BoardCommsReceiveState == RxBAD) {								// fail on bad comms
			*State = psFailed;
		}
		break;
	case psComplete:
		if (BoardResetTimer == 0) {													// once stable set calibrated bit and move to configuration
			SET_BIT(BoardConnected.BSR, BOARD_CALIBRATED);
			printT((uns_ch*) "=====     Board Calibrated     =====\n");
			CurrentState = csConfiguring;
			*State = psInitalisation;
		}
		break;
	case psFailed:
		retryCount++;																// Increase retry count
		if (retryCount < 3) {														// retry count greater than 4 retturn to IDLE
			printT((uns_ch*) "TestRig_Calibration Failed Recalibrating Device\n");
			CLEAR_REG(CalibrationStatusRegister);									// Clear calibration status register
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
		interrogateTargetBoard();													// interogate target board
		*State = psWaiting;
		break;
	case psWaiting:
		if (BoardCommsReceiveState == RxGOOD) {										// Comm state good
			Response = Data_Buffer[4];
			if (Response == 0x11)													// response is interogate command
				*State = psComplete;
			else
				*State = psFailed;
		} else if (BoardCommsReceiveState == RxBAD) {								// comms bad
			*State = psFailed;
		}
		break;
	case psComplete:
		if (READ_BIT(Board->BSR, BOARD_CALIBRATED)) {								// Check if calibrated
			communication_response(Board, &Response, &Data_Buffer[5], strlen((char*) Data_Buffer));
			CurrentState = csConfiguring;											// Move to configuring
			*State = psInitalisation;
		} else if (READ_BIT(Board->BSR, BOARD_INITIALISED)) {						// Check if initialised
			communication_response(Board, &Response, &Data_Buffer[5], strlen((char*) Data_Buffer));
			retryCount = 0;
			CurrentState = csCalibrating;											// moved to calibrating
			*State = psInitalisation;
		} else {
			communication_response(Board, &Response, &Data_Buffer[5], strlen((char*) Data_Buffer));
			currentBoardConnected(Board);											// Populate current board connected
			if (Board->Version > getCurrentVersion(Board->BoardType)) {				// If files on the SD card are outdated warn the user.
				LCD_printf((uns_ch*) "Update Firmware", 2, 0);
				LCD_printf((uns_ch*) "Files Immediately", 3, 0);
				while (!KP[hash].Pressed) {											// wait for hash to be pressed if files are outdated

				}
				LCD_Clear();
			}
			CurrentState = csInitialising;											// Initialise board if it hasnt been
			*State = psInitalisation;
		}
		break;

	case psFailed:
		if (retryCount > 3) {														// retry 4 times
			if (!READ_BIT(BoardConnected.BSR, BOARD_PROGRAMMED)) {					// else move to programmming
				currentBoardConnected(&BoardConnected);
				CurrentState = csProgramming;
			} else
				CurrentState = csIDLE;												// move to idle if board was previously programmed
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
		if (Board->BoardType == b422x) {											//Given all ports on 4220 are latch ports just skip to latch test
			TestFunction(Board);
			*State = psComplete;													// run test function to establish CHval then move state to complete
		} else
			*State = psWaiting;
		break;
	case psWaiting:
		if (BoardCommsReceiveState == RxGOOD) {										// comms good
			if (!OutputsSet)														//only set the outputs once
				TestFunction(Board);												// run test function to establish CHval
			Response = Data_Buffer[0];												// repsponse check for configuration command
			if (Response == 0x57) {
				if (TestRigMode == VerboseMode)
					printT((uns_ch*) "===Board Configuration Successful===\n");
				BoardResetTimer = 500;												// pause for board reset
				*State = psComplete;												// move to complete
			} else
				*State = psFailed;													// failed if response isnt configuration command
		} else if (BoardCommsReceiveState == RxBAD) {								// comms bad move to failed
			*State = psFailed;
		} else if (!OutputsSet) {													// if outputs not set, set the outputs, while waiting for response to save time
			TestFunction(Board);
		}
		break;
	case psComplete:
		if (BoardResetTimer == 0) {													// move to output test if reset is zero
			CurrentState = csOutputTest;
			*State = psInitalisation;
		}
		break;
	case psFailed:
		if (retryCount > 4) {
			CurrentState = csInterogating;											// Move to interrogating following 4 failed retries
			*State = psInitalisation;
		} else {
			retryCount++;
			configureTargetBoard(Board);											// Reattempt to configure board if failed
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
		if (RelayPort_Enabled) {  													// Output test for the 402x Boards, tests port 10 pulls low
			BoardCommsParameters[0] = 0x89;											// TODO: Change this to a static array and memcpy into boardcomms parameters
			BoardCommsParameters[1] = 0x05;
			BoardCommsParameters[2] = 0x00;
			BoardCommsParameters[3] = 0x00;
			BoardCommsParameters[4] = 0x00;
			BoardCommsParameters[5] = 0x05;
			BoardCommsParameters[6] = 0x00;
			communication_array(0x26, &BoardCommsParameters[0], 5);
			RelayCount = 125;														// Up down relay count to establish stability on the output relay port, up for stable
			*State = psWaiting;
		} else
			*State = psComplete;													// Move to complete if the relay port is not enabled
		break;
	case psWaiting:
		if (BoardCommsReceiveState == RxGOOD) {										// Receive state set to good
			Response = Data_Buffer[0];
			if (Response == 0x27) {													// Check if response is latch off command
				if (RelayCount > 250) {
					*State = psComplete;											// move to complete if relay count reaches 250
				} else if (RelayCount == 0) {
					*State = psFailed;												// set to failed if the relay count reaches 0
				}
			}
		} else if (BoardCommsReceiveState == RxBAD) {								// fail if the comms go bad
			*State = psFailed;
		}
		break;
	case psComplete:
		RelayPort_Enabled = false;													// disable relay enable
		CurrentState = csLatchTest;													// move to latch test
		*State = psInitalisation;
		break;
	case psFailed:
		SET_BIT(Board->TPR, (1 << Board->GlobalTestNum));							// TODO: Check if this requires to be cleared opposed to set
		*State = psComplete;
		break;
	}
}

void handleLatchTest(TboardConfig *Board, TprocessState *State) {
	switch (*State) {
	case psInitalisation:
		LatchTestInit();															// Initialise the latch testing procedure
		if (LatchingSolenoidDriverTest(Board)) {									// Check if latch test on any port
			LCD_printf((uns_ch*) "   Latch Testing    ", 2, 0);
			*State = psWaiting;														// move to waiting if latch test on any port
		} else
			*State = psComplete;													// move to complete
		break;
	case psWaiting:
		if (LatchADCflag) {															// check if sample available and within valid sampling time
			if (LatchCountTimer < 2048) {
				HandleLatchSample();												// Handle latch test moving through the various states/pulses
			} else
				CLEAR_BIT(LatchTestStatusRegister, LATCH_SAMPLING);					// Disable sampling if timer limit reached
			LatchADCflag = false;													// Disable sample flag to allow another ADC sample
		}
		if (!READ_BIT(LatchTestStatusRegister, LATCH_SAMPLING) || READ_BIT(LatchTestStatusRegister, LATCH_TEST_COMPLETE))
			*State = psComplete;													// Move to complete if the sample or complete flag has been set
		break;
	case psComplete:
		if (READ_REG(Board->LatchTestPort)) {										// Check if a latch test was run
			HAL_TIM_Base_Stop(&htim10);												// stop the latch timer
			TransmitResults(Board);													// Transmit the results to the SD card
			normaliseLatchResults();												// Get the results into valid voltage form
			PrintLatchResults();													// Print Results & Error Messages
			LatchErrorCheck(Board);													// Check the Latch test error register to ensure test passed
			if (Board->LTR == 0) {													// Print passed if the Latch test register is equal to zero
				printT(
						(uns_ch*) "\n=======================          LATCH TEST PASSED         =======================\n\n");
				Board->TestResults[Board->GlobalTestNum][LatchTestPort * 2] = 1000; // Base 1000 for easier sorting results															// Multiply by two to acount for 2ch of data
			} else {
				printLatchError(Board);
				printT(
						(uns_ch*) "\n=======================          LATCH TEST FAILED          =======================\n\n");
				Board->TestResults[Board->GlobalTestNum][LatchTestPort * 2] = 0;
			}
			CLEAR_REG(Board->LatchTestPort);										// Clear the latch test port register
		}
		if (Board->BoardType == b422x)												// move to sampling if the 4220 is connected as no input sampling is required
			CurrentState = csSampling;
		else
			CurrentState = csAsyncTest;												// begin async sampling on all other boards
		*State = psInitalisation;
		break;
	case psFailed:
		retryCount++;
		if (retryCount > 3) {														// retry latch testing 4 times before returning to IDLE
			HAL_GPIO_WritePin(PIN2EN_GPIO_Port, PIN2EN_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(PIN5EN_GPIO_Port, PIN5EN_Pin, GPIO_PIN_RESET);
			CurrentState = csIDLE;
		} else {
			if (BoardCommsParameters[0] & 0x80) {
				communication_array(0x26, &BoardCommsParameters[0], 1);				// retransmit the latch test command
			}
			retryCount++;
		}
		*State = psInitalisation;
		break;
	}
}

void handleAsyncTest(TboardConfig *Board, TprocessState *State) {
	switch (*State) {
	case psInitalisation:
		HAL_TIM_Base_Start(&htim14);												// Start async clock
		AsyncComplete = false;														// Disable complete flag
		for (uint8 i = Port_1; i <= Port_9; i++) {									// Cycle through checking each port if pulses are set
			Port[i].Async.Active = Port[i].Async.PulseCount ? true : false;
		}
		*State = psWaiting;															// Move to waiting
		break;
	case psWaiting:
		if (AsyncComplete)															// Check if async is complete
			*State = psComplete;
		break;
	case psComplete:
		HAL_TIM_Base_Stop(&htim14);													// Disable clock for async
		CurrentState = csSampling;													// Move to sampling
		*State = psInitalisation;
		break;
	case psFailed:
		retryCount++;
		if (retryCount > 3) {														// reattempt to generate pulses 4 times
			HAL_GPIO_WritePin(PIN2EN_GPIO_Port, PIN2EN_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(PIN5EN_GPIO_Port, PIN5EN_Pin, GPIO_PIN_RESET);
			HAL_TIM_Base_Stop(&htim14);												// Stop the async clock
			CurrentState = csIDLE;													// return to IDLE state
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
		sampleTargetBoard(Board);													// Begin sampling process
		VuserSamples = 0;															// Set the User voltage sample count to zero
		*State = psWaiting;															// move to waiting
		break;
	case psWaiting:
		if (BoardCommsReceiveState == RxGOOD) {
			Response = Data_Buffer[0];
			if (Response == 0x1B || Response == 0x03)								// TODO: change this to wait until samples uploaded!
				communication_response(Board, &Response, &Data_Buffer[1], Datalen);	// Handle the response
			BoardCommsReceiveState = RxWaiting;										// change to waiting state following assessing the comms
		} else if (BoardCommsReceiveState == RxBAD) {								// Move to failed if comms are bad
			*State = psFailed;
			BoardCommsReceiveState = RxWaiting;
		}
		if (samplesUploading && (sampleTime >= 1000)) {								// Print percentage bar to LCD if the sample wait time is greater than 1000
			if (sampleCount == 0)
				LCD_ClearLine(3);													// Clear LCD line 3 if sampleCount reaches 0, prepare for percentage bar
			Percentage = (uint8) ((sampleCount / ((float) sampleTime * 0.9)) * 100);// Reduce SampleTime range to ensure progress bar reaches 100/ holds 100 for a short time
			ProgressBar(Percentage);												// Print the progress bar and percentage
		}

		if (samplesUploaded) {
			samplesUploading = false;
			samplesUploaded = false;												// Once timer times out ensure the system stops waiting and sample upload the results from the target board
			*State = psComplete;
			BoardCommsReceiveState = RxWaiting;
		}
		break;
	case psComplete:
		if (Board->GlobalTestNum == V_12output) {									// Get the various sample voltages of the boards dependant on the test being currently run
			if ((Board->BoardType == b401x) || (Board->BoardType == b402x))
				Vuser.total += getOutputVoltage();
			else
				Vuser.total += getFuseVoltage();
		} else
			Vuser.total += getSampleVoltage();
		VuserSamples++;
		if (TestRigMode == VerboseMode) {											// print what is occuring if the system is in verbose mode
			printT((uns_ch*) "Samples Uploaded\n\n");
			printT((uns_ch*) "Requesting Results\n\n");
		}

		if (VuserSamples >= 100) {
			Vuser.total /= VuserSamples;											// Get the average voltage across the sample time
			Vuser.average = Vuser.total * (15.25 / 4096);							// calculate the average sample voltage
			if (compareSampleVoltage(Board, &(Vuser.average), &(setSampleVoltages[Board->GlobalTestNum]))) {// Check to see if the average is within tolerance
				*State = psInitalisation;
				CurrentState = csUploading;											// move to uploading if successful
			}
			VuserSamples = Vuser.total = sampleCount = 0;							// reinitialise variables
		}
		break;
	case psFailed:
		retryCount++;																// increment retry count if sampling fails
		if (retryCount > 3) {
			HAL_GPIO_WritePin(PIN2EN_GPIO_Port, PIN2EN_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(PIN5EN_GPIO_Port, PIN5EN_Pin, GPIO_PIN_RESET);
			CurrentState = csIDLE;													// Move to IDLE if samples fails
		}
		*State = psInitalisation;
		break;
	}
}

void handleUploading(TboardConfig *Board, TprocessState *State) {
	uns_ch Response;
	switch (*State) {
	case psInitalisation:
		uploadSamplesTargetBoard(Board);											// Begin sample uploading to test rig
		*State = psWaiting;
		break;
	case psWaiting:
		if (BoardCommsReceiveState == RxGOOD) {										// Comms good
			Response = Data_Buffer[0];
			if (Response == 0x19)													// check if response from target board is uploading response
				*State = psComplete;
			if (Response == 0x03)													// if board is busy, reattempt to upload results
				communication_response(Board, &Response, (uns_ch*) &Data_Buffer[0], Datalen);
			BoardCommsReceiveState = RxWaiting;
		} else if (BoardCommsReceiveState == RxBAD) {
			*State = psFailed;														// fail on bad comms
		}
		break;
	case psComplete:
		communication_response(Board, &Response, &Data_Buffer[1], Datalen - 1);		// handle response
		*State = psInitalisation;
		CurrentState = csSortResults;												// move to sort results
		break;
	case psFailed:
		BoardCommsReceiveState = RxWaiting;
		if (retryCount > 15) {														// retry 16 times to see if uploading is complete, continue to poll board if response is busy
			HAL_GPIO_WritePin(FAIL_GPIO_Port, FAIL_Pin, GPIO_PIN_SET);				// enable fail LED and disable power to board
			HAL_GPIO_WritePin(PIN2EN_GPIO_Port, PIN2EN_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(PIN5EN_GPIO_Port, PIN5EN_Pin, GPIO_PIN_RESET);
			CurrentState = csIDLE;
			*State = psWaiting;
		} else {
			retryCount++;															// increment retrycount on fail
			*State = psInitalisation;
		}
		break;
	}
}

void handleSortResults(TboardConfig *Board, TprocessState *State) {
	switch (*State) {
	case psInitalisation:
		Decompress_Channels(Board, (uns_ch*) &sampleBuffer[0]); 					//Returns the ammount of channels sampled
		*State = psWaiting;
		break;
	case psWaiting:
		sprintf(SDcard.FILEname, "/TEST_RESULTS/%lu_%x/%lu.CSV", Board->SerialNumber, Board->BoardType,
				Board->SerialNumber);												// Populate the filename for the results to be written to the SDcard
		HandleResults(Board, &CHval[Board->GlobalTestNum][0]);						// Handle the results, compare the set v measured results determine pass or fail for the results
		*State = psComplete;
		break;
	case psComplete:
		Board->GlobalTestNum++; 													//Increment Test Number
		if (CheckTestNumber(Board)) {												// Check if test limit reached
			*State = psInitalisation;												// continue to test if more testing required
			CurrentState = csConfiguring;
		} else {
			WriteVoltages(Board, &SDcard);											// write voltages to board
			if (READ_BIT(Board->BSR, BOARD_TEST_PASSED)) {							// Initialise board if test passed
				CurrentState = csInitialising;
				*State = psInitalisation;
			} else {
				TestComplete(Board);												// Run the test complete routine
				LCD_printf((uns_ch*) "# - Cont", 4, 13);
				while (!KP[hash].Pressed) {											// prompt user to press hash on a failed test

				}
				KP[hash].Pressed = false;
				LCD_Clear();														// Clear LCD and return to IDLE state
				*State = psInitalisation;
				CurrentState = csIDLE;
			}
		}
		break;
	case psFailed:
		retryCount++;
		if (retryCount > 3) {
			HAL_GPIO_WritePin(PIN2EN_GPIO_Port, PIN2EN_Pin, GPIO_PIN_RESET);		// remove power and return to IDLE state upon failure
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
		if (READ_BIT(Board->BSR, BOARD_SERIALISED))									// Check if board serialised previously
			*State = psComplete;
		else {
			interrogateTargetBoard();												// Use the interrogate command to get serial number
			*State = psWaiting;
		}
		break;
	case psWaiting:
		if (BoardCommsReceiveState == RxGOOD) {										// Upon comms good
			tempSerial = ReadSerialNumber(&Response, &Data_Buffer[0], Datalen);		// store temp serial to compare to that of the board
			if (tempSerial == Board->SerialNumber) {								// If they match set the serialised flag and move to complete
				SET_BIT(Board->BSR, BOARD_SERIALISED);
				*State = psComplete;
			} else if (Response == 0x11) {
				memcpy(&CurrentSerial, &tempSerial, 4); 							// Load Current Serial Number
				Command = 0x12;														// Send new serial number to board
				communication_arraySerial(Command, CurrentSerial, Board->SerialNumber);	//Write New Serial Number
//				BoardCommsReceiveState = RxWaiting;									// Set RX to waiting, not sure if waiting is needed as it is set in the transmit routine
			} else if (Response == 0x13) {
				if (tempSerial != Board->SerialNumber) {							// If new serial number does not match reserialise board
					Command = 0x10;													// TODO: Remove this as it is the same routine that is run in psInitialisation
					communication_arraySerial(Command, 0, 0);
					*State = psInitalisation;
				} else {
					SET_BIT(Board->BSR, BOARD_SERIALISED);							// Set bit on successful serialising
					*State = psComplete;
				}
				BoardCommsReceiveState = RxWaiting;
			}
		} else if (BoardCommsReceiveState == RxBAD)									// Move to failed on bad comms
			*State = psFailed;
		break;
	case psComplete:
		if (READ_BIT(Board->BSR, BOARD_SERIALISED)) {								// Create results file upon successful serialisation
			CreateResultsFile(&SDcard, Board);
			Close_File(&SDcard);
			printT((uns_ch*) "=====     Board Serialised     =====\n");
			if (TestRigMode == SerialiseMode)										// Return to IDLE if mode is serialise
				CurrentState = csIDLE;
			else
				CurrentState = csInterogating;										// Move to interrogation on all other modes
			*State = psInitalisation;
		} else
			*State = psFailed;														// failed if board isnt serialised
		break;
	case psFailed:
		retryCount++;
		if (retryCount > 3) {
			HAL_GPIO_WritePin(PIN2EN_GPIO_Port, PIN2EN_Pin, GPIO_PIN_RESET);		// remove power and return to IDLE
			HAL_GPIO_WritePin(PIN5EN_GPIO_Port, PIN5EN_Pin, GPIO_PIN_RESET);
			CurrentState = csIDLE;
		}
		*State = psInitalisation;
		break;
	}
}
