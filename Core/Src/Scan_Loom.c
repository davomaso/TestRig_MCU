#include <main.h>
#include "interogate_project.h"

void scan_loom(){
		//First Wire
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
			LCD_setCursor(1, 0);
			sprintf(Buffer,"9352 Loom Connected");
			LCD_printf(&Buffer[0], strlen(Buffer));
			break;
		case 2:
			LoomConnected = b937x;
			LCD_Clear();
			LCD_setCursor(1, 0);
			sprintf(Buffer,"9371 Loom Connected");
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
			break;
		case 4:
			LoomConnected = b402x;
			LCD_Clear();
			LCD_setCursor(1, 0);
			sprintf(Buffer,"4021 Loom Connected");
			LCD_printf(&Buffer[0], strlen(Buffer));
			sprintf(Buffer, "4021 Connected...\n");
			CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
			break;
		case 5:
			LoomConnected = b427x;
			LCD_Clear();
			LCD_setCursor(1, 0);
			sprintf(Buffer,"4271 Loom Connected");
			LCD_printf(&Buffer[0], strlen(Buffer));
			sprintf(Buffer, "4271 Connected...\n");
			CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
			break;
		case 6:
			LoomConnected = b422x;
			LCD_Clear();
			LCD_setCursor(1, 0);
			sprintf(Buffer,"4220 Loom Connected");
			LCD_printf(&Buffer[0], strlen(Buffer));
			sprintf(Buffer, "422x Connected...\n");
			CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
			break;
		default:
			LoomConnected = None;
			sprintf(Buffer, "Connect A Loom...\n");
			CDC_Transmit_FS(&Buffer[0], strlen(Buffer));

		}
		  LCD_setCursor(4, 0);
		  LCD_CursorOn_Off(false);
		  sprintf(Buffer,"   1 - Begin Test ");
		  LCD_printf(&Buffer[0], strlen(Buffer));
	}

}
