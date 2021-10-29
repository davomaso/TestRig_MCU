#include <main.h>
#include "Global_Variables.h"
#include "SerialNumber.h"
#include "LCD.h"
#include "string.h"
#include "UART_Routine.h"

uns_ch ScanKeypad() {
	for (uint8 i = 0; i <= 9; i++) {
		if (KP[i].Pressed) {
			KP[i].Pressed = false;
			 return 0x30 + i;
		}
	} return 0;
}


//uint32 read_serial() {
//	while (!KP[hash].Pressed) {
//		for (uint8 i = 0; i <= 9; i++) {
//			if (KP[i].Pressed && SerialCount < 9) {
//				KP[i].Pressed = 0;
//				tempdata = 0x30 + i;
//				keyPressed = true;
//			}
//		}
//
//		if (keyPressed) {
//			SerialNumber[SerialCount++] = tempdata;
//			if (SerialCount >= 9) {
//				memmove(&SerialNumber[0], &SerialNumber[1], 8);
//				SerialCount--;
//			} else if (SerialCount) {
//				LCD_ClearLine(4);
//				LCD_setCursor(4, 0);
//				sprintf((char*) &debugTransmitBuffer[0], " * - Bspc   # - Ent");
//				LCD_displayString((uns_ch*)&debugTransmitBuffer[0], strlen((char*)debugTransmitBuffer));
//			}
//			LCD_ClearLine(3);
//			LCD_setCursor(3, 0);
//			LCD_displayString(&SerialNumber[0], SerialCount);
//			keyPressed = false;
//		}
//		if (KP[star].Pressed && SerialCount < 9) {
//			KP[star].Pressed = 0;
//
//		}
//
//	}
//	KP[hash].Pressed = 0;
//	if (!Quit_flag) {
//		int i = 0;
//		while (SerialCount != i) {
//			tempSerialNumber = (tempSerialNumber * 10 + (SerialNumber[i++] - 0x30));
//		}
//		LCD_ClearLine(2);
//		LCD_ClearLine(3);
//		LCD_ClearLine(4);
//		LCD_CursorOn_Off(false);
//		USART3->CR1 &= ~(USART_CR1_RXNEIE);
//		sprintf((char*) &debugTransmitBuffer[0], "\n\n\n");
//		printT((uns_ch*) &debugTransmitBuffer[0]);
//		SerialCount = 0;
//	}
//	return tempSerialNumber;
//}
