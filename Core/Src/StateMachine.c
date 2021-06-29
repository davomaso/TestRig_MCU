#include "main.h"
#include "interogate_project.h"
#include "SetVsMeasured.h"
#include "UART_Routine.h"
#include "calibration.h"

void handleIdle(TboardConfig *Board, TprocessState * State) {
	switch (*State) {
		case psComplete:
    		  initialiseTargetBoard();
    		  CurrentState = csInitialising;
    		  ProcessState = psWaiting;
			break;
		case psWaiting:
			if (CheckLoom) {
		  	  	  /*
		  	  	   * If system is in the idle, waiting and checkloom is toggled true by the timeout, the loom
		  	  	   * should be scanned with the board that is connected returned.
		  	  	   */
				  scanLoom(&BoardConnected);
				  ConfigInit();
			}
			break;
		case psFailed:
			break;
	}

}

void handleInitialising(TboardConfig *Board, TprocessState * State) {
	uint8 Command;
	uint8 Response;

	switch (*State) {
	case psComplete:
  		if (READ_BIT( Board->BSR, BOARD_TEST_PASSED )) {	//Board Passed
  			SET_BIT( Board->BSR, BOARD_INITIALISED );
  			communication_response(&Response, &UART2_RXbuffer, UART2_RecPos);
  			if (!READ_BIT(Board->BSR, BOARD_SERIALISED)) {	//Board Serialised
				Command = 0x10;
				communication_arraySerial(Command, 0, 0);
				CurrentState = csSerialise;
				ProcessState = psWaiting;
  			} else {
  				TestComplete(Board);
  				CurrentState = csIDLE;
  				*State = psWaiting;
  			}
		} else {
				//Refresh LCD screen
			if ( !READ_BIT(BoardConnected.BSR, BOARD_SERIALISED)) {	// when board is not serialised prompt user for user input
				EnterSerialNumber();
			} else {
				SET_BIT(BoardConnected.BSR, BOARD_INITIALISED);
				LCD_Clear();
				TestRig_Init();
				interrogateTargetBoard();
				CurrentState = csInterogating;
				ProcessState = psWaiting;
			}
		}
		break;
	case psFailed:
			retryCount++;
			initialiseTargetBoard();
			ProcessState = psWaiting;
		break;
	}
}


void handleProgramming(TboardConfig *Board, TprocessState * State) {
	switch (*State) {
	case psComplete:
  			printT("Programming\n");
  			ProgramTargetBoard(&BoardConnected);
		break;
	case psFailed:
			retryCount++;
			SDfileInit();
		break;
	}
}

void handleCalibrating(TboardConfig *Board, TprocessState * State) {
	uint8 Command;
	uint8 Response;
	switch (*State) {
	case psComplete:
  		communication_response(&Response, &UART2_RXbuffer, UART2_RecPos);
  		if ( Response == 0xC1 ) {
      		if (!READ_BIT(Board->BSR, BOARD_INITIALISED)) {
				initialiseTargetBoard();
				CurrentState = csInitialising;
				*State = psWaiting;
      		} else {
      			configureTargetBoard();
	            CurrentState = csConfiguring;
	            *State = psWaiting;
      		}
  		} else {
  			//TODO: add timeout and repeat transmission
  		}
		break;
	case psFailed:
			retryCount++;
			printT("Calibration Failed Recalibrating Device\n");
			switchToCurrent = false;
			TargetBoardCalibration(&BoardConnected);
			ProcessState = psWaiting;
		break;
	}
}

