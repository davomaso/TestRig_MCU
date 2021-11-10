/*
 * v1.c
 *
 *  Created on: 30 Jun 2021
 *      Author: mason
 */
#include "main.h"
#include "v1.h"
#include "Global_Variables.h"
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
	if (Board->BoardType == b422x) {
		SET_BIT(Board->BVR, V3_SAMPLE_STABLE);
		SET_BIT(Board->BVR, V12_SAMPLE_STABLE);
	} else {
		if ((Board->BoardType != b401x) && (Board->BoardType != b402x) && (Board->VoltageBuffer[V_12] > 11))
			SET_BIT(Board->BVR, V12_SAMPLE_STABLE);
		else if (((Board->BoardType == b401x) || (Board->BoardType == b402x)) && (Board->VoltageBuffer[V_trim] > 10.46))
			SET_BIT(Board->BVR, V12_SAMPLE_STABLE);
		if (((Board->VoltageBuffer[V_3] >= 2.90) && (Board->VoltageBuffer[V_3] < 3.5)) || (Board->BoardType == b427x)
				|| (Board->BoardType == b401x))
			SET_BIT(Board->BVR, V3_SAMPLE_STABLE);
	}
	if (Board->VoltageBuffer[V_12output] > 11.0) {
		SET_BIT(Board->BVR, V12_OUTPUT_STABLE);
	}
	if ( READ_REG(Board->BVR) != 0x3F)
		CLEAR_BIT(Board->BSR, BOARD_POWER_STABLE);
	else
		SET_BIT(Board->BSR, BOARD_POWER_STABLE);
}

void TestComplete(TboardConfig *Board) {
	HAL_GPIO_WritePin(PIN2EN_GPIO_Port, PIN2EN_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(PIN5EN_GPIO_Port, PIN5EN_Pin, GPIO_PIN_RESET);
	if (READ_REG(Board->BSR) == 0x3F) {
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
		sprintf((char*) &debugTransmitBuffer[0], "10.5V User Voltage:    %.3f\n", Board->VoltageBuffer[V_12]);
		printT((uns_ch*) &debugTransmitBuffer[0]);
		sprintf((char*) &debugTransmitBuffer[0], "10.5V Adjusted:        %.3f\n", Board->VoltageBuffer[V_trim]);
		printT((uns_ch*) &debugTransmitBuffer[0]);
	}
	sprintf((char*) &debugTransmitBuffer[0], "12V Output:            %.3f\n", Board->VoltageBuffer[V_12output]);
	printT((uns_ch*) &debugTransmitBuffer[0]);
	sprintf((char*) &debugTransmitBuffer[0], "Battery Level:         %.3f\n", Board->BatteryLevel);
	printT((uns_ch*) &debugTransmitBuffer[0]);
}
