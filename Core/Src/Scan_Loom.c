#include <main.h>
#include "interogate_project.h"

void scan_loom(){
		//First Wire
	HAL_GPIO_WritePin(Loom_Sel_GPIO_Port, Loom_Sel_Pin, GPIO_PIN_SET);
	CheckLoom = false;
	PrevLoomState = LoomState;
	LoomState = 0x00;
	for(int i = 0; i < 4;i++){
		ADC_MUXsel(i);
		if(!HAL_GPIO_ReadPin(Loom_Sel_GPIO_Port, Loom_Sel_Pin)){
			LoomState |= (1 << (i));
		} else {
			LoomState |= (0 << (i));
		}
	}
	if(LoomState != PrevLoomState || LoomState == None){
		switch(LoomState){
		case 0:
			sprintf(Buffer, "Connect A Loom...\n");
			CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
			  HAL_UART_Transmit(&huart1, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);
			LoomConnected = None;
			LCD_setCursor(1, 0);
			sprintf(Buffer,"Connect A Loom");
			LCD_printf(&Buffer[0], strlen(Buffer));
			break;
		case 1:
			LoomConnected = b935x;
			LCD_Clear();
			sprintf(Buffer, "9352 Connected...\n");
			CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
			  HAL_UART_Transmit(&huart1, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);
			LCD_setCursor(1, 0);
			sprintf(Buffer,"9352 Loom Connected");
			LCD_printf(&Buffer[0], strlen(Buffer));
			break;
		case 2:
			LoomConnected = b937x;
			LCD_Clear();
			LCD_setCursor(1, 0);
			sprintf(Buffer,"9371 Loom Connected");
			  HAL_UART_Transmit(&huart1, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);
			LCD_printf(&Buffer[0], strlen(Buffer));
			sprintf(Buffer, "9371 Connected...\n");
			CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
			break;
		case 3:
			LoomConnected = b401x;
			LCD_Clear();
			LCD_setCursor(1, 0);
			sprintf(Buffer,"4011 Loom Connected");
			LCD_printf(&Buffer[0], strlen(Buffer));
			sprintf(Buffer, "4011 Connected...\n");
			CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
			  HAL_UART_Transmit(&huart1, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);
			break;
		case 4:
			LoomConnected = b402x;
			LCD_Clear();
			LCD_setCursor(1, 0);
			sprintf(Buffer,"4021 Loom Connected");
			LCD_printf(&Buffer[0], strlen(Buffer));
			sprintf(Buffer, "4021 Connected...\n");
			CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
			  HAL_UART_Transmit(&huart1, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);
			break;
		case 5:
			LoomConnected = b427x;
			LCD_Clear();
			LCD_setCursor(1, 0);
			sprintf(Buffer,"4271 Loom Connected");
			LCD_printf(&Buffer[0], strlen(Buffer));
			sprintf(Buffer, "4271 Connected...\n");
			CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
			  HAL_UART_Transmit(&huart1, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);
			break;
		case 6:
			LoomConnected = b422x;
			LCD_Clear();
			LCD_setCursor(1, 0);
			sprintf(Buffer,"4220 Loom Connected");
			LCD_printf(&Buffer[0], strlen(Buffer));
			sprintf(Buffer, "422x Connected...\n");
			CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
			  HAL_UART_Transmit(&huart1, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);
			break;
		default:
			LoomConnected = None;
			sprintf(Buffer, "Connect A Loom...\n");
			CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
			  HAL_UART_Transmit(&huart1, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);

		}
		  LCD_setCursor(4, 0);
		  LCD_CursorOn_Off(false);
		  sprintf(Buffer,"   1 - Begin Test ");
		  LCD_printf(&Buffer[0], strlen(Buffer));
	}

}

//void BoardInterrogated() {
//	if(Board & 0xFF00 == 0x93)
//	{
//		 if( ((Board & 0xF0) == 0x50 )  && (LoomConnected == b935x) ){
//			 	BoardConnected = TestConfig935x();
//				TportCount = BoardConnected.outputPortCount + BoardConnected.analogInputCount + BoardConnected.digitalInputCout;
//		 } else if( ((Board & 0xF0) == 0x70) && (LoomConnected == b937x) ){
//				BoardConnected = TestConfig937x();
//				TportCount = BoardConnected.outputPortCount + BoardConnected.analogInputCount + BoardConnected.digitalInputCout;
//		 }
//	}
//	else if(Board & 0xFF00 == 0x40)
//	{
//		if( ((Board & 0xF0) == 0x10) && (LoomConnected == b401x) ){
//			BoardConnected = TestConfig401x();
//			TportCount = BoardConnected.outputPortCount + BoardConnected.analogInputCount + BoardConnected.digitalInputCout;
//		} else if ( ((Board & 0xF0) == 0x20) && (LoomConnected == b402x)){
//			BoardConnected = TestConfig402x();
//			TportCount = BoardConnected.outputPortCount + BoardConnected.analogInputCount + BoardConnected.digitalInputCout;
//		}
//	} else if (Board & 0xFF00 == 0x42) {
//		if( ((Board & 0xF0) == 0x20) && (LoomConnected == b422x)){
//			BoardConnected = TestConfig422x();
//			TportCount = BoardConnected.outputPortCount + BoardConnected.analogInputCount + BoardConnected.digitalInputCout;
//		} else if ( ((Board & 0xF0) == 0x70) && (LoomConnected == b427x)) {
//			BoardConnected = TestConfig427x();
//			TportCount = BoardConnected.outputPortCount + BoardConnected.analogInputCount + BoardConnected.digitalInputCout;
//		}
//	} else{
//		sprintf(Buffer, "BoardConfig Error/Loom Connected Error");
//		CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
//		  HAL_UART_Transmit(&huart1, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);
//	}
//}

void currentBoardConnected() {
			if(LoomConnected == b935x) {
				 	TestConfig935x(&BoardConnected);
					return;
			} else if(LoomConnected == b937x) {
					TestConfig937x(&BoardConnected);
					return;
			} else if(LoomConnected == b401x) {
					TestConfig401x(&BoardConnected);
					return;
			} else if (LoomConnected == b402x) {
					TestConfig402x(&BoardConnected);
					return;
			} else if (LoomConnected == b422x) {
					TestConfig422x(&BoardConnected);
					return;
			} else if (LoomConnected == b427x) {
					TestConfig427x(&BoardConnected);
					return;
			} else{
			sprintf(Buffer, "BoardConfig Error/Loom Connected Error");
			CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
			  HAL_UART_Transmit(&huart1, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);
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
