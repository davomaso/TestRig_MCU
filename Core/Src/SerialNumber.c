#include <main.h>
#include "Global_Variables.h"
#include "SerialNumber.h"
#include "LCD.h"
#include "string.h"
#include "UART_Routine.h"

uint32 read_serial() {
	USART3->CR1 |= (USART_CR1_RXNEIE);
	LCD_printf((uns_ch*) "Enter Serial Number", 2, 0);
	LCD_printf((uns_ch*) " * - Esc    # - Ent ", 4, 0);
	memset(&SerialNumber,0,9);
	LCD_ClearLine(3);
	LCD_setCursor(3, 0);
	LCD_CursorOn_Off(true);
	SerialCount = 0;
	uint8 tempdata = 0;
	uint32 tempSerialNumber = 0;
	_Bool keyPressed = false;
	_Bool Quit_flag;
	while (!KP[hash].Pressed) {
		for (uint8 i = 0; i <= 9; i++) {
			if (KP[i].Pressed && SerialCount < 9) {
				KP[i].Pressed = 0;
				tempdata = 0x30 + i;
				keyPressed = true;
			}
		}

		if (keyPressed) {
			SerialNumber[SerialCount++] = tempdata;
			if (SerialCount >= 9) {
				memmove(&SerialNumber[0], &SerialNumber[1], 8);
				SerialCount--;
			} else if (SerialCount) {
				LCD_ClearLine(4);
				LCD_setCursor(4, 0);
				printT((uns_ch*)"\n");
				sprintf((char*) &debugTransmitBuffer[0], " * - Bspc   # - Ent");
				LCD_displayString((uns_ch*)&debugTransmitBuffer[0], strlen((char*)debugTransmitBuffer));
			}
			LCD_ClearLine(3);
			LCD_setCursor(3, 0);
			LCD_displayString(&SerialNumber[0], SerialCount);
			keyPressed = false;
		}
		if (KP[star].Pressed && SerialCount < 9) {
			KP[star].Pressed = 0;
			if (SerialCount) {
				SerialNumber[SerialCount--] = 0x08;
				LCD_setCursor(3, SerialCount + 1);
				sprintf((char*) &debugTransmitBuffer[0], "  ");
				LCD_displayString((uns_ch*)&debugTransmitBuffer[0], strlen((char*)debugTransmitBuffer));
				LCD_setCursor(3, SerialCount + 1);
				sprintf((char*) &debugTransmitBuffer[0], "\x8 \x8");
				printT(&debugTransmitBuffer[0]);
				if (SerialCount == 0) {
					LCD_ClearLine(4);
					LCD_setCursor(4, 0);
					sprintf((char*) &debugTransmitBuffer[0], " * - Esc    # - Ent");
					LCD_displayString((uns_ch*) &debugTransmitBuffer[0], strlen((char*)debugTransmitBuffer));
					LCD_setCursor(3, SerialCount + 1);
				}
			} else {
				Quit_flag = true;
				break;
			}
		}
		if (BarcodeScanned == true) {
			BarcodeScanned = false;
			LCD_ClearLine(3);
			LCD_setCursor(3, 0);
			LCD_displayString(&BarcodeBuffer[0], BarcodeCount);
			for (int i = 0; i < BarcodeCount; i++) {
				SerialNumber[i] = BarcodeBuffer[i];
			}
			printT((uns_ch*)&SerialNumber);
			SerialCount = BarcodeCount;
			BarcodeCount = 0;
			USART3->CR1 |= (USART_CR1_RXNEIE);
			break;
		}
	}
	KP[hash].Pressed = 0;
	if (!Quit_flag) {
		int i = 0;
		while (SerialCount != i) {
			tempSerialNumber = (tempSerialNumber * 10 + (SerialNumber[i++] - 0x30));
		}
		LCD_ClearLine(2);
		LCD_ClearLine(3);
		LCD_ClearLine(4);
		LCD_CursorOn_Off(false);
		USART3->CR1 &= ~(USART_CR1_RXNEIE);
		sprintf((char*) &debugTransmitBuffer[0], "\n\n\n");
		printT((uns_ch*) &debugTransmitBuffer[0]);
		SerialCount = 0;
	}
	return tempSerialNumber;
}

bool ContinueWithCurrentSerial() {
	LCD_ClearLine(2);
	sprintf((char*) &lcdBuffer[0], "    S/N Detected    ");
	printT((uns_ch*) &lcdBuffer[0]);
	LCD_setCursor(2, 0);
	LCD_displayString((uns_ch*) &lcdBuffer, strlen((char*) lcdBuffer));

	LCD_ClearLine(3);
	sprintf((char*) &lcdBuffer[0], "      %lu", BoardConnected.SerialNumber);
	printT((uns_ch*) &lcdBuffer[0]);
	LCD_setCursor(3, 0);
	LCD_displayString((uns_ch*) &lcdBuffer[0], strlen((char*) lcdBuffer));

	LCD_ClearLine(4);
	sprintf((char*) &lcdBuffer[0], "*-Renew       #-Keep");
	printT((uns_ch*)&lcdBuffer);
	LCD_setCursor(4, 0);
	LCD_displayString((uns_ch*) &lcdBuffer[0], strlen((char*) lcdBuffer));
	while (1) {
		if (KP[hash].Pressed) {
			KP[hash].Pressed = 0;
			SET_BIT(BoardConnected.BSR, BOARD_SERIALISED);
			LCD_ClearLine(4);
			return true;
		}
		if (KP[star].Pressed) {
			KP[star].Pressed = 0;
			CLEAR_BIT(BoardConnected.BSR, BOARD_SERIALISED);
			LCD_ClearLine(4);
			return false;
		}
	}
	return false;
}
