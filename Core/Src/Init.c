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
#include "calibration.h"

void TestRig_Init() {
	printT("==========TestRig========== \n");
	HAL_GPIO_WritePin(TB_Reset_GPIO_Port, TB_Reset_Pin, GPIO_PIN_SET);

	LCD_CursorOn_Off(false);
	LCD_printf("Test Rig      ", 1, 0);
	sprintf(&lcdBuffer, "Firmware: v%.1f", FIRMWARE_VERSION);
	LCD_printf(&lcdBuffer, 2, 0);
	Para[0] = 0;
	Paralen = 0;

	ProcessState = psInitalisation;
	CurrentState = csIDLE;
	ReceiveState = RxWaiting;

	UART2_RecPos = 0;
	samplesUploading = false;

	HAL_GPIO_WritePin(Buffer_OE_GPIO_Port, Buffer_OE_Pin, GPIO_PIN_SET);
	//Set DAC to zero
	reset_ALL_DAC();
	HAL_GPIO_WritePin(MUX_RS_GPIO_Port, MUX_RS_GPIO_Port, GPIO_PIN_SET);
	HAL_GPIO_WritePin(MUX_EN_GPIO_Port, MUX_EN_Pin, GPIO_PIN_SET);
	reset_ALL_MUX();

	ASYNCinit();
	SDIinit();
	for (uint8 i = Port_1; i <= Port_9;i++){
		Port[i].Async.Active = false;
		if (i <= Port_6)
			Port[i].lowItestComplete = false;
	}
	HAL_TIM_Base_Start_IT(&htim6); 	// LED blink, Scan Loom timer
	HAL_TIM_Base_Start_IT(&htim7);	// Scan keypad timer
	HAL_TIM_Base_Start_IT(&htim10);	// Calibration & Latch test timer
	HAL_TIM_Base_Start_IT(&htim11);	// Timeout, Input/Solar Voltage, Sampling timer
	HAL_TIM_Base_Start_IT(&htim14);	// Async Timer

	LoomChecking = true;
	LoomState = 0xFF; // Ensure that checkloom() initialises LCD first iteration

	timeOutCount = 0;
	timeOutEn = false;
}

void initialiseTargetBoard(TboardConfig * Board) {
	LCD_printf("    Initialising    ",2,0);

	uns_ch Command;
	Command = 0xCC;
	Para[0] = 0x49;
	Paralen = 1;
	communication_array(Command, &Para[0], Paralen);
}

void interrogateTargetBoard() {
	LCD_printf("   Interrogating    ",2,0);
	uns_ch Command;
	Command = 0x10;
	communication_arraySerial(Command, 0, 0);
}

void configureTargetBoard(TboardConfig * Board) {
	uns_ch Command;
	Command = 0x56;
	SetPara(Board, Command);
	if (Board->BoardType != b422x)
		communication_array(Command, &Para[0], Paralen);
	OutputsSet = false;
}

void uploadSamplesTargetBoard(TboardConfig * Board) {
	uns_ch Command;
	Command = 0x18;
	SetPara(Board, Command);
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
	CLEAR_REG(BoardConnected.BSR);
	BoardConnected.TPR = 0xFFFF;
	Vfuse.average = 0;
	Vin.average = 0;
}

uint32 ReadSerialNumber(uint8 * Response, uns_ch * data, uint16 length) {
	uns_ch *ptr;
	uint32 SerialNumberRead = 0;
	uint16 Length;
	//Stop re-entry into communication Routines
	UART2_ReceiveComplete = false;
	*Response = *(data + 4);
	switch (*Response) {
		case 0x11:
		case 0x13:
				memcpy(&SerialNumberRead, data+5, 4);
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
	}
	return 255;
}

void SDIinit() {
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
}

