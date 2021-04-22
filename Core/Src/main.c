/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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
#include "fatfs.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "interogate_project.h"
#include "UART_Routine.h"
#include "Board_Config.h"
#include "calibration.h"
#include "SetVsMeasured.h"
#include "Programming.h"
#include "File_Handling.h"
#include "usbd_cdc_if.h"
#include "DAC_Variables.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
extern void reset_ALL_DAC(void);
extern void Async_Init(void);
extern void delay_us(int);
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c1;

SD_HandleTypeDef hsd;

SPI_HandleTypeDef hspi3;

TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim7;
TIM_HandleTypeDef htim10;
TIM_HandleTypeDef htim11;
TIM_HandleTypeDef htim13;
TIM_HandleTypeDef htim14;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
UART_HandleTypeDef huart6;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C1_Init(void);
static void MX_SDIO_SD_Init(void);
static void MX_SPI3_Init(void);
static void MX_TIM6_Init(void);
static void MX_TIM7_Init(void);
static void MX_TIM13_Init(void);
static void MX_TIM14_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_USART6_UART_Init(void);
static void MX_TIM10_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM11_Init(void);
/* USER CODE BEGIN PFP */
uint32 read_serial(void);
void read_correctionFactors(void);
void write_correctionFactors(void);
void LCD_init(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_SDIO_SD_Init();
  MX_SPI3_Init();
  MX_TIM6_Init();
  MX_TIM7_Init();
  MX_TIM13_Init();
  MX_TIM14_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_USART6_UART_Init();
  MX_FATFS_Init();
  MX_USB_DEVICE_Init();
  MX_TIM10_Init();
  MX_USART1_UART_Init();
  MX_TIM11_Init();
  /* USER CODE BEGIN 2 */
  HAL_GPIO_WritePin(Buffer_OE_GPIO_Port, Buffer_OE_Pin, GPIO_PIN_SET);
//  addSD_DelayVariable = true;
  HAL_I2C_Init(&hi2c1);
  LCD_init();
  TestRig_Init();
  SDfileInit();

//  HAL_TIM_Base_Start_IT(&htim10);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

//=========================================================================================================//


//=========================================================================================================//
	  if ((CurrentState == csIDLE) && (ProcessState == psWaiting) && CheckLoom) {
		  scanLoom(&BoardConnected);
		  checkLoomConnected(&BoardConnected);
		  ConfigInit();
	  }
//=========================================================================================================//


//=========================================================================================================//
	  	  //Testing Functionality
	  if (KP_1.Pressed && (CurrentState == csIDLE) && (ProcessState == psWaiting)) {
	  		KP_1.Pressed = false;
	  		KP_1.Count = 0;
	  		TestRig_Init();
	  		TargetBoardParamInit();
	  		HAL_GPIO_WritePin(PASS_FAIL_GPIO_Port, PASS_FAIL_Pin, GPIO_PIN_RESET);
	  		LCD_ClearLine(4);
	  		LCD_ClearLine(3);
	  		LCD_setCursor(2, 1);
	  		sprintf(debugTransmitBuffer, "    Programming    ");
	  		LCD_printf(&debugTransmitBuffer, strlen(debugTransmitBuffer));
	  		uns_ch Command = 0x10;
//	  		SetPara(Command);
	  		communication_arraySerial(Command, 0, 0);
	  		setTimeOut(200);
	  		CurrentState = csInterogating;
	  		ProcessState = psWaiting;
	  }
//=========================================================================================================//


//=========================================================================================================//
	  if (KP_6.Pressed && (ProcessState == psWaiting) && (CurrentState == csIDLE) ) {
		  reset_ALL_MUX();
		  reset_ALL_DAC();
		  MUX_Sel(Port_1, ASYNC_PULSE);
		  MUX_Sel(Port_2, ASYNC_PULSE);
		  MUX_Sel(Port_3, ASYNC_PULSE);
		  MUX_Sel(Port_4, ASYNC_PULSE);
		  MUX_Sel(Port_5, ASYNC_PULSE);
		  MUX_Sel(Port_6, ASYNC_PULSE);

	  }
//=========================================================================================================//


//=========================================================================================================//
	  	if (ProcessState == psComplete) {
	  		uns_ch Response;
	  		uns_ch Command;
	  		char * fileLocation = malloc(MAX_FILE_NAME_LENGTH * sizeof(char));
	  	    uns_ch tempLine[100];
	  	    uint8 Pos;
	  	    switch(CurrentState) {
	  	    	case csInitialising:	// 0xCC/0xC0 ????
	  	    		if (READ_BIT( BoardConnected.BSR, BOARD_TEST_PASSED )) {
	  	    			SET_BIT( BoardConnected.BSR, BOARD_INITIALISED );
	  	    			communication_response(&Response, &UART2_RXbuffer, UART2_RecPos);
	  	    			if (!READ_BIT(BoardConnected.BSR, BOARD_SERIALISED)) {
							Command = 0x10;
							communication_arraySerial(Command, 0, 0);
							CurrentState = csSerialise;
							ProcessState = psWaiting;
	  	    			} else {
	  	    				timeOutEn = false;
	  	    				CurrentState = csIDLE;
	  	    				ProcessState = psWaiting;
	  	    			}
					} else {
						currentBoardConnected(&BoardConnected);
							//Refresh LCD screen
						if ( !READ_BIT(BoardConnected.BSR, BOARD_SERIALISED)) {
							LCD_ClearLine(2);
							LCD_ClearLine(3);
							LCD_ClearLine(4);
							LCD_setCursor(2, 0);
							sprintf(debugTransmitBuffer, "Enter Serial Number:");
							LCD_printf(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));

							printT("Enter Serial Number: \n");
							BoardConnected.SerialNumber = read_serial();
						}
						if (BoardConnected.SerialNumber) {
							Command = 0x08;
							SetPara(Command);
							HAL_Delay(50);
							communication_array(Command,&Para, Paralen);
							sprintf(debugTransmitBuffer,"Interogating...\n");
							HAL_UART_Transmit(&D_UART, &debugTransmitBuffer[0], strlen(debugTransmitBuffer), HAL_MAX_DELAY);
							CurrentState = csInterogating;
							ProcessState = psWaiting;
						} else {
							LCD_Clear();
							TestRig_Init();
							TestRig_MainMenu();
							CurrentState = csIDLE;
							ProcessState = psWaiting;
						}
					}
	  	    		break;
	  	    	case csProgramming:
	  		  		// Programming Routine
	  	    		ProgrammingInit();
//	  	    		EnableProgramming();
					if( FindBoardFile(&BoardConnected, fileLocation) ) {
						OpenFile(fileLocation);
						printT("\n\n\n");
						uns_ch LineBuffer[MAX_LINE_LENGTH];
						uint8 LineBufferPosition;
						uns_ch PageBuffer[MAX_PAGE_LENGTH];
						uint8 PageBufferPosition = 0;
						uint16 count = 0;
						uns_ch RecBuffer[MAX_PAGE_LENGTH];
						uint16 page = 0;

						while (f_gets(&tempLine, sizeof(tempLine), &fil)) {	//							printT(&tempLine[0]); //print file
					        sortLine(&tempLine, &LineBuffer[0], &LineBufferPosition);
					        Pos = LineBufferPosition;
					        if (populatePageBuffer(&PageBuffer[PageBufferPosition], &PageBufferPosition, &LineBuffer, &LineBufferPosition) ) {
						        uns_ch data[4];
					        	if (page == 0) {
					        		data[0] = 0x4D;
					        		data[1] = 0x00;
					        		data[2] = 0x00;
					        		data[3] = 0x00;
					        		HAL_SPI_Transmit(&hspi3, &data, 4, HAL_MAX_DELAY);
					        	}
//					        	PollReady();
					        	PageWrite(&PageBuffer[0], flashPagelen/2, page);
							    for (uint8 i = 0; i < 128; i++) {
							        uns_ch LowByte;
							        uns_ch HighByte;
							        uns_ch data[4];
							        uns_ch receive[4];
	//						        PollReady();
							        data[0] = 0x20;
							        data[1] = page;
							        data[2] = i;
							        data[3] = 0x00;
							        HAL_SPI_TransmitReceive(&hspi3, &data[0], &receive[0], 4, HAL_MAX_DELAY);
							        LowByte = receive[3];
							        RecBuffer[count++] = LowByte;
	//						        PollReady();
							        data[0] = 0x28;
							        HAL_SPI_TransmitReceive(&hspi3, &data[0], &receive[0], 4, HAL_MAX_DELAY);
							        HighByte = receive[3];
							        RecBuffer[count++] = HighByte;
							        PollReady();
							    }
							    count = 0;
							    page++;
					        	PageBufferPosition = 0;
					        	if(LineBufferPosition)
					        		populatePageBuffer(&PageBuffer[PageBufferPosition], &PageBufferPosition, &LineBuffer[Pos-LineBufferPosition], &LineBufferPosition);
					        }
					    }
						while (PageBufferPosition != 0 ) {
							 PageBuffer[PageBufferPosition++] = 0xFF;
						}
						PageWrite(&PageBuffer[0], flashPagelen/2, page);
					    for (uint8 i = 0; i < 128; i++) {
					        uns_ch LowByte;
					        uns_ch HighByte;
					        uns_ch data[4];
					        uns_ch receive[4];
//						        PollReady();
					        data[0] = 0x20;
					        data[1] = page;
					        data[2] = i;
					        data[3] = 0x00;
					        HAL_SPI_TransmitReceive(&hspi3, &data[0], &receive[0], 4, HAL_MAX_DELAY);
					        LowByte = receive[3];
					        RecBuffer[i*2] = LowByte;
//						        PollReady();
					        data[0] = 0x28;
					        HAL_SPI_TransmitReceive(&hspi3, &data[0], &receive[0], 4, HAL_MAX_DELAY);
					        HighByte = receive[3];
					        RecBuffer[i*2+1] = HighByte;
					        PollReady();
					    }
						printT("Programming Done\n");
						//TODO: Verify programming here
						Close_File(fileLocation);

						count = PageBufferPosition = page = 0;

				        uns_ch data[4];
			        	if (page == 0) {
			        		data[0] = 0x4D;
			        		data[1] = 0x00;
			        		data[2] = 0x00;
			        		data[3] = 0x00;
			        		HAL_SPI_Transmit(&hspi3, &data, 4, HAL_MAX_DELAY);
			        	}
							// Verify Board Programming
						for (page = 0;page < 256;page++) {
						    for (uint8 i = 0; i < 128; i++) {
						        uns_ch LowByte;
						        uns_ch HighByte;
						        uns_ch data[4];
						        uns_ch receive[4];
//						        PollReady();
						        data[0] = 0x20;
						        data[1] = page;
						        data[2] = i;
						        data[3] = 0x00;
						        HAL_SPI_TransmitReceive(&hspi3, &data[0], &receive[0], 4, HAL_MAX_DELAY);
						        LowByte = receive[3];
						        RecBuffer[i*2] = LowByte;
//						        PollReady();
						        data[0] = 0x28;
						        HAL_SPI_TransmitReceive(&hspi3, &data[0], &receive[0], 4, HAL_MAX_DELAY);
						        HighByte = receive[3];
						        RecBuffer[i*2+1] = HighByte;
						    }
						    count = 0;
						}
//						OpenFile(&fileLocation);
//						while(f_gets(&tempLine, 100, &fil)) {
//							sortLine(&tempLine, &LineBuffer[0], &LineBufferPosition);
//					        if (populatePageBuffer(&PageBuffer[PageBufferPosition], &PageBufferPosition, &LineBuffer, &LineBufferPosition) ) {
//					        	for (int i = 0; i < 128; i++) {
//					        		uns_ch data[4];
//					        		uns_ch receive[4];
//					        		PollReady();
//					        		data[0] = 0x20;
//					        		data[1] = page;
//					        		data[2] = i;
//					        		data[3] = 0x00;
//					        		HAL_SPI_TransmitReceive(&hspi3, &data, &receive, 4, HAL_MAX_DELAY);
//					        		PollReady();
//					        		RecBuffer[count] = receive[3];
//					        		data[0] = 0x28;
//					        		data[1] = page;
//					        		data[2] = i;
//					        		data[3] = 0x00;
//					        		HAL_SPI_TransmitReceive(&hspi3, &data, &receive, 4, HAL_MAX_DELAY);
//					        		RecBuffer[count+1] = receive[3];
//					        		if ((RecBuffer[count] != PageBuffer[i]) && (RecBuffer[count+1] != PageBuffer[i+1] ) ) {
//					        			_Bool Program = false;
//					        		}
//					        		count += 2;
//
//					        	}
//				        		page++;
//					        	count = PageBufferPosition = 0;
//					        	if(LineBufferPosition)
//					        		populatePageBuffer(&PageBuffer[0], &PageBufferPosition, &LineBuffer[Pos-LineBufferPosition], &LineBufferPosition);
//					        }
//
//						}
//						Close_File(&fileLocation);
						SPI3->CR1 &= ~(SPI_BAUDRATEPRESCALER_32);
						SPI3->CR1 |= (0xFF & SPI_BAUDRATEPRESCALER_256);
						SetClkAndLck();
						HAL_GPIO_WritePin(TB_Reset_GPIO_Port, TB_Reset_Pin, GPIO_PIN_SET);

						SET_BIT(BoardConnected.BSR, BOARD_PROGRAMMED);
						Command = 0x08;
						SetPara(Command);
						communication_array(Command, &Para, Paralen);
						CurrentState = csInterogating;
						ProcessState = psWaiting;
					} else {

					}
	  	    		break;
	  	    	case csCalibrating:
	  	    		//TODO: Implement calibration counter here for if the calibration fails, retry 3 times,
	  	    		communication_response(&Response, &UART2_RXbuffer, UART2_RecPos);
	  	    		if ( Response == 0xC1 ) {
  	            		initialiseTargetBoard();
  	            		CurrentState = csInitialising;
	  	    			ProcessState = psWaiting;
	  	    		} else {
	  	    			//TODO: add timeout and repeat transmission
	  	    		}
	  	    		break;
	  	        case csInterogating: //0x09 & 0x35
	  	        	communication_response(&Response, &UART2_RXbuffer[0], UART2_RecPos);
	  	            if (Response == 0x09) {
	  	            	communication_command(&Response);
	  	            	if (READ_BIT( BoardConnected.BSR, BOARD_CALIBRATED)) {	//Check if board has been calibrated yet
		  	                Command = 0x56;
		  	                SetPara(Command);
		  	                communication_array(Command,&Para[0], Paralen);
		  	                ProcessState = psWaiting;
		  	                CurrentState = csConfiguring;
	  	            	} else {
	  	            		switchToCurrent = false;
	  	            		HAL_Delay(50);
	  	            		TargetBoardCalibration();
	  	            		CurrentState = csCalibrating;
	  	            		ProcessState = psWaiting;
	  	            	}
	  	            } else if (Response == 0x11) {
	  	            	if (~BoardConnected.SerialNumber) {
	  	            		ContinueWithCurrentSerial();
	  	            		LCD_ClearLine(4);
	  	            	}
	  	            	if (BoardConnected.Version < getCurrentVersion(BoardConnected.BoardType) ) {
	  	            		CurrentState = csProgramming;
	  	            		ProcessState = psComplete;
	  	            	} else { //TODO: Put the user choice to reprogram the board here
	  	            		if (ContinueWithCurrentProgram() ) {
		  	            		CurrentState = csProgramming;
		  	            		ProcessState = psComplete;
	  	            		} else {
	  		  	            	Command = 0x08;
	  		  	            	SetPara(Command);
	  		  	            	communication_array(Command, &Para, Paralen);
	  	  	            		CurrentState = csInterogating;
	  	  	            		ProcessState = psWaiting;
	  	            		}
	  	            		LCD_ClearLine(4);
	  	            	}
	  	            }
	  	            break;
	  	        case csConfiguring: // 0x57
						communication_response(&Response, &UART2_RXbuffer, UART2_RecPos);
						if (Response == 0x57) {
							communication_command(&Response);
							Command = 0x1A;
							SetPara(Command);
							communication_array(Command,&Para, Paralen);
							ProcessState = psWaiting;
							CurrentState = csSampling;
						}
	  	            break;
	  	        case csSampling: // 0x1B
						communication_response(&Response, &UART2_RXbuffer, UART2_RecPos);
						communication_command(&Response);
						ProcessState = psWaiting;
						CurrentState = csUploading;
	  	            break;
	  	        case csUploading: // 0x19
						Command = 0x18;
						SetPara(Command);
						communication_array(Command,&Para, Paralen);
						ProcessState = psWaiting;
						CurrentState = csSortResults;
	  	            break;
	  	        case csSortResults:
						communication_response(&Response, &UART2_RXbuffer, UART2_RecPos);
						if(Response == 0x03) {
							CurrentState = csUploading;
							ProcessState = psWaiting;
						} else if (BoardConnected.GlobalTestNum <= BoardConnected.testNum) {
							if( CheckTestNumber(&BoardConnected)) {
								Command = 0x56;
								ProcessState = psWaiting;
								CurrentState = csConfiguring;
								SetPara(Command);
								communication_array(Command,&Para, Paralen);
							} else {
							if(READ_BIT( BoardConnected.BSR, BOARD_TEST_PASSED )) {
								LCD_setCursor(2, 0);
								sprintf(debugTransmitBuffer, "    Test Passed    ");
								LCD_printf(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
								HAL_GPIO_WritePin(PASS_FAIL_GPIO_Port, PASS_FAIL_Pin, GPIO_PIN_SET);
								timeOutEn = false;
								ProcessState = psComplete;
								CurrentState = csIDLE;
							} else {
								LCD_setCursor(2, 0);
								sprintf(debugTransmitBuffer, "    Test Failed    ");
								LCD_printf(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
								HAL_GPIO_WritePin(PASS_FAIL_GPIO_Port, PASS_FAIL_Pin, GPIO_PIN_RESET);
								timeOutEn = false;
								ProcessState = psWaiting;
								CurrentState = csIDLE;
									}
								}
						}
	  	        	break;
	  	        case csSerialise:
					if (Command == 0x10) {
						uint32 tempSerial;
						uint32 CurrentSerial;
						uint32 NewSerial;
						tempSerial = ReadSerialNumber(&UART2_RXbuffer[0], UART2_RecPos);
							//Load Current Serial Number
						memcpy(&CurrentSerial, &tempSerial, 4);
							//Write New Serial Number
						memcpy(&NewSerial, &(BoardConnected.SerialNumber), 4);
						Command = 0x12;
						communication_arraySerial(Command, CurrentSerial, NewSerial);
						ProcessState = psWaiting;
					} else if (Command == 0x12) {
						uint32 tempSerial;
						tempSerial = ReadSerialNumber(&UART2_RXbuffer[0], UART2_RecPos);
						if(tempSerial != BoardConnected.SerialNumber) {
							Command = 0x10;
							communication_arraySerial(Command, 0, 0);
							ProcessState = psWaiting;
						} else {
							SET_BIT( BoardConnected.BSR, BOARD_SERIALISED );
							sprintf(debugTransmitBuffer, "=====     Board Serialised     =====\n");
							HAL_UART_Transmit(&huart1, &debugTransmitBuffer[0], strlen(debugTransmitBuffer), HAL_MAX_DELAY);
							sprintf(debugTransmitBuffer, "Serial number %u loaded into board\n", BoardConnected.SerialNumber);
							HAL_UART_Transmit(&huart1, &debugTransmitBuffer[0], strlen(debugTransmitBuffer), HAL_MAX_DELAY);
							ProcessState = psWaiting;
							CurrentState = csIDLE;
						}
					}
	  	        	break;
	  	        case csIDLE:
	  	        		  initialiseTargetBoard();
	  	        		  CurrentState = csInitialising;
	  	        		  ProcessState = psWaiting;
	  	        	break;

	  	    }
	  	} else if (ProcessState == psFailed) {
	  		sprintf(debugTransmitBuffer, "=========     Timeout Failure     =========\n");
			HAL_UART_Transmit(&D_UART, &debugTransmitBuffer, strlen(debugTransmitBuffer), HAL_MAX_DELAY);
			uns_ch Command;
			timeOutEn = false;
			switch (CurrentState) {
				case csInterogating:
					if (!READ_BIT(BoardConnected.BSR, BOARD_PROGRAMMED)) {
						currentBoardConnected(&BoardConnected);
						CurrentState = csProgramming;
						ProcessState = psComplete;
					} else if ( !READ_BIT(BoardConnected.BSR, BOARD_SERIALISED) ) {
						Command = 0x08;
						SetPara(Command);
						communication_array(Command, &Para, Paralen);
						ProcessState = psWaiting;
					} else {
						CurrentState = csIDLE;
						ProcessState = psWaiting;
					}
					break;
				default:
					CurrentState = csIDLE;
					ProcessState = psWaiting;
					break;
			}
	  	}

	  		// Quit
	  	if (KP_star.Pressed) {
			KP_star.Pressed = false;
			KP_star.Count = 0;
			QuitCount++;
  			if (QuitCount == 5) {
  				LCD_Clear();
  				TestRig_Init();
  				TestRig_MainMenu();
  				QuitCount = 0;
  			}
	  	}
	  			// Scan I2C
		  if (KP_5.Pressed && (CurrentState == csIDLE) && (ProcessState == psWaiting)) {
			  	KP_5.Pressed = KP_5.Count = 0;
			  	sprintf(debugTransmitBuffer, "Scanning I2C bus:\r\n");
			  	CDC_Transmit_FS(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
				  HAL_UART_Transmit(&huart1, &debugTransmitBuffer[0], strlen(debugTransmitBuffer), HAL_MAX_DELAY);
			  	HAL_StatusTypeDef result;
			  	uint8_t i;
			  	for (i=1; i<128; i++) {
			  	  result = HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)(i<<1), 2, 2);
			  	  if (result != HAL_OK) {// HAL_ERROR or HAL_BUSY or HAL_TIMEOUT
			  		  sprintf(debugTransmitBuffer, "."); // No ACK received at that address
			  		  HAL_UART_Transmit(&huart1, &debugTransmitBuffer[0], strlen(debugTransmitBuffer), HAL_MAX_DELAY);
			  	  }
			  	  if (result == HAL_OK) {
			  		  sprintf(debugTransmitBuffer, "0x%X", i); // Received an ACK at that address
			  	  }
				  	CDC_Transmit_FS(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
					  HAL_UART_Transmit(&huart1, &debugTransmitBuffer[0], strlen(debugTransmitBuffer), HAL_MAX_DELAY);
			  	}
		  }
		  	  //Calibration Routine
		  if((KP_7.Pressed && KP_9.Pressed) && (CurrentState == csIDLE) && (ProcessState == psWaiting)){
			  KP_7.Count = KP_7.Pressed = 0;
			  KP_9.Count = KP_9.Pressed = 0;

			  LCD_Clear();
			  LCD_setCursor(1, 6);
			  sprintf(debugTransmitBuffer,"Test Rig");
			  LCD_printf(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));

			  LCD_setCursor(2, 0);
			  sprintf(debugTransmitBuffer, "Calibrate Test Rig");
			  LCD_printf(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));

			  LCD_setCursor(3, 5);
			  sprintf(debugTransmitBuffer, "1V - Port 1");
			  LCD_printf(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));

			  sprintf(debugTransmitBuffer,"\n\n==========  Calibration Routine  ==========\n");
			  CDC_Transmit_FS(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
			  HAL_UART_Transmit(&D_UART, &debugTransmitBuffer[0], strlen(debugTransmitBuffer), HAL_MAX_DELAY);
			  Calibration();
			  LCD_Clear();
			  TestRig_Init();
			  TestRig_MainMenu();
			  sprintf(debugTransmitBuffer,"\n\n==========  Test Rig  ==========\n");
			  CDC_Transmit_FS(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
			  HAL_UART_Transmit(&D_UART, &debugTransmitBuffer[0], strlen(debugTransmitBuffer), HAL_MAX_DELAY);
		  }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SDIO|RCC_PERIPHCLK_CLK48;
  PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48CLKSOURCE_PLLQ;
  PeriphClkInitStruct.SdioClockSelection = RCC_SDIOCLKSOURCE_CLK48;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ENABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_112CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SDIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_SDIO_SD_Init(void)
{

  /* USER CODE BEGIN SDIO_Init 0 */

  /* USER CODE END SDIO_Init 0 */

  /* USER CODE BEGIN SDIO_Init 1 */

  /* USER CODE END SDIO_Init 1 */
  hsd.Instance = SDIO;
  hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
  hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
  hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
  hsd.Init.BusWide = SDIO_BUS_WIDE_1B;
  hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd.Init.ClockDiv = 16;
  /* USER CODE BEGIN SDIO_Init 2 */

  /* USER CODE END SDIO_Init 2 */

}

/**
  * @brief SPI3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI3_Init(void)
{

  /* USER CODE BEGIN SPI3_Init 0 */

  /* USER CODE END SPI3_Init 0 */

  /* USER CODE BEGIN SPI3_Init 1 */

  /* USER CODE END SPI3_Init 1 */
  /* SPI3 parameter configuration*/
  hspi3.Instance = SPI3;
  hspi3.Init.Mode = SPI_MODE_MASTER;
  hspi3.Init.Direction = SPI_DIRECTION_2LINES;
  hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi3.Init.NSS = SPI_NSS_SOFT;
  hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
  hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi3.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI3_Init 2 */

  /* USER CODE END SPI3_Init 2 */

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 4799;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 99;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief TIM7 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM7_Init(void)
{

  /* USER CODE BEGIN TIM7_Init 0 */

  /* USER CODE END TIM7_Init 0 */

  /* USER CODE BEGIN TIM7_Init 1 */

  /* USER CODE END TIM7_Init 1 */
  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 999;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.Period = 47;
  htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM7_Init 2 */

  /* USER CODE END TIM7_Init 2 */

}

/**
  * @brief TIM10 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM10_Init(void)
{

  /* USER CODE BEGIN TIM10_Init 0 */

  /* USER CODE END TIM10_Init 0 */

  /* USER CODE BEGIN TIM10_Init 1 */

  /* USER CODE END TIM10_Init 1 */
  htim10.Instance = TIM10;
  htim10.Init.Prescaler = 119;
  htim10.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim10.Init.Period = 1;
  htim10.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim10.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim10) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM10_Init 2 */

  /* USER CODE END TIM10_Init 2 */

}

