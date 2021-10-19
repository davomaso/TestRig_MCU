/*
 * Init.c
 *
 *  Created on: 24 Mar 2021
 *      Author: mason
 */
#include "main.h"
#include "Init.h"
#include "string.h"
#include <Calibration.h>
#include "Global_Variables.h"
#include "SetVsMeasured.h"
#include "DAC.h"
#include "LCD.h"
#include "Communication.h"
#include "TestFunctions.h"
#include "UART_Routine.h"

void TestRig_Init() {
	printT((uns_ch*) "==========TestRig========== \n");
	HAL_GPIO_WritePin(TB_Reset_GPIO_Port, TB_Reset_Pin, GPIO_PIN_SET);

	LCD_CursorOn_Off(false);
	LCD_printf((uns_ch*) "Test Rig      ", 1, 0);
	sprintf((char*) &lcdBuffer, "Firmware: v%.1f", FIRMWARE_VERSION);
	LCD_printf((uns_ch*) &lcdBuffer, 2, 0);
	BoardCommsParameters[0] = 0;
	BoardCommsParametersLength = 0;

	BoardCommsReceiveState = RxNone;
	samplesUploading = false;

	ASYNCinit();
	SDIinit();
	keypadInit();
	reset_ALL_DAC();	//Set DAC to zero
	HAL_GPIO_WritePin(MUX_RS_GPIO_Port, MUX_RS_GPIO_Port, GPIO_PIN_SET);
	HAL_GPIO_WritePin(MUX_EN_GPIO_Port, MUX_EN_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(RS485_4011EN_GPIO_Port, RS485_4011EN_Pin, GPIO_PIN_RESET);
	reset_ALL_MUX();

	HAL_TIM_Base_Start_IT(&htim6); 	// LED blink, Scan Loom timer
	HAL_TIM_Base_Start_IT(&htim7);	// Scan keypad timer
	HAL_TIM_Base_Start_IT(&htim10);	// Calibration & Latch test timer
	HAL_TIM_Base_Start_IT(&htim11);	// Timeout, Input/Solar Voltage, Sampling timer
	HAL_TIM_Base_Start_IT(&htim14);	// Async Timer

	timeOutCount = 0;	//General Timeout initialisation
	timeOutEn = false;
}

void initialiseTargetBoard(TboardConfig *Board) {
	LCD_printf((uns_ch*) "    Initialising    ", 2, 0);

	uns_ch Command;
	Command = 0xCC;
	BoardCommsParameters[0] = 0x49;
	BoardCommsParametersLength = 1;
	communication_array(Command, &BoardCommsParameters[0], BoardCommsParametersLength);
}

void interrogateTargetBoard() {
	if(CurrentState != csIDLE){
		if (CurrentState == csSerialise)
			LCD_printf((uns_ch *) "    Serialising    ", 2, 0);
		else
			LCD_printf((uns_ch *) "   Interrogating    ", 2, 0);
	}
	uns_ch Command;
	Command = 0x10;
	communication_arraySerial(Command, 0, 0);
}

void configureTargetBoard(TboardConfig *Board) {
	uns_ch Command;
	Command = 0x56;
	SetPara(Board, Command);
	if (Board->BoardType != b422x)	// No need to reconfigure as board is configured with the initialisation
		communication_array(Command, &BoardCommsParameters[0], BoardCommsParametersLength);
	OutputsSet = false;
}

void uploadSamplesTargetBoard(TboardConfig *Board) {
	uns_ch Command;
	Command = 0x18;
	SetPara(Board, Command);
	communication_array(Command, &BoardCommsParameters[0], BoardCommsParametersLength);
}

void TargetBoardParamInit(_Bool FullErase) {
	TloomConnected TempBoardType;
	uns_ch TempSubClass;
	uint32 TempSerial;
	if (!FullErase) {
		TempBoardType = BoardConnected.BoardType;
		TempSubClass = BoardConnected.Subclass;
		TempSerial = BoardConnected.SerialNumber;
		memset(&BoardConnected, 0, sizeof(TboardConfig));
		//Dont Clear BoardType, can only be accessed from scanLoom()
		BoardConnected.BoardType = TempBoardType;
		BoardConnected.Subclass = TempSubClass;
		BoardConnected.SerialNumber = TempSerial;
	} else
		memset(&BoardConnected, 0, sizeof(TboardConfig));
	memset(&Vfuse, 0, sizeof(TADCconfig));
	memset(&Vin, 0, sizeof(TADCconfig));
}

uint32 ReadSerialNumber(uint8 *Response, uns_ch *data, uint16 length) {
	uint32 SerialNumberRead = 0;
	//Stop re-entry into communication Routines
	UART2_ReceiveComplete = false;
	*Response = *(data + 4);
	switch (*Response) {
	case 0x11:
	case 0x13:
		memcpy(&SerialNumberRead, data + 5, 4);
		return SerialNumberRead;
		break;
	}
	return 0;
}

void setTimeOut(uint16 wait) {
	timeOutEn = true;
	timeOutCount = wait;
	HAL_TIM_Base_Start_IT(&htim11);
}

void runLatchTimeOut(uint16 wait) {
	latchTimeOutEn = true;
	LatchTimeOut = true;
	latchTimeOutCount = wait;
	HAL_TIM_Base_Start_IT(&htim11);
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
	case bNone:
		return 255;
		break;
	}
	return 255;
}