void handleInterogating(TboardConfig *Board, TprocessState * State) {
	uns_ch Command;
	uns_ch Response;

	switch (*State) {
	case psComplete:
		communication_response(&Response, &UART2_RXbuffer[0], UART2_RecPos);
		switch(Response) {
			case 0x09:
	            if (READ_BIT( Board->BSR, BOARD_CALIBRATED)) {	//Check if board has been calibrated yet
  	                if (Board->BoardType == b422x) {
  	                	if ( READ_BIT(Board->BSR, BOARD_INITIALISED) ) {
							if (CheckTestNumber(Board)) {
								LatchingSolenoidDriverTest(Board, Board->GlobalTestNum);
								CompareResults(Board, &CHval[Board->GlobalTestNum]);
							} else {
								TestComplete(Board);
								CurrentState = csIDLE;
								*State = psWaiting;
							}
  	                	} else {
  	                		initialiseTargetBoard();
  	                		CurrentState = csInitialising;
  	                		*State = psWaiting;
  	                	}
  	                } else {
  	                	configureTargetBoard();
	  	                CurrentState = csConfiguring;
	  	                *State = psWaiting;
  	                }
	            } else {
		  	    		LCD_setCursor(2, 0);
		  	    		sprintf(debugTransmitBuffer, "    Calibrating    ");
		  	    		LCD_printf(&debugTransmitBuffer, strlen(debugTransmitBuffer));
	            		switchToCurrent = false;
	            		HAL_Delay(200);
	            		TargetBoardCalibration(Board);
	            		CurrentState = csCalibrating;
	            		*State = psWaiting;
	            }
				break;

			case 0x11:
				currentBoardConnected(Board);
	            	if (Board->Version < getCurrentVersion(Board->BoardType) ) {
	            		CurrentState = csProgramming;
	            		ProcessState = psComplete;
	            	} else { //TODO: Put the user choice to reprogram the board here
	            		if (ContinueWithCurrentProgram() ) {
							currentBoardConnected(Board);
  	            		CurrentState = csProgramming;
  	            		*State = psComplete;
  	            		LCD_ClearLine(4);
  	            		break;
	            		}
	            	}
	            		if( ContinueWithCurrentSerial() ) {
	            			interrogateTargetBoard();
	            			CurrentState = csInterogating;
	            			ProcessState = psWaiting;
	            			break;
	            		} else {
	            			EnterSerialNumber(Board);
	            			CurrentState = csSerialise;
	            			ProcessState = psWaiting;
	            		}
				break;

			case 0x27:
	            	if ( (BoardConnected.TestResults[BoardConnected.GlobalTestNum][BoardConnected.GlobalTestNum] == true) || (BoardConnected.GlobalTestNum == BoardConnected.latchPortCount) ) {
	            		interrogateTargetBoard();
          			ProcessState = psWaiting;
	            	}
				break;
			}
		break;

	case psFailed:
		retryCount++;
		if (!READ_BIT(BoardConnected.BSR, BOARD_PROGRAMMED) && (retryCount > 2)) {
			currentBoardConnected(&BoardConnected);
			LCD_ClearLine(1);
			sprintf(debugTransmitBuffer, "    Programming    ");
			LCD_printf(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
			CurrentState = csProgramming;
			ProcessState = psComplete;
		} else {
			uns_ch Command;
			Command = 0x10;
			communication_arraySerial(Command, 0, 0);
			ProcessState = psWaiting;
		}
		break;
	}
}

void handleConfiguring(TboardConfig *Board, TprocessState * State) {
	uns_ch Command;
	uns_ch Response;
	switch (*State) {
		case psComplete:
				communication_response(&Response, &UART2_RXbuffer, UART2_RecPos);
				if (Response == 0x57) {
					if (SDIenabled)
						USART6->CR1 |= (USART_CR1_RXNEIE);

					Command = 0x1A;
					SetPara(Command);
					communication_array(Command,&Para, Paralen);
					*State = psWaiting;
					CurrentState = csSampling;
				}
			break;
		case psFailed:
			retryCount++;
	          Command = 0x56;
              communication_array(Command,&Para[0], Paralen);
              ProcessState = psWaiting;
			break;
	}
}

void handleSampling(TboardConfig *Board, TprocessState * State) {
	uns_ch Response;
	switch (*State) {
	case psComplete:
			communication_response(&Response, &UART2_RXbuffer, UART2_RecPos);
			printT("Samples Uploaded\n\n");
			printT("Requesting Results\n\n");
			ProcessState = psWaiting;
			CurrentState = csUploading;
		break;
	case psFailed:
		retryCount++;
			HAL_GPIO_WritePin(FAIL_GPIO_Port, FAIL_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(PIN2EN_GPIO_Port, PIN2EN_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(PIN5EN_GPIO_Port, PIN5EN_Pin, GPIO_PIN_RESET);
			TargetBoardParamInit(); //Change position of this if board connected data is required by other functions.
			CurrentState = csIDLE;
			ProcessState = psWaiting;
		break;
	}
}

void handleUploading(TboardConfig *Board, TprocessState * State) {
	uns_ch Command;
	switch (*State) {
	case psComplete:
			Command = 0x18;
			SetPara(Command);
			communication_array(Command,&Para, Paralen);
			*State = psWaiting;
			CurrentState = csSortResults;
		break;
	case psFailed:
			retryCount++;
			HAL_GPIO_WritePin(FAIL_GPIO_Port, FAIL_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(PIN2EN_GPIO_Port, PIN2EN_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(PIN5EN_GPIO_Port, PIN5EN_Pin, GPIO_PIN_RESET);
			TargetBoardParamInit(); //Change position of this if board connected data is required by other functions.
			CurrentState = csIDLE;
			ProcessState = psWaiting;
		break;
	}
}

void handleSortResults(TboardConfig *Board, TprocessState * State) {
	uns_ch Command;
	uns_ch Response;
	switch (*State) {
	case psComplete:
		communication_response(&Response, &UART2_RXbuffer, UART2_RecPos);
		if(Response == 0x03) {
			CurrentState = csUploading;
			*State = psWaiting;
		} else if (Board->GlobalTestNum <= Board->testNum) {
			if( CheckTestNumber(Board)) {
				configureTargetBoard();
				*State = psWaiting;
				CurrentState = csConfiguring;
			} else {
			if(READ_BIT( Board->BSR, BOARD_TEST_PASSED )) {
				initialiseTargetBoard();
				CurrentState = csInitialising;
				*State = psWaiting;
			} else {
				TestComplete(Board);
				*State = psWaiting;
				CurrentState = csIDLE;
					}
				}
		}
		break;
	case psFailed:
			retryCount++;
			HAL_GPIO_WritePin(FAIL_GPIO_Port, FAIL_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(PIN2EN_GPIO_Port, PIN2EN_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(PIN5EN_GPIO_Port, PIN5EN_Pin, GPIO_PIN_RESET);
			TargetBoardParamInit(); //Change position of this if board connected data is required by other functions.
			CurrentState = csIDLE;
			ProcessState = psWaiting;
		break;
	}
}

void handleSerialise(TboardConfig *Board, TprocessState * State) {
	uint8 Command;
	uint8 Response;
	uint32 tempSerial;
	uint32 CurrentSerial;
	uint32 NewSerial;
	switch (*State) {
		case psComplete:
			tempSerial = ReadSerialNumber(&Response, &UART2_RXbuffer[0], UART2_RecPos);
			if (Response == 0x11) {
						//Load Current Serial Number
					memcpy(&CurrentSerial, &tempSerial, 4);
						//Write New Serial Number
					memcpy(&NewSerial, &(Board->SerialNumber), 4);
					Command = 0x12;
					communication_arraySerial(Command, CurrentSerial, NewSerial);
					ProcessState = psWaiting;
			} else if (Response == 0x13) {
				if(tempSerial != Board->SerialNumber) {
					Command = 0x10;
					communication_arraySerial(Command, 0, 0);
					ProcessState = psWaiting;
				} else {
					SET_BIT( Board->BSR, BOARD_SERIALISED );
					printT("=====     Board Serialised     =====\n");
					sprintf(debugTransmitBuffer, "Serial number %d loaded into board\n", BoardConnected.SerialNumber);
					printT(&debugTransmitBuffer[0]);

					interrogateTargetBoard();
					printT("Interogating...\n");
					CurrentState = csInterogating;
					ProcessState = psWaiting;
				}
			}
		break;
	case psFailed:
			HAL_GPIO_WritePin(FAIL_GPIO_Port, FAIL_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(PIN2EN_GPIO_Port, PIN2EN_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(PIN5EN_GPIO_Port, PIN5EN_Pin, GPIO_PIN_RESET);
			TargetBoardParamInit(); //Change position of this if board connected data is required by other functions.
			CurrentState = csIDLE;
			ProcessState = psWaiting;
		break;
	}
}

void TestComplete(TboardConfig * Board) {
		HAL_GPIO_WritePin(PIN2EN_GPIO_Port, PIN2EN_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(PIN5EN_GPIO_Port, PIN5EN_Pin, GPIO_PIN_RESET);
		if (READ_BIT(Board->BSR, BOARD_TEST_PASSED) ) {
			HAL_GPIO_WritePin(PASS_GPIO_Port, PASS_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(FAIL_GPIO_Port, FAIL_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);
			sprintf(previousTestBuffer, "Previous Test Passed");
		} else {
			HAL_GPIO_WritePin(FAIL_GPIO_Port, FAIL_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(PASS_GPIO_Port, PASS_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);
			sprintf(previousTestBuffer, "Previous Test Failed");
		}
		timeOutEn = false;
		LCD_ClearLine(2);
		LCD_setCursor(2, 0);
		LCD_printf(&previousTestBuffer[0], strlen(previousTestBuffer));
}

void EnterSerialNumber(TboardConfig *Board) {
	uint8 Command;
	LCD_ClearLine(2);
	LCD_ClearLine(3);
	LCD_ClearLine(4);
	LCD_setCursor(2, 0);
	sprintf(debugTransmitBuffer, "Enter Serial Number:");
	LCD_printf(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));

	printT("Enter Serial Number: \n");
	Board->SerialNumber = read_serial();
//				SET_BIT(BoardConnected.BSR, BOARD_SERIALISED);	// Set board register serialised bit to set
	if (Board->SerialNumber) {
		Command = 0x10;
		communication_arraySerial(Command, 0, 0);
		CurrentState = csSerialise;
		ProcessState = psWaiting;
	}
}
