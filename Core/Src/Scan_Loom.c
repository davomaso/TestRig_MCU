#include <main.h>
#include "Global_Variables.h"
#include "LCD.h"
#include "Delay.h"
#include "UART_Routine.h"

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
	if( LoomState != PrevLoomState) {
			LCD_Clear();
					switch(LoomState){
					case 0:
						Board->BoardType = bNone;
						TargetBoardParamInit(true);
						LoomState = 0xFF;
						LCD_printf((uns_ch*)"Connect Loom",2,4);
						printT((uns_ch*)"Connect Loom...\n");
						break;
					case 1:
						Board->BoardType = b935x;
						Board->Subclass = 'C';
						printT((uns_ch*)"9352 Connected...\n");
						break;
					case 2:
						Board->BoardType = b937x;
						Board->Subclass = 'D';
						printT((uns_ch*)"9371 Connected...\n");
						break;
					case 3:
						Board->BoardType = b401x;
						Board->Subclass = 0;
						printT((uns_ch*)"401x Connected...\n");
						break;
					case 4:
						Board->BoardType = b402x;
						Board->Subclass = 0;
						printT((uns_ch*)"4022 Connected...\n");
						break;
					case 5:
						Board->BoardType = b427x;
						Board->Subclass = 0;
						printT((uns_ch*)"4271 Connected...\n");
						break;
					case 0x0A: //change to 6 when loom is fixed
						Board->BoardType = b422x;
						Board->Subclass = 0;
						printT((uns_ch*)"4220 Connected...\n");
						break;
					default:
						Board->BoardType = bNone;
						Board->Subclass = 0;
						LCD_printf((uns_ch*)"Unknown Loom",1,0);
						printT((uns_ch*)"Unknown Loom...\n");
					}
					if (Board->BoardType != bNone) {
						sprintf(lcdBuffer, "TEST RIG  %x",Board->BoardType);
						HAL_GPIO_WritePin(PIN2EN_GPIO_Port, PIN2EN_Pin, GPIO_PIN_SET);
						HAL_Delay(1200);
					} else {
						sprintf(lcdBuffer, "TEST RIG");
						HAL_GPIO_WritePin(PIN2EN_GPIO_Port, PIN2EN_Pin, GPIO_PIN_RESET);
					}
					LCD_printf((uns_ch*)&lcdBuffer,1,7);

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
				printT((uns_ch*)"\n BoardConfig Error/Loom Connected Error \n");
			}
			return;
}

