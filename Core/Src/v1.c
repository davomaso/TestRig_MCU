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

void clearTestStatusLED() {
	HAL_GPIO_WritePin(FAIL_GPIO_Port, FAIL_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(PASS_GPIO_Port, PASS_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);
}

void testInputVoltage() {
	Vin.total = InputVoltageCounter = 0;
	HAL_GPIO_WritePin(PIN2EN_GPIO_Port, PIN2EN_Pin, GPIO_PIN_SET);
	LCD_printf((uns_ch*) "Board Power Test", 2, 0);
	InputVoltageTimer = 2500;
	InputVoltageSampling = true;
}

void testSolarCharger() {
	Vin.total = SolarChargerCounter = 0;
	LCD_printf((uns_ch*) "Solar Charger Test", 2, 1);
	SolarChargerTimer = 1250;
	SolarChargerSampling = true;
	HAL_GPIO_WritePin(SOLAR_CH_EN_GPIO_Port, SOLAR_CH_EN_Pin, GPIO_PIN_SET);
}

void CheckPowerRegisters(TboardConfig *Board) {
	PrintVoltages(Board);
	if ((Board->BoardType != b401x) && (Board->VoltageBuffer[V_12] > 11.6) )
		SET_BIT(Board->BVR, V12_SAMPLE_STABLE);
	else if ( (Board->BoardType == b401x) && (Board->VoltageBuffer[V_12] > 10.46) && (Board->VoltageBuffer[V_105] > 10.48))
		SET_BIT(Board->BVR, V12_SAMPLE_STABLE);
	if (( Board->VoltageBuffer[V_3] > 2.95 ) || (Board->BoardType == b427x))
		SET_BIT(Board->BVR, V3_SAMPLE_STABLE);
	if ( READ_REG(Board->BVR) != 0x0F )
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
	LCD_printf(&previousTestBuffer[0], 2, 0);
}
void PrintHomeScreen(TboardConfig *Board) {
	if (Board->SerialNumber) {
		sprintf(lcdBuffer, "SN:%lu  v%x", Board->SerialNumber, Board->Version);
		LCD_printf((uns_ch*) &lcdBuffer, 2, 5);
		LCD_printf((uns_ch*) "1 - Test Only", 3, 0);
		LCD_printf((uns_ch*) "3 - New SN  Prog - #", 4, 0);
	} else {
		sprintf(lcdBuffer, "SN: N/a");
		LCD_ClearLine(3);
		LCD_printf((uns_ch*) &lcdBuffer, 2, 5);
		LCD_printf((uns_ch*) "Test - #", 4, 12);
	}

}

void PrintVoltages(TboardConfig *Board) {
	printT("\n==========     Target Board Voltages     ==========\n");
	if ( (Board->BoardType == b402x) || (Board->BoardType == b427x) ) {
		sprintf((uns_ch*)&debugTransmitBuffer[0], "Solar Voltage:         %.3f\n", Board->VoltageBuffer[V_SOLAR]);
		printT(&debugTransmitBuffer);
	}
	sprintf((uns_ch*)&debugTransmitBuffer[0], "Input Voltage:         %.3f\n", Board->VoltageBuffer[V_INPUT]);
	printT(&debugTransmitBuffer);
	if (Board->BoardType != b427x && Board->BoardType != b422x) {
		sprintf((uns_ch*)&debugTransmitBuffer[0], "3V Sample Voltage:      %.3f\n", Board->VoltageBuffer[V_3]);
		printT(&debugTransmitBuffer);
	}
	if ( (Board->BoardType != b422x) && (Board->BoardType != b401x) ){
		sprintf((uns_ch*)&debugTransmitBuffer[0], "12V Sample Voltage:    %.3f\n", Board->VoltageBuffer[V_12]);
		printT(&debugTransmitBuffer);
	} else if(Board->BoardType == b401x) {
		sprintf((uns_ch*)&debugTransmitBuffer[0], "10.5V User Voltage:    %.3f\n", Board->VoltageBuffer[V_12]);
		printT(&debugTransmitBuffer);
		sprintf((uns_ch*)&debugTransmitBuffer[0], "10.5V Adjusted:    %.3f\n", Board->VoltageBuffer[V_105]);
		printT(&debugTransmitBuffer);
	}
	sprintf((uns_ch*)&debugTransmitBuffer[0], "12V Output:            %.3f\n", Board->VoltageBuffer[V_12output]);
	printT(&debugTransmitBuffer);
}

}
