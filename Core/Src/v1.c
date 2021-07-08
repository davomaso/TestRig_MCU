/*
 * v1.c
 *
 *  Created on: 30 Jun 2021
 *      Author: mason
 */
#include "main.h"
#include "interogate_project.h"

void clearTestStatusLED() {
	HAL_GPIO_WritePin(FAIL_GPIO_Port, FAIL_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(PASS_GPIO_Port, PASS_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);
}

void testInputVoltage(){
		LCD_printf("Board Power Test", 2, 0);
		InputVoltageTimer = 2500;
		while(1) {
			if (InputVoltageStable){
				InputVoltageStable = false;
				InputVoltageTimer = InputVoltageCounter = 0;
				printT("Input Voltage Stable...\n");
				break;
			} else if (!InputVoltageTimer) {
				HAL_GPIO_WritePin(FAIL_GPIO_Port, FAIL_Pin, GPIO_PIN_SET);
				printT("Input Voltage Failure...\n");
				CurrentState = csIDLE;
				ProcessState = psFailed;
				break;
			}
		}
}
