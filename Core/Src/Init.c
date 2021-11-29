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

	/*
	 * Routine initialises all variables and peripherals to the default state for when the system returns to idle state or
	 * begins a new test to assure that the system will behave as expected.
	 */
void TestRig_Init() {
	HAL_GPIO_WritePin(TB_Reset_GPIO_Port, TB_Reset_Pin, GPIO_PIN_SET);				// Pull target board reset high to enable communication if it was previously low
	QuitEnabled = true;																// Allows the system to quit, (removal of quit from serial entry when using keypad)
	LCD_CursorOn_Off(false);														// Remove cursor from screen
	BoardCommsParametersLength = 0;													// Set board communications length to zero

	BoardCommsReceiveState = RxNone;												// State of board communications, no communications waiting
	samplesUploading = false;

	ASYNCinit();																	// Set GPIO pins & ports, set all structs to false
	currentTestInit();																// Set all low current variables to incomplete/test not run yet
	SDIinit();																		// Mount SD card and print the free space
	keypadInit();																	// Set GPIO pins & ports, set all structs to false
	reset_ALL_DAC();																//Set all DAC to zero
	HAL_GPIO_WritePin(MUX_RS_GPIO_Port, MUX_RS_GPIO_Port, GPIO_PIN_SET);			// Enable the multiplexers for input testing on target board
	HAL_GPIO_WritePin(MUX_EN_GPIO_Port, MUX_EN_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(RS485_4011EN_GPIO_Port, RS485_4011EN_Pin, GPIO_PIN_RESET);	// Pull the RS485 enable to low to disable communications on the SDI-12 line
	reset_ALL_MUX();																// Reset all multiplexers, return to async port, (no voltage on the ports)

	HAL_TIM_Base_Start_IT(&htim6); 													// LED blink, Scan Loom timer
	HAL_TIM_Base_Start_IT(&htim7);													// Scan keypad timer
	HAL_TIM_Base_Start_IT(&htim10);													// Calibration & Latch test timer
	HAL_TIM_Base_Start_IT(&htim11);													// Timeout, Input/Solar Voltage, Sampling timer
	HAL_TIM_Base_Start_IT(&htim14);													// Async pulse Timer

	printT((uns_ch*) "==========TestRig========== \n");
}


/*
 * Initialise target board, pass 0/1 to determine whether the board struct is completely erased or if data is kept to continue polling board in idle state
 */
void TargetBoardParamInit(_Bool FullErase) {
	TloomConnected TempBoardType;
	uns_ch TempSubClass;
	uint32 TempSerial;
	uint8 TempVersion;
	if (!FullErase) {
		TempBoardType = BoardConnected.BoardType;
		TempSubClass = BoardConnected.Subclass;
		TempSerial = BoardConnected.SerialNumber;
		TempVersion = BoardConnected.Version;
		memset(&BoardConnected, 0, sizeof(TboardConfig));
		BoardConnected.BoardType = TempBoardType;			//Dont Clear BoardType, can only be accessed from scanLoom()
		BoardConnected.Subclass = TempSubClass;
		BoardConnected.SerialNumber = TempSerial;
		BoardConnected.Version = TempVersion;
	} else
		memset(&BoardConnected, 0, sizeof(TboardConfig));
	memset(&Vfuse, 0, sizeof(TADCconfig));
	memset(&Vin, 0, sizeof(TADCconfig));
}

	// Following 0x10 or 0x12 command read the data string and return the serial number
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

void setTimeOut(uint16 wait) {		// General timeout routine on a 1ms interupt timer
	timeOutEn = true;
	timeOutCount = wait;
}

void setTerminalTimeOut (uint16 wait) {	// timout routine on 1ms interupt timer for the terminal, CDC routine to ensure that system doesnt get stuck transmitting to terminal
	terminalTimeOutCount = wait;
	terminalTimeOutEn = true;
}

void runLatchTimeOut(uint16 wait) {	// Timeout specifically for the latch routine to determine whether the system has become stable
	latchTimeOutCount = wait;
	latchTimeOutEn = true;			// Also initialised on the 1ms interupt timer
	LatchTimeOut = true;
}

// Depending on board connected return the current version of firmware
uint8 getCurrentVersion(TloomConnected Board) {
	switch (Board) {
	case b935x:
		return CURRENT_VERSION_935x;
		break;
	case b937x:
		return CURRENT_VERSION_937x;
		break;
	case b401x:
		return CURRENT_VERSION_401x;
		break;
	case b402x:
		return CURRENT_VERSION_402x;
		break;
	case b422x:
		return CURRENT_VERSION_422x;
		break;
	case b427x:
		return CURRENT_VERSION_427x;
		break;
	case bNone:
		return 255;
		break;
	}
	return 0;
}

void SDIinit() {	// Routine to initialise the SDI-12 functionality
	HAL_GPIO_WritePin(Buffer_OE_GPIO_Port, Buffer_OE_Pin, GPIO_PIN_SET);	// SDI-12 Buffer enable, Tristate the BUS
}

void ASYNCinit() {	// Routine to initialise the Async pulse functionality
	Port[Port_1].Async.Port = ASYNC1_GPIO_Port;	// Set the ports and pins of each asynchronous test port
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
		Port[i].Async.Active = false;	// set all ports to inactive
	}
}