/**
  * @brief TIM11 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM11_Init(void)
{

  /* USER CODE BEGIN TIM11_Init 0 */

  /* USER CODE END TIM11_Init 0 */

  /* USER CODE BEGIN TIM11_Init 1 */

  /* USER CODE END TIM11_Init 1 */
  htim11.Instance = TIM11;
  htim11.Init.Prescaler = 999;
  htim11.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim11.Init.Period = 47;
  htim11.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim11.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim11) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM11_Init 2 */

  /* USER CODE END TIM11_Init 2 */

}

/**
  * @brief TIM13 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM13_Init(void)
{

  /* USER CODE BEGIN TIM13_Init 0 */

  /* USER CODE END TIM13_Init 0 */

  /* USER CODE BEGIN TIM13_Init 1 */

  /* USER CODE END TIM13_Init 1 */
  htim13.Instance = TIM13;
  htim13.Init.Prescaler = 48;
  htim13.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim13.Init.Period = 50;
  htim13.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim13.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim13) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM13_Init 2 */

  /* USER CODE END TIM13_Init 2 */

}

/**
  * @brief TIM14 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM14_Init(void)
{

  /* USER CODE BEGIN TIM14_Init 0 */

  /* USER CODE END TIM14_Init 0 */

  /* USER CODE BEGIN TIM14_Init 1 */

  /* USER CODE END TIM14_Init 1 */
  htim14.Instance = TIM14;
  htim14.Init.Prescaler = 480;
  htim14.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim14.Init.Period = 200;
  htim14.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim14.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim14) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM14_Init 2 */

  /* USER CODE END TIM14_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 9600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_RTS_CTS;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief USART6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART6_UART_Init(void)
{

  /* USER CODE BEGIN USART6_Init 0 */

  /* USER CODE END USART6_Init 0 */

  /* USER CODE BEGIN USART6_Init 1 */

  /* USER CODE END USART6_Init 1 */
  huart6.Instance = USART6;
  huart6.Init.BaudRate = 1200;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_EVEN;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART6_Init 2 */

  /* USER CODE END USART6_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, V12fuseEN_Pin|MUX_WRodd3_Pin|MUX_WReven3_Pin|MUX_A0_Pin
                          |MUX_A1_Pin|MUX_RS_Pin|MUX_EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, PASS_FAIL_Pin|ADC_MUX_A_Pin|ADC_MUX_B_Pin|Buffer_OE_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOF, KP_R1_Pin|KP_R2_Pin|KP_R3_Pin|KP_R4_Pin
                          |MUX_WRodd1_Pin|MUX_WReven1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LED1_Pin|LED2_Pin|DAC_CS3_Pin|DAC_CS2_Pin
                          |DAC_CS1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, MUX_WRodd2_Pin|MUX_WReven2_Pin|ASYNC3_Pin|ASYNC4_Pin
                          |ASYNC5_Pin|ASYNC6_Pin|ASYNC7_Pin|ASYNC8_Pin
                          |ASYNC9_Pin|V12fuseLatch_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, ASYNC1_Pin|ASYNC2_Pin|TB_Reset_Pin|res_LoomSel_Pin
                          |Radio_EN_Pin|RS485_EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(RS485_4011EN_GPIO_Port, RS485_4011EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : V12fuseEN_Pin MUX_WRodd3_Pin MUX_WReven3_Pin MUX_A0_Pin
                           MUX_A1_Pin MUX_RS_Pin MUX_EN_Pin */
  GPIO_InitStruct.Pin = V12fuseEN_Pin|MUX_WRodd3_Pin|MUX_WReven3_Pin|MUX_A0_Pin
                          |MUX_A1_Pin|MUX_RS_Pin|MUX_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PASS_FAIL_Pin ADC_MUX_A_Pin ADC_MUX_B_Pin Buffer_OE_Pin */
  GPIO_InitStruct.Pin = PASS_FAIL_Pin|ADC_MUX_A_Pin|ADC_MUX_B_Pin|Buffer_OE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : KP_R1_Pin KP_R2_Pin KP_R3_Pin KP_R4_Pin
                           MUX_WRodd1_Pin MUX_WReven1_Pin */
  GPIO_InitStruct.Pin = KP_R1_Pin|KP_R2_Pin|KP_R3_Pin|KP_R4_Pin
                          |MUX_WRodd1_Pin|MUX_WReven1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pins : KP_C1_Pin KP_C2_Pin KP_C3_Pin */
  GPIO_InitStruct.Pin = KP_C1_Pin|KP_C2_Pin|KP_C3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pins : LED1_Pin LED2_Pin DAC_CS3_Pin DAC_CS2_Pin
                           DAC_CS1_Pin */
  GPIO_InitStruct.Pin = LED1_Pin|LED2_Pin|DAC_CS3_Pin|DAC_CS2_Pin
                          |DAC_CS1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : MUX_WRodd2_Pin MUX_WReven2_Pin ASYNC3_Pin ASYNC4_Pin
                           ASYNC5_Pin ASYNC6_Pin ASYNC7_Pin ASYNC8_Pin
                           ASYNC9_Pin V12fuseLatch_Pin */
  GPIO_InitStruct.Pin = MUX_WRodd2_Pin|MUX_WReven2_Pin|ASYNC3_Pin|ASYNC4_Pin
                          |ASYNC5_Pin|ASYNC6_Pin|ASYNC7_Pin|ASYNC8_Pin
                          |ASYNC9_Pin|V12fuseLatch_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pins : ASYNC1_Pin ASYNC2_Pin TB_Reset_Pin res_LoomSel_Pin
                           Radio_EN_Pin RS485_EN_Pin */
  GPIO_InitStruct.Pin = ASYNC1_Pin|ASYNC2_Pin|TB_Reset_Pin|res_LoomSel_Pin
                          |Radio_EN_Pin|RS485_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : RS485_4011EN_Pin */
  GPIO_InitStruct.Pin = RS485_4011EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(RS485_4011EN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : Loom_Sel_Pin */
  GPIO_InitStruct.Pin = Loom_Sel_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(Loom_Sel_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : SD_Det_Pin */
  GPIO_InitStruct.Pin = SD_Det_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(SD_Det_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
