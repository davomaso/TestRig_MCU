#include <main.h>
#include "interogate_project.h"

void scanLoom(TboardConfig *Board){
		//First Wire
//	HAL_GPIO_WritePin(Loom_Sel_GPIO_Port, Loom_Sel_Pin, GPIO_PIN_SET);
	CheckLoom = false;
	PrevLoomState = LoomState;
	LoomState = 0x00;
	for(int i = 0; i < 4;i++){
		ADC_MUXsel(i);
		delay_us(50); //Wait for system to become stable
		if(!HAL_GPIO_ReadPin(Loom_Sel_GPIO_Port, Loom_Sel_Pin)){
			LoomState |= (1 << (i));
		} else {
			LoomState |= (0 << (i));
		}
	}
	if(LoomState != PrevLoomState || LoomState == bNone){
		switch(LoomState){
		case 0:
			sprintf(debugTransmitBuffer, "Connect A Loom...\n");
			CDC_Transmit_FS(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
			  HAL_UART_Transmit(&huart1, &debugTransmitBuffer[0], strlen(debugTransmitBuffer), HAL_MAX_DELAY);
			Board->BoardType = bNone;
			LCD_setCursor(1, 0);
			sprintf(debugTransmitBuffer,"Connect A Loom");
			LCD_printf(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
			break;
		case 1:
			Board->BoardType = b935x;
			LCD_Clear();
			sprintf(debugTransmitBuffer, "9352 Connected...\n");
			CDC_Transmit_FS(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
			  HAL_UART_Transmit(&huart1, &debugTransmitBuffer[0], strlen(debugTransmitBuffer), HAL_MAX_DELAY);
			LCD_setCursor(1, 0);
			sprintf(debugTransmitBuffer,"9352 Loom Connected");
			LCD_printf(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
			break;
		case 2:
			Board->BoardType = b937x;
			LCD_Clear();
			LCD_setCursor(1, 0);
			sprintf(debugTransmitBuffer,"9371 Loom Connected");
			  HAL_UART_Transmit(&huart1, &debugTransmitBuffer[0], strlen(debugTransmitBuffer), HAL_MAX_DELAY);
			LCD_printf(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
			sprintf(debugTransmitBuffer, "9371 Connected...\n");
			CDC_Transmit_FS(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
			break;
		case 3:
			Board->BoardType = b401x;
			LCD_Clear();
			LCD_setCursor(1, 0);
			sprintf(debugTransmitBuffer,"4011 Loom Connected");
			LCD_printf(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
			sprintf(debugTransmitBuffer, "4011 Connected...\n");
			CDC_Transmit_FS(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
			  HAL_UART_Transmit(&huart1, &debugTransmitBuffer[0], strlen(debugTransmitBuffer), HAL_MAX_DELAY);
			break;
		case 4:
			Board->BoardType = b402x;
			LCD_Clear();
			LCD_setCursor(1, 0);
			sprintf(debugTransmitBuffer,"4021 Loom Connected");
			LCD_printf(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
			sprintf(debugTransmitBuffer, "4021 Connected...\n");
			CDC_Transmit_FS(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
			  HAL_UART_Transmit(&huart1, &debugTransmitBuffer[0], strlen(debugTransmitBuffer), HAL_MAX_DELAY);
			break;
		case 5:
			Board->BoardType = b427x;
			LCD_Clear();
			LCD_setCursor(1, 0);
			sprintf(debugTransmitBuffer,"4271 Loom Connected");
			LCD_printf(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
			sprintf(debugTransmitBuffer, "4271 Connected...\n");
			CDC_Transmit_FS(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
			  HAL_UART_Transmit(&huart1, &debugTransmitBuffer[0], strlen(debugTransmitBuffer), HAL_MAX_DELAY);
			break;
		case 6:
			Board->BoardType = b422x;
			LCD_Clear();
			LCD_setCursor(1, 0);
			sprintf(debugTransmitBuffer,"4220 Loom Connected");
			LCD_printf(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
			sprintf(debugTransmitBuffer, "422x Connected...\n");
			CDC_Transmit_FS(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
			  HAL_UART_Transmit(&huart1, &debugTransmitBuffer[0], strlen(debugTransmitBuffer), HAL_MAX_DELAY);
			break;
		default:
			Board->BoardType = bNone;
			sprintf(debugTransmitBuffer, "Connect A Loom...\n");
			CDC_Transmit_FS(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
			  HAL_UART_Transmit(&huart1, &debugTransmitBuffer[0], strlen(debugTransmitBuffer), HAL_MAX_DELAY);

		}
		  LCD_setCursor(4, 0);
		  LCD_CursorOn_Off(false);
		  sprintf(debugTransmitBuffer,"   1 - Begin Test ");
		  LCD_printf(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
	}

}

void currentBoardConnected(TboardConfig * Board) {
			if(Board->BoardType == b935x) {
				 	TestConfig935x(Board);
					return;
			} else if(Board->BoardType == b937x) {
					TestConfig937x(Board);
					return;
			} else if(Board->BoardType == b401x) {
					TestConfig401x(Board);
					return;
			} else if (Board->BoardType == b402x) {
					TestConfig402x(Board);
					return;
			} else if (Board->BoardType == b422x) {
					TestConfig422x(Board);
					return;
			} else if (Board->BoardType == b427x) {
					TestConfig427x(Board);
					return;
			} else{
				sprintf(debugTransmitBuffer, "BoardConfig Error/Loom Connected Error");
				CDC_Transmit_FS(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
				HAL_UART_Transmit(&D_UART, &debugTransmitBuffer[0], strlen(debugTransmitBuffer), HAL_MAX_DELAY);
			}
			return;
}

void checkLoomConnected() {
	uint8 ADCreading;
	ADC_Ch3sel();
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 100);
	ADCreading = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_Stop(&hadc1);
	if(ADCreading < 10)
		HAL_GPIO_WritePin(PASS_FAIL_GPIO_Port, PASS_FAIL_Pin, GPIO_PIN_RESET);
}
