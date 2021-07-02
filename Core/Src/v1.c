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
		HAL_TIM_Base_Start_IT(&htim10);
		InputVoltageStable = false;
		InputVoltageTimer = 0;
		LCD_printf("Board Power Test", 2, 0);

		while(1) {
			if (InputVoltageStable){
				sprintf(&debugTransmitBuffer,"Input Voltage Stable");
				LCD_setCursor(2, 0);
				LCD_printf(&debugTransmitBuffer, strlen(debugTransmitBuffer));
				printT("Input Voltage Stable...\n");
				break;
			} else if (InputVoltageTimer > 5000) {
				HAL_GPIO_WritePin(FAIL_GPIO_Port, FAIL_Pin, GPIO_PIN_SET);
				printT("Input Voltage Failure...\n");
				CurrentState = csIDLE;
				ProcessState = psFailed;
				break;
			}
		}
}
