#include <main.h>
#include "interogate_project.h"
void read_keypad() {
	LCD_ClearLine(4);
	LCD_setCursor(4,0);
	sprintf(Buffer, " * - Esc    # - Ent ");
	LCD_printf(&Buffer[0], strlen(Buffer));
	LCD_setCursor(3, 0);
	LCD_CursorOn_Off(true);
	SerialCount = 0;
	while(!KP_hash.Pressed){
		if(KP_1.Pressed && SerialCount < 9){
			KP_1.Count = KP_1.Pressed = 0;
			SerialNumber[SerialCount++] = 1;
			sprintf(Buffer, "%d", 1);
			CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
			LCD_printf(&Buffer[0], strlen(Buffer));
			if (SerialCount == 1) {
				LCD_ClearLine(4);
				LCD_setCursor(4,0);
				sprintf(Buffer, " * - Bkspc   # - Ent");
				LCD_printf(&Buffer[0], strlen(Buffer));
				LCD_setCursor(3,SerialCount+1);
			}
		}
		if(KP_2.Pressed && SerialCount < 9){
			KP_2.Count = KP_2.Pressed = 0;
			SerialNumber[SerialCount++] = 2;
			sprintf(Buffer, "%d", 2);
			CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
			LCD_printf(&Buffer[0], strlen(Buffer));
			if (SerialCount == 1) {
				LCD_ClearLine(4);
				LCD_setCursor(4,0);
				sprintf(Buffer, " * - Bkspc   # - Ent");
				LCD_printf(&Buffer[0], strlen(Buffer));
				LCD_setCursor(3,SerialCount+1);
			}
		}
		if(KP_3.Pressed && SerialCount < 9){
			KP_3.Count = KP_3.Pressed = 0;
			SerialNumber[SerialCount++] = 3;
			sprintf(Buffer, "%d", 3);
			CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
			LCD_printf(&Buffer[0], strlen(Buffer));
			if (SerialCount == 1) {
				LCD_ClearLine(4);
				LCD_setCursor(4,0);
				sprintf(Buffer, " * - Bkspc   # - Ent");
				LCD_printf(&Buffer[0], strlen(Buffer));
				LCD_setCursor(3,SerialCount+1);
			}
		}
		if(KP_4.Pressed && SerialCount < 9){
			KP_4.Count = KP_4.Pressed = 0;
			SerialNumber[SerialCount++] = 4;
			sprintf(Buffer, "%d", 4);
			CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
			LCD_printf(&Buffer[0], strlen(Buffer));
			if (SerialCount == 1) {
				LCD_ClearLine(4);
				LCD_setCursor(4,0);
				sprintf(Buffer, " * - Bkspc   # - Ent");
				LCD_printf(&Buffer[0], strlen(Buffer));
				LCD_setCursor(3,SerialCount+1);
			}
		}
		if(KP_5.Pressed && SerialCount < 9){
			KP_5.Count = KP_5.Pressed = 0;
			SerialNumber[SerialCount++] = 5;
			sprintf(Buffer, "%d", 5);
			CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
			LCD_printf(&Buffer[0], strlen(Buffer));
			if (SerialCount == 1) {
				LCD_ClearLine(4);
				LCD_setCursor(4,0);
				sprintf(Buffer, " * - Bkspc   # - Ent");
				LCD_printf(&Buffer[0], strlen(Buffer));
				LCD_setCursor(3,SerialCount+1);
			}
		}
		if(KP_6.Pressed && SerialCount < 9){
			KP_6.Count = KP_6.Pressed = 0;
			SerialNumber[SerialCount++] = 6;
			sprintf(Buffer, "%d", 6);
			CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
			LCD_printf(&Buffer[0], strlen(Buffer));
			if (SerialCount == 1) {
				LCD_ClearLine(4);
				LCD_setCursor(4,0);
				sprintf(Buffer, " * - Bkspc   # - Ent");
				LCD_printf(&Buffer[0], strlen(Buffer));
				LCD_setCursor(3,SerialCount+1);
			}
		}
		if(KP_7.Pressed && SerialCount < 9){
			KP_7.Count = KP_7.Pressed = 0;
			SerialNumber[SerialCount++] = 7;
			sprintf(Buffer, "%d", 7);
			CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
			LCD_printf(&Buffer[0], strlen(Buffer));
			if (SerialCount == 1) {
				LCD_ClearLine(4);
				LCD_setCursor(4,0);
				sprintf(Buffer, " * - Bkspc   # - Ent");
				LCD_printf(&Buffer[0], strlen(Buffer));
				LCD_setCursor(3,SerialCount+1);
			}
		}
		if(KP_8.Pressed && SerialCount < 9){
			KP_8.Count = KP_8.Pressed = 0;
			SerialNumber[SerialCount++] = 8;
			sprintf(Buffer, "%d", 8);
			CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
			LCD_printf(&Buffer[0], strlen(Buffer));
			if (SerialCount == 1) {
				LCD_ClearLine(4);
				LCD_setCursor(4,0);
				sprintf(Buffer, " * - Bkspc   # - Ent");
				LCD_printf(&Buffer[0], strlen(Buffer));
				LCD_setCursor(3,SerialCount+1);
			}
		}
		if(KP_9.Pressed && SerialCount < 9){
			KP_9.Count = KP_9.Pressed = 0;
			SerialNumber[SerialCount++] = 9;
			sprintf(Buffer, "%d", 9);
			CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
			LCD_printf(&Buffer[0], strlen(Buffer));
			if (SerialCount == 1) {
				LCD_ClearLine(4);
				LCD_setCursor(4,0);
				sprintf(Buffer, " * - Bkspc   # - Ent");
				LCD_printf(&Buffer[0], strlen(Buffer));
				LCD_setCursor(3,SerialCount+1);
			}
		}
		if(KP_0.Pressed && SerialCount < 9){
			KP_0.Count = KP_0.Pressed = 0;
			SerialNumber[SerialCount++] = 0;
			sprintf(Buffer, "%d", 0);
			CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
			LCD_printf(&Buffer[0], strlen(Buffer));
			if (SerialCount == 1) {
				LCD_ClearLine(4);
				LCD_setCursor(4,0);
				sprintf(Buffer, " * - Bkspc   # - Ent");
				LCD_printf(&Buffer[0], strlen(Buffer));
				LCD_setCursor(3,SerialCount+1);
			}
		}
		if(KP_star.Pressed && SerialCount < 9){
			KP_star.Count = KP_star.Pressed = 0;
			if (SerialCount) {
				SerialNumber[SerialCount--] = 0x08;
				LCD_setCursor(3, SerialCount+1);
				sprintf(Buffer, "  ");
				LCD_printf(&Buffer[0], strlen(Buffer));
				LCD_setCursor(3, SerialCount+1);
				sprintf(Buffer, "\x8 \x8");
				CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
				if (SerialCount == 0) {
					LCD_ClearLine(4);
					LCD_setCursor(4,0);
					sprintf(Buffer, " * - Esc    # - Ent");
					LCD_printf(&Buffer[0], strlen(Buffer));
					LCD_setCursor(3,SerialCount+1);
				}
			} else {
				Quit_flag = true;
				break;
			}
		}
		if (SerialCount == 9) {
			break;
		}
	}
	KP_hash.Count = KP_hash.Pressed = 0;
	if (!Quit_flag) {
		BoardConnected.SerialNumber = 0;
		int i = 0;
		while (SerialCount != i) {
			BoardConnected.SerialNumber = (BoardConnected.SerialNumber * 10 + SerialNumber[i++]);
		}
		LCD_ClearLine(2);
		LCD_ClearLine(3);
		LCD_ClearLine(4);
		LCD_CursorOn_Off(false);

		sprintf(Buffer, "\n\n\n", 1);
		CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
		SerialCount = 0;
	}
}
