#include <main.h>
#include "interogate_project.h"

void scanLoom(TboardConfig *Board) {
	uint8 PrevLoomState;
	CheckLoom = false;
	PrevLoomState = LoomState;
	LoomState = 0x00;
	for(int i = 0; i < 4;i++){
		ADC_MUXsel(i);
		delay_us(50); //Wait for system to become stable
		if(!HAL_GPIO_ReadPin(Loom_Sel_GPIO_Port, Loom_Sel_Pin)){
			LoomState |= (1 << (i));
		}
	}	// ###### add loom checking to occur during testing
	if(LoomState != PrevLoomState) {
		LCD_Clear();
		switch(LoomState){
		case 0:
			LCD_printf("Connect Loom",2,4);
			printT("Connect Loom...\n");
			break;
		case 1:
			Board->BoardType = b935x;
			LCD_printf("9352 Loom Connected",1,0);
			printT("9352 Connected...\n");
			break;
		case 2:
			Board->BoardType = b937x;
			LCD_printf("9371 Loom Connected",1,0);
			printT("9371 Connected...\n");
			break;
		case 3:
			Board->BoardType = b401x;
			LCD_printf("4012 Loom Connected",1,0);
			printT("401x Connected...\n");
			break;
		case 4:
			Board->BoardType = b402x;
			LCD_printf("4022 Loom Connected",1,0);
			printT("4022 Connected...\n");
			break;
		case 5:
			Board->BoardType = b427x;
			LCD_printf("4271 Loom Connected",1,0);
			printT("4271 Connected...\n");
			break;
		case 0x0A: //change to 6 when loom is fixed
			Board->BoardType = b422x;
			LCD_printf("4220 Loom Connected",1,0);
			printT("4220 Connected...\n");
			break;
		default:
			Board->BoardType = bNone;
			LCD_printf("Unknown Loom",1,0);
			printT("Unknown Loom...\n");
		}
		LCD_printf("   1 - Begin Test ",4,0);	// change wording to be dependant on state and loom connected
		printT("Press 1 - Begin Test \n");
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
					SET_BIT(Board->BSR, BOARD_CALIBRATED);
					return;
			} else if (Board->BoardType == b427x) {
					TestConfig427x(Board);
					return;
			} else{
				printT("\n BoardConfig Error/Loom Connected Error \n");
			}
			return;
}

//void checkLoomConnected() {
//	uint8 ADCreading;
//	ADC_Ch3sel();
//	HAL_ADC_Start(&hadc1);
//	HAL_ADC_PollForConversion(&hadc1, 100);
//	ADCreading = HAL_ADC_GetValue(&hadc1);
//	HAL_ADC_Stop(&hadc1);
////	if(ADCreading < 10)
////		HAL_GPIO_WritePin(FAIL_GPIO_Port, FAIL_Pin, GPIO_PIN_RESET);
////		HAL_GPIO_WritePin(PASS_GPIO_Port, PASS_Pin, GPIO_PIN_RESET);
////		HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);
//}