void currentTestInit() {
	for (uint8 i = Port_1; i <= Port_6; i++) {
		Port[i].lowItestComplete = false;	// Set all current test ports to low mA test to incomplete
	}
}

void keypadInit() {	// Routine to set all the ports and pins for the keypad and set the debounce count to 5 for the system to countdown when a button is pressed
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

void DebounceArrayInit() {	// Array for the async pulses to generate random noise when pulsing TODO: Wider pulse widths can be implemented when all boards filtration resistor is changed to 240k
	for (uint8 i = Port_1; i <= Port_9;i++) {
		Port[i].Async.scount = 0;
		Port[i].Async.fcount = 0;
	}
	uint8 TempAsyncBuffer[5][16] = {	// each layer is a different wave of noise, 5 different noise generations
			{ 1, 2, 3, 3, 1, 2, 3, 3, 1, 2, 3, 3, 1, 2, 3, 3 },
			{ 3, 3, 2, 1, 3, 3, 2, 1, 3, 3, 2, 1, 3, 3, 2, 1 },
			{ 1, 1, 2, 3, 3, 1, 1, 2, 3, 4, 2, 2, 1, 1, 3, 2 },
			{ 1, 1, 2, 2, 3, 3, 3, 3, 2, 3, 3, 2, 1, 3, 3, 1 },
			{ 2, 1, 2, 1, 3, 3, 3, 3, 1, 1, 2, 2, 3, 3, 3, 3 }
//			{ 1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4 },
//			{ 4, 3, 2, 1, 4, 3, 2, 1, 4, 3, 2, 1, 4, 3, 2, 1 },
//			{ 1, 1, 2, 4, 3, 1, 1, 2, 3, 4, 2, 2, 1, 1, 3, 2 },
//			{ 1, 1, 2, 2, 3, 3, 4, 4, 2, 3, 3, 2, 1, 4, 4, 1 },
//			{ 2, 1, 2, 1, 3, 4, 3, 4, 1, 1, 2, 2, 3, 3, 4, 4 }
	};
	memcpy(&AsyncDebounceBuffer, &TempAsyncBuffer, sizeof(TempAsyncBuffer));
}

void PrintTestRigFirmwareVersion() {
	LCD_printf((uns_ch*) "Test Rig      ", 1, 0);
	sprintf((char*) &lcdBuffer, "Firmware: v%.2f", FIRMWARE_VERSION);
	LCD_printf((uns_ch*) &lcdBuffer, 2, 0);
}

