#include "main.h"
#include "interogate_project.h"
#include "SetVsMeasured.h"
#include "UART_Routine.h"
#include "calibration.h"

void handleIdle(TboardConfig *Board, TprocessState * State) {

	switch (*State) {
		case psInitalisation:
				*State = psWaiting;
			break;
		case psWaiting:
			if (CheckLoom) {
		  	  	  /*
		  	  	   * If system is in the idle, waiting and checkloom is toggled true by the timeout, the loom
		  	  	   * should be scanned with the board that is connected returned.
		  	  	   */
				  scanLoom(&BoardConnected);
			}
			if (KP_1.Pressed) {
						HAL_Delay(50);
				  		//If 1 is pressed
					  	KP_1.Pressed = false;
				  		KP_1.Count = 0;
				  		TestRig_Init();
				  		TargetBoardParamInit();
				  		HAL_GPIO_WritePin(PIN2EN_GPIO_Port, PIN2EN_Pin, GPIO_PIN_SET);
				  		clearTestStatusLED();
				  		testInputVoltage();
				  		HAL_Delay(1200);
				  		CurrentState = csInterogating;
				  		*State = psInitalisation;
				  }
			break;
		case psComplete:
    		  initialiseTargetBoard(Board);
    		  CurrentState = csInitialising;
    		  ProcessState = psWaiting;
			break;
		case psFailed:
			*State = psWaiting;
			break;
	}

}

