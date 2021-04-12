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
	  sprintf(debugTransmitBuffer,"==========TestRig========== \n");
	  CDC_Transmit_FS(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
	  HAL_GPIO_WritePin(TB_Reset_GPIO_Port, TB_Reset_Pin, GPIO_PIN_SET);
	  HAL_UART_Transmit(&huart1, &debugTransmitBuffer[0], strlen(debugTransmitBuffer), HAL_MAX_DELAY);

	  LCD_setCursor(1, 0);
	  LCD_CursorOn_Off(false);
	  sprintf(debugTransmitBuffer,"      Test Rig      ");
	  LCD_printf(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));

	  Para[0] = 0;
	  Paralen = 0;

	  UART2_RecPos = 0;
	  samplesUploading = false;
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
	  HAL_TIM_Base_Start_IT(&htim14);

	  LoomChecking = true;
	  LoomState = bNone;
	  PrevLoomState = bNone;
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
}

void TargetBoardParamInit() {
		//Dont Clear BoardType, can only be accessed from scanLoom()
	BoardConnected.SerialNumber = 0;
	BoardConnected.analogInputCount = 0;
	BoardConnected.digitalInputCout = 0;
	BoardConnected.latchPortCount = 0;
	BoardConnected.testNum = 0;
	BoardConnected.GlobalTestNum = 0;
	BoardConnected.BSR = 0;
	for (int i = 0; i < 15; i++) {
		TestResults[i] = true;
	}
	Vfuse.steadyState = 0;
	Vin.steadyState = 0;
}

uint32 ReadSerialNumber(uns_ch * data, uint16 length) {
	uns_ch *ptr;
	uns_ch Response;
	uint32 SerialNumberRead = 0;
	uint16 Length;
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

void setTimeOut(uint16 wait) {
	timeOutEn = true;
	timeOutCount = wait;
}

void LatchTestInit() {

}

uint8 getCurrentVersion(TloomConnected Board) {
		switch (Board) {
			case b935x:
					return BOARD_935x;
				break;
			case b937x:
					return BOARD_937x;
				break;
			case b401x:
					return BOARD_401x;
				break;
			case b402x:
				return BOARD_402x;
				break;
			case b422x:
				return BOARD_422x;
				break;
			case b427x:
				return BOARD_427x;
				break;
		}
	return 255;
}
