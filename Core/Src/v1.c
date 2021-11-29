/*
 * v1.c
 *
 *  Created on: 30 Jun 2021
 *      Author: mason
 */
#include "main.h"
#include "v1.h"
#include "Global_Variables.h"
#include "Communication.h"
#include "LCD.h"
#include "UART_Routine.h"
#include "string.h"

void clearTestStatusLED() {
	HAL_GPIO_WritePin(FAIL_GPIO_Port, FAIL_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(PASS_GPIO_Port, PASS_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);
}

void testInputVoltage() {
	Vin.total = InputVoltageCounter = 0;
	HAL_GPIO_WritePin(PIN2EN_GPIO_Port, PIN2EN_Pin, GPIO_PIN_SET);
	BoardResetTimer = 125;
	InputVoltageStable = false;
	InputVoltageCounter = 0;
	LCD_printf((uns_ch*) "Board Power Test", 2, 0);
}

void testSolarCharger() {
	Vin.total = SolarChargerCounter = 0;
	LCD_printf((uns_ch*) "Solar Charger Test", 2, 1);
	SolarChargerTimer = 1250;
	SolarChargerSampling = true;
	HAL_GPIO_WritePin(SOLAR_CH_EN_GPIO_Port, SOLAR_CH_EN_Pin, GPIO_PIN_SET);
}

void initialiseTargetBoard(TboardConfig *Board) {
	LCD_printf((uns_ch*) "    Initialising    ", 2, 0);								// Initialise target board ready to be placed on self or return to standard 250,1 parameters
	uns_ch Command;
	Command = 0xCC;																	// Initialisation command
	BoardCommsParameters[0] = 0x49;
	BoardCommsParametersLength = 1;
	communication_array(Command, &BoardCommsParameters[0], BoardCommsParametersLength);	// Transmit command
}

void interrogateTargetBoard() {
	if(CurrentState != csIDLE){
		if (CurrentState == csSerialise)											// Print to LCD if in serialising or not in IDLE
			LCD_printf((uns_ch *) "    Serialising    ", 2, 0);
		else
			LCD_printf((uns_ch *) "   Interrogating    ", 2, 0);
	}
	uns_ch Command;
	Command = 0x10;
	communication_arraySerial(Command, 0, 0);										// Transmit the interogation command
}

void configureTargetBoard(TboardConfig *Board) {
	uns_ch Command;
	Command = 0x56;
	SetPara(Board, Command);
	if (Board->BoardType != b422x)													// No need to reconfigure as board is configured with the initialisation
		communication_array(Command, &BoardCommsParameters[0], BoardCommsParametersLength);
	OutputsSet = false;																// Ensure that the outputs are only set the once
}

void sampleTargetBoard(TboardConfig *Board) {
	uns_ch Command;
	LCD_printf((uns_ch*) "      Sampling      ", 2, 0);
	Command = 0x1A;																	// 0x1A command to initialise sampling
	SetPara(Board, Command);
	communication_array(Command, (uns_ch*) &BoardCommsParameters[0], BoardCommsParametersLength);
}

void calibrateTargetBoard(TboardConfig *Board) {
	uns_ch Command = 0xC0;
	LCD_printf((uns_ch*) "     Calibrating      ", 2, 0);
	BoardCommsParameters[0] = 0x50;
	BoardCommsParametersLength = 1;
	communication_array(Command, &BoardCommsParameters[0], BoardCommsParametersLength);
}

void uploadSamplesTargetBoard(TboardConfig *Board) {
	uns_ch Command;
	Command = 0x18;
	LCD_printf((uns_ch*) "     Uploading      ", 2, 0);
	SetPara(Board, Command);
	communication_array(Command, &BoardCommsParameters[0], BoardCommsParametersLength);
}

void GetBatteryLevel(TboardConfig *Board) {
	for (uint8 i = 0; i < Board->GlobalTestNum; i++) {
		Board->BatteryLevel += (float) Board->rawBatteryLevel[i] / 10.0;
	}
	Board->BatteryLevel /= Board->GlobalTestNum;
	if (Board->BatteryLevel > 11.5)
		SET_BIT(Board->BVR, BATT_LVL_STABLE);
	else
		CLEAR_BIT(Board->BVR, BATT_LVL_STABLE);
}

void CheckPowerRegisters(TboardConfig *Board) {
	PrintVoltages(Board);
	if (VoltageComparison(&Board->VoltageBuffer[V_INPUT], INPUT_VOLTAGE_THRESHOLD))// Compare Input Voltage for all boards
		SET_BIT(Board->BVR, INPUT_V_STABLE);

	switch (Board->BoardType) {
	case b422x:
		if (VoltageComparison(&Board->VoltageBuffer[V_12output], OUTPUT_VOLTAGE_VALUE))
			SET_BIT(Board->BVR, V12_OUTPUT_STABLE);
		SET_BIT(Board->BVR, V3_SAMPLE_STABLE);
		SET_BIT(Board->BVR, V12_SAMPLE_STABLE);
		SET_BIT(Board->BVR, V10_SAMPLE_STABLE);
		SET_BIT(Board->BVR, TRIM_VOLTAGE_STABLE);
		SET_BIT(Board->BVR, SOLAR_V_STABLE);
	case b427x:
		if (VoltageComparison(&Board->VoltageBuffer[V_SOLAR], SOLAR_CHARGER_THRESHOLD))
			SET_BIT(Board->BVR, SOLAR_V_STABLE);
		SET_BIT(Board->BVR, V3_SAMPLE_STABLE);
		SET_BIT(Board->BVR, V12_SAMPLE_STABLE);
		if (VoltageComparison(&Board->VoltageBuffer[V_12output], OUTPUT_VOLTAGE_VALUE))
			SET_BIT(Board->BVR, V12_OUTPUT_STABLE);
		SET_BIT(Board->BVR, V10_SAMPLE_STABLE);
		SET_BIT(Board->BVR, TRIM_VOLTAGE_STABLE);
		break;
	case b935x:
	case b937x:
		SET_BIT(Board->BVR, SOLAR_V_STABLE);
		if (VoltageComparison(&Board->VoltageBuffer[V_3], THREE_VOLT_SAMPLE_VALUE))
			SET_BIT(Board->BVR, V3_SAMPLE_STABLE);
		if (VoltageComparison(&Board->VoltageBuffer[V_12], TWELVE_VOLT_SAMPLE_VALUE))
			SET_BIT(Board->BVR, V12_SAMPLE_STABLE);
		if (VoltageComparison(&Board->VoltageBuffer[V_12output], OUTPUT_VOLTAGE_VALUE))
			SET_BIT(Board->BVR, V12_OUTPUT_STABLE);
		SET_BIT(Board->BVR, V10_SAMPLE_STABLE);
		SET_BIT(Board->BVR, TRIM_VOLTAGE_STABLE);
		break;
	case b401x:
		SET_BIT(Board->BVR, SOLAR_V_STABLE);
		if (VoltageComparison(&Board->VoltageBuffer[V_3], THREE_VOLT_SAMPLE_VALUE))
			SET_BIT(Board->BVR, V3_SAMPLE_STABLE);
		if (VoltageComparison(&Board->VoltageBuffer[V_12], TWELVE_VOLT_SAMPLE_VALUE))
			SET_BIT(Board->BVR, V12_SAMPLE_STABLE);
		if (VoltageComparison(&Board->VoltageBuffer[V_12output], OUTPUT_VOLTAGE_VALUE))
			SET_BIT(Board->BVR, V12_OUTPUT_STABLE);
		if (VoltageComparison(&Board->VoltageBuffer[V_105], TEN_VOLT_SAMPLE_THRESHOLD))
			SET_BIT(Board->BVR, V10_SAMPLE_STABLE);
		if (VoltageComparison(&Board->VoltageBuffer[V_trim], TEN_VOLT_SAMPLE_THRESHOLD))
			SET_BIT(Board->BVR, TRIM_VOLTAGE_STABLE);
		break;
	case b402x:

		break;
	case bNone:
		CLEAR_REG(Board->BVR);
		break;

	}
	if ( READ_REG(Board->BVR) != 0xFF)
		CLEAR_BIT(Board->BSR, BOARD_POWER_STABLE);
}

_Bool VoltageComparison(float *Sample, float ExpectedValue) {
	float tolerance;
	tolerance = GET_SAMPLE_VOLTAGE_TOLERANCE(ExpectedValue);
	if ((*Sample < (ExpectedValue + tolerance)) && (*Sample > (ExpectedValue - tolerance)))
		return true;
	else
		return false;
}

void TestComplete(TboardConfig *Board) {
	HAL_GPIO_WritePin(PIN2EN_GPIO_Port, PIN2EN_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(PIN5EN_GPIO_Port, PIN5EN_Pin, GPIO_PIN_RESET);
	if (READ_BIT(Board->BSR,BOARD_TEST_PASSED)) {
		HAL_GPIO_WritePin(PASS_GPIO_Port, PASS_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(FAIL_GPIO_Port, FAIL_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);
		sprintf((char*) &previousTestBuffer[0], "Previous Test Passed");
	} else {
		HAL_GPIO_WritePin(FAIL_GPIO_Port, FAIL_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(PASS_GPIO_Port, PASS_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);
		sprintf((char*) &previousTestBuffer[0], "Previous Test Failed");
	}
	timeOutEn = false;
	LCD_printf(&previousTestBuffer[0], 1, 0);
}
void PrintHomeScreen(TboardConfig *Board) {
	LCD_Clear();
	if (Board->BoardType != bNone) {
		sprintf((char*) &lcdBuffer, "TEST RIG        %x", Board->BoardType);
	} else {
		LCD_printf((uns_ch*) "Connect Loom", 2, 1);
		sprintf((char*) &lcdBuffer, "TEST RIG");
	}
	LCD_printf((uns_ch*) &lcdBuffer, 1, 1);
	if (Board->SerialNumber) {
		sprintf((char*) &lcdBuffer[0], "SN:%lu        ", Board->SerialNumber);	// populate buffer with serial no.
		LCD_printf((uns_ch*) &lcdBuffer, 2, 1);									// display serial no. to screen
		sprintf((char*) &lcdBuffer[0], "v%x", Board->Version);					// populate buffer with version no.
		LCD_setCursor(2, 18);													// Move cursor to edge of screen
		LCD_displayString((uns_ch*) &lcdBuffer[0], strlen((char*) &lcdBuffer));	// display version to screen
		LCD_printf((uns_ch*) "1 - Test Only", 3, 0);							// print options to lcd screen
		LCD_printf((uns_ch*) "3 - New SN  Prog - #", 4, 0);
	} else if (TestRigMode != BatchMode) {
		clearTestStatusLED();
		LCD_ClearLine(3);														// clear line 3 and status leds
	}
}

void PrintVoltages(TboardConfig *Board) {
	printT((uns_ch*) "\n==========     Target Board Voltages     ==========\n");
	if ((Board->BoardType == b402x) || (Board->BoardType == b427x)) {
		sprintf((char*) &debugTransmitBuffer[0], "Solar Voltage:         %.3f\n", Board->VoltageBuffer[V_SOLAR]);
		printT((uns_ch*) &debugTransmitBuffer[0]);
	}
	sprintf((char*) &debugTransmitBuffer[0], "Input Voltage:         %.3f\n", Board->VoltageBuffer[V_INPUT]);
	printT((uns_ch*) &debugTransmitBuffer[0]);
	if (Board->BoardType != b427x && Board->BoardType != b422x) {
		sprintf((char*) &debugTransmitBuffer[0], "3V Sample Voltage:      %.3f\n", Board->VoltageBuffer[V_3]);
		printT((uns_ch*) &debugTransmitBuffer[0]);
	}
	if ((Board->BoardType != b422x) && (Board->BoardType != b401x) && (Board->BoardType != b402x)) {
		sprintf((char*) &debugTransmitBuffer[0], "12V Sample Voltage:    %.3f\n", Board->VoltageBuffer[V_12]);
		printT((uns_ch*) &debugTransmitBuffer[0]);
	} else if (Board->BoardType == b401x || Board->BoardType == b402x) {
		sprintf((char*) &debugTransmitBuffer[0], "10.5V User Voltage:    %.3f\n", Board->VoltageBuffer[V_105]);
		printT((uns_ch*) &debugTransmitBuffer[0]);
		sprintf((char*) &debugTransmitBuffer[0], "10.5V Adjusted:        %.3f\n", Board->VoltageBuffer[V_trim]);
		printT((uns_ch*) &debugTransmitBuffer[0]);
	}
	sprintf((char*) &debugTransmitBuffer[0], "12V Output:            %.3f\n", Board->VoltageBuffer[V_12output]);
	printT((uns_ch*) &debugTransmitBuffer[0]);
	sprintf((char*) &debugTransmitBuffer[0], "Battery Level:         %.3f\n", Board->BatteryLevel);
	printT((uns_ch*) &debugTransmitBuffer[0]);
}
