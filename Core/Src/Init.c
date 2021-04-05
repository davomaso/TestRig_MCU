/*
 * Init.c
 *
 *  Created on: 24 Mar 2021
 *      Author: mason
 */
#include "main.h"
#include "interogate_project.h"
#include "SetVsMeasured.h"
#include "UART_Routine.h"

void TestRig_Init() {
	  sprintf(Buffer,"==========TestRig========== \n");
	  CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
	  HAL_GPIO_WritePin(TB_Reset_GPIO_Port, TB_Reset_Pin, GPIO_PIN_SET);
	  HAL_UART_Transmit(&huart1, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);

	  LCD_setCursor(1, 0);
	  LCD_CursorOn_Off(false);
	  sprintf(Buffer,"      Test Rig      ");
	  LCD_printf(&Buffer[0], strlen(Buffer));

//	  LoomConnected = None;
//	  LCD_setCursor(2, 0);
//	  sprintf(Buffer,"   Connect a Loom   ");
//	  LCD_printf(&Buffer[0], strlen(Buffer));
//	  HAL_UART_Transmit(&huart1, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);


	  Para[0] = 0;
	  Paralen = 0;
	  Length = 14 + Paralen; //length is
	  Comlen = Length + 3; // plus 5 for the header, length and CRC
	  SDIAddress = 255;

	  UART2_RecPos = 0;
	  TestingComplete = true;
	  samplesUploading = false;
	  sampleUploadComplete = false;
	  GlobalTestNum = 0;
	  HAL_GPIO_WritePin(PASS_FAIL_GPIO_Port, PASS_FAIL_Pin, GPIO_PIN_RESET);
	  //Set DAC to zero
	  reset_ALL_DAC();
	  HAL_GPIO_WritePin(MUX_RS_GPIO_Port, MUX_RS_GPIO_Port, GPIO_PIN_SET);
	  HAL_GPIO_WritePin(MUX_EN_GPIO_Port, MUX_EN_Pin, GPIO_PIN_SET);
	  reset_ALL_MUX();


	  Async_Port1.Active = false;
	  Async_Port2.Active = false;
	  Async_Port3.Active = false;
	  Async_Port4.Active = false;
	  Async_Port5.Active = false;
	  Async_Port6.Active = false;
	  Async_Port7.Active = false;
	  Async_Port8.Active = false;
	  Async_Port9.Active = false;

	  HAL_TIM_Base_Start_IT(&htim6);
	  HAL_TIM_Base_Start_IT(&htim7);
//	  HAL_TIM_Base_Start_IT(&htim10);
	  HAL_TIM_Base_Start_IT(&htim14);

	  LoomChecking = true;

	  for (int i = 0; i < 15; i++) {
		  TestResults[i] = true;
	  }

	  LoomState = None;
	  PrevLoomState = None;
	  read_correctionFactors();


	  //Mount SD and check space
//	  SDfileInit();
//	  LCD_setCursor(2, 0);
//	  sprintf(Buffer, " SD card Connected  ");
//	  LCD_printf(&Buffer[0], strlen(Buffer));
      HAL_GPIO_WritePin(V12fuseEN_GPIO_Port, V12fuseEN_Pin, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(V12fuseLatch_GPIO_Port, V12fuseLatch_Pin, GPIO_PIN_RESET);
}

void initialiseTargetBoard() {
		uns_ch Command;
		Command = 0xCC;
		SetPara(Command);
		communication_array(Command, &Para[0], Paralen);
		while(!UART2_ReceiveComplete) {

		}
}

void TargetBoardParamInit() {
	BoardConnected.BoardType = 0;
	BoardConnected.SerialNumber = 0;
	BoardConnected.analogInputCount = 0;
	BoardConnected.digitalInputCout = 0;
	BoardConnected.latchPortCount = 0;
	BoardConnected.testNum = 0;
	TestPassed = false;
	BoardCalibrated = false;
	for (int i = 0; i < 15; i++) {
		TestResults[i] = true;
	}
	Vfuse.steadyState = 0;
	Vin.steadyState = 0;
}

void WriteSerialNumber() {
	uns_ch Command = 0x10;

	uint32 CurrentSerial;
	uint32 NewSerial;
	uint8 SerialLength;
	uint32 tempSerial;

		//Read Current Serial Number
	CurrentSerial = NewSerial = 0;
	communication_arraySerial(Command, CurrentSerial, NewSerial);
	while (!UART2_ReceiveComplete) {
	}
	tempSerial = ReadSerialNumber(&UART2_Receive[0], UART2_RecPos);
		//Load Current Serial Number
	memcpy(&CurrentSerial, &tempSerial, 4);

		//Write New Serial Number
	memcpy(&NewSerial, &(BoardConnected.SerialNumber), 4);

	Command = 0x12;
	communication_arraySerial(Command, CurrentSerial, NewSerial);
	while (!UART2_ReceiveComplete) {
	}
		//Confirm Serial Number
	tempSerial = ReadSerialNumber(&UART2_Receive[0], UART2_RecPos);
	if(tempSerial != BoardConnected.SerialNumber) {
		WriteSerialNumber();
	} else {
		sprintf(Buffer, "=====     Board Serialised     =====\n");
		HAL_UART_Transmit(&huart1, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);
		sprintf(Buffer, "Serial number %u loaded into board\n", BoardConnected.SerialNumber);
		HAL_UART_Transmit(&huart1, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);
	}
}

uint32 ReadSerialNumber(uns_ch * data, uint16 length) {
	uns_ch *ptr;
	uns_ch Response;
	uint32 SerialNumberRead = 0;
	//Stop re-entry into communication Routines
	UART2_ReceiveComplete = false;
	if (CRC_Check(data, length)) {
		//from here break the Receive Buffer array down
		//Length
		Length = *(data + 2);
		//	C/R will determine how the system will behave following
		Response = *(data + 7);
		ptr = data + 8;
		switch (Response) {
		case 0x11:
		case 0x13:
				memcpy(&SerialNumberRead, ptr, 4);
			return SerialNumberRead;
			break;
		}
	}
	return 0;
}

void LatchTestInit() {

}