void SDIinit() {
	HAL_GPIO_WritePin(Buffer_OE_GPIO_Port, Buffer_OE_Pin, GPIO_PIN_SET);	// SDI-12 Buffer enable, Tristate the BUS
	Port[Port_1].Sdi.setValue = 7.064;
	Port[Port_2].Sdi.setValue = 9.544;
	Port[Port_3].Sdi.setValue = 4.408;
	Port[Port_4].Sdi.setValue = 6.515;
	Port[Port_5].Sdi.setValue = 5.892;
	Port[Port_6].Sdi.setValue = 6.922;
}

void ASYNCinit() {
	Port[Port_1].Async.Port = ASYNC1_GPIO_Port;
	Port[Port_1].Async.Pin = ASYNC1_Pin;
	Port[Port_2].Async.Port = ASYNC2_GPIO_Port;
	Port[Port_2].Async.Pin = ASYNC2_Pin;
	Port[Port_3].Async.Port = ASYNC3_GPIO_Port;
	Port[Port_3].Async.Pin = ASYNC3_Pin;
	Port[Port_4].Async.Port = ASYNC4_GPIO_Port;
	Port[Port_4].Async.Pin = ASYNC4_Pin;
	Port[Port_5].Async.Port = ASYNC5_GPIO_Port;
	Port[Port_5].Async.Pin = ASYNC5_Pin;
	Port[Port_6].Async.Port = ASYNC6_GPIO_Port;
	Port[Port_6].Async.Pin = ASYNC6_Pin;
	Port[Port_7].Async.Port = ASYNC7_GPIO_Port;
	Port[Port_7].Async.Pin = ASYNC7_Pin;
	Port[Port_8].Async.Port = ASYNC8_GPIO_Port;
	Port[Port_8].Async.Pin = ASYNC8_Pin;
	Port[Port_9].Async.Port = ASYNC9_GPIO_Port;
	Port[Port_9].Async.Pin = ASYNC9_Pin;

	for (uint8 i = Port_1; i <= Port_9; i++) {
		Port[i].Async.Active = false;
		if (i <= Port_6)
			Port[i].lowItestComplete = false;
	}
}

void keypadInit() {
	for (uint8 i = 0; i < 12; i++) {
		KP[i].debounceCount = 5;
	}
	// Column Ports
	KP[1].ColPort = KP[4].ColPort = KP[7].ColPort = KP[star].ColPort = KP_C1_GPIO_Port;
	KP[2].ColPort = KP[5].ColPort = KP[8].ColPort = KP[0].ColPort = KP_C2_GPIO_Port;
	KP[3].ColPort = KP[6].ColPort = KP[9].ColPort = KP[hash].ColPort = KP_C3_GPIO_Port;
	// Column Pins
	KP[1].ColPin = KP[4].ColPin = KP[7].ColPin = KP[star].ColPin = KP_C1_Pin;
	KP[2].ColPin = KP[5].ColPin = KP[8].ColPin = KP[0].ColPin = KP_C2_Pin;
	KP[3].ColPin = KP[6].ColPin = KP[9].ColPin = KP[hash].ColPin = KP_C3_Pin;
	// Row Ports
	KP[1].RowPort = KP[2].RowPort = KP[3].RowPort = KP_R1_GPIO_Port;
	KP[4].RowPort = KP[5].RowPort = KP[6].RowPort = KP_R2_GPIO_Port;
	KP[7].RowPort = KP[8].RowPort = KP[9].RowPort = KP_R3_GPIO_Port;
	KP[star].RowPort = KP[0].RowPort = KP[hash].RowPort = KP_R4_GPIO_Port;
	// Row Pins
	KP[1].RowPin = KP[2].RowPin = KP[3].RowPin = KP_R1_Pin;
	KP[4].RowPin = KP[5].RowPin = KP[6].RowPin = KP_R2_Pin;
	KP[7].RowPin = KP[8].RowPin = KP[9].RowPin = KP_R3_Pin;
	KP[star].RowPin = KP[0].RowPin = KP[hash].RowPin = KP_R4_Pin;
}

void DebounceArrayInit() {
	for (uint8 i = Port_1; i <= Port_9;i++) {
		Port[i].Async.scount = 0;
		Port[i].Async.fcount = 0;
	}
	uint8 TempAsyncBuffer[5][16] = {
			{ 1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4 },
			{ 4, 3, 2, 1, 4, 3, 2, 1, 4, 3, 2, 1, 4, 3, 2, 1 },
			{ 1, 1, 2, 4, 3, 1, 1, 2, 3, 4, 2, 2, 1, 1, 3, 2 },
			{ 1, 1, 2, 2, 3, 3, 4, 4, 2, 3, 3, 2, 1, 4, 4, 1 },
			{ 2, 1, 2, 1, 3, 4, 3, 4, 1, 1, 2, 2, 3, 3, 4, 4 }
	};
	memcpy(&AsyncDebounceBuffer, &TempAsyncBuffer, sizeof(TempAsyncBuffer));
}

