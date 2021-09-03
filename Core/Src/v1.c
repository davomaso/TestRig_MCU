/*
 * v1.c
 *
 *  Created on: 30 Jun 2021
 *      Author: mason
 */
#include "main.h"
#include "v1.h"
#include "Global_Variables.h"

void clearTestStatusLED() {
	HAL_GPIO_WritePin(FAIL_GPIO_Port, FAIL_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(PASS_GPIO_Port, PASS_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);
}

void testInputVoltage() {
	HAL_GPIO_WritePin(PIN2EN_GPIO_Port, PIN2EN_Pin, GPIO_PIN_SET);
	LCD_printf((uns_ch*)"Board Power Test", 2, 0);
	InputVoltageTimer = 2500;
	InputVoltageSampling = true;
}

void testSolarCharger() {
	HAL_GPIO_WritePin(SOLAR_CH_EN_GPIO_Port, SOLAR_CH_EN_Pin, GPIO_PIN_SET);
	LCD_printf((uns_ch*)"Solar Charger Test", 2, 1);
	SolarChargerTimer = 2500;
	SolarChargerSampling = true;
}

void checkFuseVoltage(TboardConfig *Board) {
	if (fuseBuffer[0] < 10.8)
		CLEAR_BIT(Board->BVR, BOARD_FUSE_STABLE);
	if (fuseBuffer[1] < 2.7)
		CLEAR_BIT(Board->BVR, BOARD_FUSE_STABLE);
	if (fuseBuffer[2] > 0.7)
		CLEAR_BIT(Board->BVR, BOARD_FUSE_STABLE);
}

void CheckPowerRegisters(TboardConfig *Board) {
	if (Board->BoardType == b935x || Board->BoardType == b937x) {
		SET_BIT(Board->BVR, BOARD_FUSE_STABLE);
		checkFuseVoltage(Board);
	} else {
		SET_BIT(Board->BVR, BOARD_FUSE_STABLE);
	}

	if (Board->BVR == 0x03) {
		SET_BIT(Board->BSR, BOARD_POWER_STABLE);
	} else
		CLEAR_BIT(Board->BSR, BOARD_POWER_STABLE);
}

void TestComplete(TboardConfig *Board) {
	HAL_GPIO_WritePin(PIN2EN_GPIO_Port, PIN2EN_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(PIN5EN_GPIO_Port, PIN5EN_Pin, GPIO_PIN_RESET);
	if (READ_BIT(Board->BSR, BOARD_TEST_PASSED)) {
		HAL_GPIO_WritePin(PASS_GPIO_Port, PASS_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(FAIL_GPIO_Port, FAIL_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);
		sprintf((char*)&previousTestBuffer[0], "Previous Test Passed");
	} else {
		HAL_GPIO_WritePin(FAIL_GPIO_Port, FAIL_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(PASS_GPIO_Port, PASS_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);
		sprintf((char*)&previousTestBuffer[0], "Previous Test Failed");
	}
	timeOutEn = false;
	LCD_printf(&previousTestBuffer[0], 2, 0);
}