void handleInitialising(TboardConfig *Board, TprocessState * State) {
	uint8 Command;
	uint8 Response;
	switch (*State) {
	case psInitalisation:
			if (READ_BIT(Board->BSR, BOARD_TEST_PASSED) || !READ_BIT(Board->BSR, BOARD_INITIALISED)) {
				initialiseTargetBoard(Board);
				*State = psWaiting;
			} else {
				*State = psComplete;
			}
		break;
	case psWaiting:
			if (ReceiveState == RxGOOD) {
				Response = Data_Buffer[0];
				if(Response == 0xCD)
					*State = psComplete;
				else
					*State = psFailed;
			} else if (ReceiveState == RxBAD) {
				*State = psFailed;
			}
		break;
	case psComplete:
			SET_BIT(Board->BSR, BOARD_INITIALISED);
			if (READ_BIT(Board->BSR, BOARD_TEST_PASSED)) {
				TestComplete(Board);
				CurrentState = csIDLE;
				*State = psInitalisation;
			} else {
				CurrentState = csTestBegin;
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

void handleTestBegin(TboardConfig *Board, TprocessState * State) {
	switch (*State) {
	case psInitalisation:
		sprintf(&lcdBuffer, "    %x     v%x  ",Board->BoardType,Board->Version);
//		LCD_displayString(&debugTransmitBuffer, strlen(debugTransmitBuffer));//
		LCD_printf(&lcdBuffer[0], 1, 0);
		sprintf(&lcdBuffer, "    S/N  %d",Board->SerialNumber);
		LCD_printf(&lcdBuffer,2,0);
		LCD_printf("1 - Reprog  Exit - *",3,0);
		LCD_printf("3 - New SN  Test - #",4,0);
		*State = psWaiting;
		break;
	case psWaiting:
			if (KP_hash.Pressed || KP_1.Pressed || KP_3.Pressed)
				*State = psComplete;
		break;
	case psComplete:
			if (KP_hash.Pressed) {
				KP_hash.Pressed = false;
				KP_hash.Count = 0;
				CurrentState = csConfiguring;
				*State = psInitalisation;
			} else if (KP_1.Pressed) {
				KP_1.Pressed = false;
				KP_1.Count = 0;
				CLEAR_BIT(Board->BSR, BOARD_PROGRAMMED);
				CurrentState = csProgramming;
				*State = psInitalisation;
			} else if (KP_3.Pressed) {
				KP_3.Pressed = false;
				KP_3.Count = 0;
				CLEAR_BIT(Board->BSR, BOARD_SERIALISED);
				CurrentState = csSerialise;
				*State = psInitalisation;
			} else
				*State = psFailed;
			LCD_ClearLine(3);
			LCD_ClearLine(4);
		break;
	case psFailed:
			retryCount++;	//Dont know if this failure capture is required
			HAL_GPIO_WritePin(TB_Reset_GPIO_Port, TB_Reset_Pin, GPIO_PIN_SET);
			SDfileInit();
  			ProgramTargetBoard(Board);
  			if (retryCount >= 3){
  				printT("Cannot Program Target Board\n");
  				printT("Attempted 3 times....\n");
  				CurrentState = csIDLE;
  				*State = psWaiting;
  			}
		break;
	}
}


void handleProgramming(TboardConfig *Board, TprocessState * State) {
	switch (*State) {
	case psInitalisation:
			printT("Programming\n");
			ProgrammingInit();
			*State = psWaiting;
		break;
	case psWaiting:
			//Rewrite the programming routine to not be blocking
			ProgramTargetBoard(Board);
			*State = psComplete;
		break;
	case psComplete:
			CurrentState = csInterogating;
			*State = psInitalisation;
		break;
	case psFailed:
			retryCount++;
			HAL_GPIO_WritePin(TB_Reset_GPIO_Port, TB_Reset_Pin, GPIO_PIN_SET);
			SDfileInit();
  			ProgramTargetBoard(Board);
  			if (retryCount >= 3){
  				printT("Cannot Program Target Board\n");
  				printT("Attempted 3 times....\n");
  				CurrentState = csIDLE;
  				*State = psWaiting;
  			}
		break;
	}
}

void handleCalibrating(TboardConfig *Board, TprocessState * State) {
	uint8 Command;
	uint8 Response;
	switch (*State) {
	case psInitalisation:
			if (READ_BIT(Board->BSR, BOARD_CALIBRATED) || (Board->BoardType == b422x) ) {
					*State = psComplete;
			} else {
				sprintf(&lcdBuffer,"    Calibrating" );
				LCD_printf(&lcdBuffer, 2, 0);
				switchToCurrent = false;
				TargetBoardCalibration(Board);
				*State = psWaiting;
			}
		break;
	case psWaiting:
			if (ReceiveState == RxGOOD) {
				Response = Data_Buffer[0];
				if(Response == 0xC1)
					*State = psComplete;
				else
					*State = psFailed;
			} else if (ReceiveState == RxBAD) {
				*State = psFailed;
			}
		break;
	case psComplete:
			SET_BIT( BoardConnected.BSR, BOARD_CALIBRATED );
			printT("=====     Board Calibrated     =====\n");
			CurrentState = csSerialise;
			*State = psInitalisation;
		break;
	case psFailed:
			retryCount++;
			printT("Calibration Failed Recalibrating Device\n");
			switchToCurrent = false;
			TargetBoardCalibration(&BoardConnected);
			ProcessState = psWaiting;
			ReceiveState = RxWaiting;
		break;
	}
}

void handleInterogating(TboardConfig *Board, TprocessState * State) {
	uns_ch Command;
	uns_ch Response;
	switch (*State) {
	case psInitalisation:
			interrogateTargetBoard();
			*State = psWaiting;
		break;
	case psWaiting:
			if (ReceiveState == RxGOOD) {
				Response = Data_Buffer[4];
				if(Response == 0x11)
					*State = psComplete;
				else
					*State = psFailed;
			} else if (ReceiveState == RxBAD) {
				*State = psFailed;
			}
		break;
	case psComplete:
			communication_response(Board,&Response, &Data_Buffer[5], strlen(Data_Buffer));
			currentBoardConnected(Board);
			CurrentState = csCalibrating;
			*State = psInitalisation;
		break;

	case psFailed:
		if (!READ_BIT(BoardConnected.BSR, BOARD_PROGRAMMED) && (retryCount++ > 2)) {
			currentBoardConnected(&BoardConnected);
			LCD_ClearLine(1);
			sprintf(debugTransmitBuffer, "    Programming    ");
			LCD_displayString(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
			CurrentState = csProgramming;
			*State = psInitalisation;
		} else {
			interrogateTargetBoard();
			ReceiveState = RxWaiting;
			*State = psInitalisation;
		}
		break;
	}
}

void handleConfiguring(TboardConfig *Board, TprocessState * State) {
	uns_ch Command;
	uns_ch Response;
	switch (*State) {
		case psInitalisation:
				configureTargetBoard(Board);
				*State = psWaiting;
			break;
		case psWaiting:
				if (ReceiveState == RxGOOD) {
					Response = Data_Buffer[0];
					if(Response == 0x57) {
						HAL_Delay(250);
						*State = psComplete;
					} else
						*State = psFailed;
				} else if (ReceiveState == RxBAD) {
					*State = psFailed;
				}
			break;
		case psComplete:
				communication_response(Board,&Response, &UART2_RXbuffer, UART2_RecPos);
				if (SDIenabled)
					USART6->CR1 |= (USART_CR1_RXNEIE);

				CurrentState = csLatchTest;
				*State = psInitalisation;
			break;
		case psFailed:
				retryCount++;
				Command = 0x56;
				communication_array(Command,&Para[0], Paralen);
				*State = psWaiting;
				ReceiveState = RxWaiting;
			break;
	}
}

void handleLatchTest(TboardConfig *Board, TprocessState * State) {
	uns_ch Command;
	uns_ch Response;
	switch (*State) {
		case psInitalisation:
				LatchTestInit();
				LatchingSolenoidDriverTest(Board);
				if (LatchTestPort != 0xFF) {	//return 0xFF for failure to find if latch test port is active
					LCD_printf("   Latch Testing    ", 2, 0);
					*State = psWaiting;
				} else
					*State = psComplete;
			break;
		case psWaiting:
			if (LatchCountTimer < latchCountTo) {
				if(READ_BIT(LatchTestStatusRegister, STABLE_INPUT_VOLTAGE)) {	//If input voltage is stable
						if (!READ_BIT(LatchTestStatusRegister, LATCH_ON_SAMPLING) && !READ_BIT(LatchTestStatusRegister, LATCH_ON_COMPLETE)) {
								//Begin Latch on sampling
							SET_BIT(LatchTestStatusRegister, LATCH_ON_SAMPLING);
							Para[0] = LatchTestParam(LatchTestPort, 1);
							communication_array(0x26, &Para[0], 1);
						} else if (READ_BIT(LatchTestStatusRegister, LATCH_ON_COMPLETE) && READ_BIT(LatchTestStatusRegister,LATCH_ON_SAMPLING))	{
								//Latch on sampling complete, reset voltage stability check
							if (ReceiveState != RxWaiting) {
								if (ReceiveState == RxGOOD) {
									Response = Data_Buffer[0];
									if (Response == 0x27) {
										CLEAR_BIT(LatchTestStatusRegister, LATCH_ON_SAMPLING);
										CLEAR_BIT(LatchTestStatusRegister, STABLE_INPUT_VOLTAGE);
										stableVoltageCount = 25;
									}
									ReceiveState = RxWaiting;
								} else if (ReceiveState == RxBAD)
									*State = psFailed;
							}
						} else if (READ_BIT(LatchTestStatusRegister, LATCH_ON_COMPLETE) && !READ_BIT(LatchTestStatusRegister, LATCH_OFF_SAMPLING) && !READ_BIT(LatchTestStatusRegister, LATCH_OFF_COMPLETE)) 	{
								//Begin Latch off sampling
							SET_BIT(LatchTestStatusRegister, LATCH_OFF_SAMPLING);
							Para[0] = LatchTestParam(LatchTestPort, 0);
							communication_array(0x26, &Para[0], 1);
						} else if (READ_BIT(LatchTestStatusRegister, LATCH_OFF_COMPLETE) && READ_BIT(LatchTestStatusRegister, LATCH_OFF_SAMPLING)) 	{
								//Latch off sampling complete, reset voltage stability check
							if (ReceiveState != RxWaiting) {
								if (ReceiveState == RxGOOD) {
									Response = Data_Buffer[0];
									if (Response == 0x27) {
										CLEAR_BIT(LatchTestStatusRegister, LATCH_ON_SAMPLING);
										CLEAR_BIT(LatchTestStatusRegister, STABLE_INPUT_VOLTAGE);
										stableVoltageCount = 25;
										runLatchTimeOut(2000);
									}
									ReceiveState = RxWaiting;
								} else if (ReceiveState == RxBAD)
									*State = psFailed;
							}

						} else if (READ_BIT(LatchTestStatusRegister, LATCH_ON_COMPLETE) && READ_BIT(LatchTestStatusRegister, LATCH_OFF_COMPLETE)) {
								*State = psComplete;

						}
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
					//		TransmitResults();
						normaliseLatchResults();
						PrintLatchResults();
						LatchErrorCheck(Board);
						if(!(Board->LTR) ) {
							sprintf(debugTransmitBuffer,"\n==============   LATCH TEST PASSED  ==============\n\n\n\n");
							CDC_Transmit_FS(&debugTransmitBuffer, strlen(debugTransmitBuffer));
							Board->TestResults[Board->GlobalTestNum][LatchTestPort] = true;
						} else {
							printLatchError(&Board->LTR);
							printT("\n==============   LATCH TEST FAILED  ==============\n\n\n\n");
							Board->TestResults[Board->GlobalTestNum][LatchTestPort] = false;
							if(Board->BoardType == b422x) { //TODO: Check if this needs to be here with code changes
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
					TargetBoardParamInit(); //Change position of this if board connected data is required by other functions.
					CurrentState = csIDLE;
					ProcessState = psWaiting;
				} else {
					retryCount++;
					*State = psInitalisation;
				}
			break;
	}
}

void handleAsyncTest(TboardConfig *Board, TprocessState *State) {
	switch(*State) {
		case psInitalisation:
				AsyncComplete = false;
				Async_Port1.Active = Async_Port1.PulseCount ? true:false;
				Async_Port2.Active = Async_Port2.PulseCount ? true:false;
				Async_Port3.Active = Async_Port3.PulseCount ? true:false;
				Async_Port4.Active = Async_Port4.PulseCount ? true:false;
				Async_Port5.Active = Async_Port5.PulseCount ? true:false;
				Async_Port6.Active = Async_Port6.PulseCount ? true:false;
				Async_Port7.Active = Async_Port7.PulseCount ? true:false;
				Async_Port8.Active = Async_Port8.PulseCount ? true:false;
				Async_Port9.Active = Async_Port9.PulseCount ? true:false;
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
				TargetBoardParamInit(); //Change position of this if board connected data is required by other functions.
				CurrentState = csIDLE;
				ProcessState = psWaiting;
			break;
	}
}

void handleSampling(TboardConfig *Board, TprocessState * State) {
	uns_ch Command;
	uns_ch Response;
	switch (*State) {
	case psInitalisation:
			LCD_printf("      Sampling      ", 2, 0);
			Command = 0x1A;
			SetPara(Board, Command);
			communication_array(Command,&Para, Paralen);
			*State = psWaiting;
		break;
	case psWaiting:
			if (ReceiveState == RxGOOD) {
				Response = Data_Buffer[0];
				if(Response == 0x1B || Response == 0x03)	//TODO: change this to wait until samples uploaded!
					communication_response(Board, &Response, &Data_Buffer[1], Datalen);
				ReceiveState = RxWaiting;
			} else if (ReceiveState == RxBAD) {
				*State = psFailed;
				ReceiveState = RxWaiting;
			}
			if (samplesUploaded) {
				sampleCount = 0;
				samplesUploading = false;
				samplesUploaded = false;
				*State = psComplete;
				ReceiveState = RxWaiting;
			}
		break;
	case psComplete:
			printT("Samples Uploaded\n\n");
			printT("Requesting Results\n\n");
			*State = psInitalisation;
			CurrentState = csUploading;
		break;
	case psFailed:
		if (retryCount > 2) {
			HAL_GPIO_WritePin(FAIL_GPIO_Port, FAIL_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(PIN2EN_GPIO_Port, PIN2EN_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(PIN5EN_GPIO_Port, PIN5EN_Pin, GPIO_PIN_RESET);
			TargetBoardParamInit(); //Change position of this if board connected data is required by other functions.
			CurrentState = csIDLE;
			ProcessState = psWaiting;
		} else {
			retryCount++;
			*State = psInitalisation;
		}
		break;
	}
}

void handleUploading(TboardConfig *Board, TprocessState * State) {
	uns_ch Command;
	uns_ch Response;
	switch (*State) {
	case psInitalisation:
			LCD_printf("     Uploading      ", 2, 0);
			uploadSamplesTargetBoard(Board);
			*State = psWaiting;
		break;
	case psWaiting:
			if (ReceiveState == RxGOOD) {
				Response = Data_Buffer[0];
				if(Response == 0x19)	//TODO: change this to wait until samples uploaded!
					*State = psComplete;
				if(Response == 0x03)
					communication_response(Board,&Response, &Data_Buffer, Datalen);
				ReceiveState = RxWaiting;
			} else if (ReceiveState == RxBAD) {
				*State = psFailed;
			}
			if (samplesUploaded) {
				sampleCount = 0;
				samplesUploading = false;
				samplesUploaded = false;
				uploadSamplesTargetBoard(Board);
				ReceiveState = RxWaiting;
			}

		break;
	case psComplete:
			communication_response(Board,&Response, &Data_Buffer[1], Datalen-1);
			*State = psInitalisation;
			CurrentState = csSortResults;
		break;
	case psFailed:
		ReceiveState = RxWaiting;
		if (retryCount > 2) {
			HAL_GPIO_WritePin(FAIL_GPIO_Port, FAIL_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(PIN2EN_GPIO_Port, PIN2EN_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(PIN5EN_GPIO_Port, PIN5EN_Pin, GPIO_PIN_RESET);
			TargetBoardParamInit(); //Change position of this if board connected data is required by other functions.
			CurrentState = csIDLE;
			ProcessState = psWaiting;
		} else {
			retryCount++;
			uploadSamplesTargetBoard(Board);
			*State = psWaiting;
		}
		break;
	}
}

void handleSortResults(TboardConfig *Board, TprocessState * State) {
	uns_ch Command;
	uns_ch Response;
	switch (*State) {
	case psInitalisation:
			Decompress_Channels(Board,&sampleBuffer); //Returns the ammount of channels sampled
			*State = psWaiting;
		break;
	case psWaiting:
			CompareResults(Board, &CHval[Board->GlobalTestNum][0]);
			*State = psComplete;
		break;
	case psComplete:
			Board->GlobalTestNum++; //Increment Test Number
			if( CheckTestNumber(Board)) {
				*State = psInitalisation;
				CurrentState = csConfiguring;
			} else {
				if(READ_BIT( Board->BSR, BOARD_TEST_PASSED )) {
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
		case psInitalisation:
				if (READ_BIT(Board->BSR, BOARD_SERIALISED) )
					*State = psComplete;
				else {
					interrogateTargetBoard();
					*State = psWaiting;
				}
			break;
		case psWaiting:
			if(ReceiveState == RxGOOD) {
				tempSerial = ReadSerialNumber(&Response, &Data_Buffer[0], Datalen);
				if (Response == 0x11) {
							//Load Current Serial Number
						memcpy(&CurrentSerial, &tempSerial, 4);
						Board->SerialNumber = read_serial();
							//Write New Serial Number
						Command = 0x12;
						communication_arraySerial(Command, CurrentSerial, Board->SerialNumber);
						*State = psInitalisation;
				} else if (Response == 0x13) {
					if(tempSerial != Board->SerialNumber) {
						Command = 0x10;
						communication_arraySerial(Command, 0, 0);
						*State = psInitalisation;
					} else {
						SET_BIT( Board->BSR, BOARD_SERIALISED );
						*State = psComplete;
					}
				}
			} else if (ReceiveState == RxBAD) {
				*State = psComplete;
			}

			break;
		case psComplete:
				if(READ_BIT(Board->BSR, BOARD_SERIALISED)) {
					printT("=====     Board Serialised     =====\n");
						sprintf(debugTransmitBuffer, "Serial number %d loaded into board\n", BoardConnected.SerialNumber);
						printT(&debugTransmitBuffer[0]);
						CurrentState = csInitialising;
						*State = psInitalisation;
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
		LCD_printf(&previousTestBuffer, 2, 0);
}

void EnterSerialNumber(TboardConfig *Board) {
	uint8 Command;
	LCD_ClearLine(2);
	LCD_ClearLine(3);
	LCD_ClearLine(4);
	LCD_setCursor(2, 0);
	sprintf(debugTransmitBuffer, "Enter Serial Number:");
	LCD_displayString(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));

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
