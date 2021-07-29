#include <main.h>
#include "interogate_project.h"
uint32 read_serial() {
	USART3->CR1 |= (USART_CR1_RXNEIE);
	LCD_printf("Enter Serial Number ", 2, 0);
	LCD_printf(" * - Esc    # - Ent ",4,0);
	LCD_ClearLine(3);
	LCD_setCursor(3, 0);
	LCD_CursorOn_Off(true);
	SerialCount = 0;
	uint8 tempdata;
	uint32 tempSerialNumber = 0;
	_Bool keyPressed;
	_Bool Quit_flag;
	while(!KP_hash.Pressed){
		if(KP_1.Pressed && SerialCount < 9) {
			KP_1.Count = KP_1.Pressed = 0;
			tempdata = 0x31;
			keyPressed = true;
		}
		if(KP_2.Pressed && SerialCount < 9){
			KP_2.Count = KP_2.Pressed = 0;
			tempdata = 0x32;
			keyPressed = true;
		}
		if(KP_3.Pressed && SerialCount < 9){
			KP_3.Count = KP_3.Pressed = 0;
			tempdata = 0x33;
			keyPressed = true;
		}
		if(KP_4.Pressed && SerialCount < 9){
			KP_4.Count = KP_4.Pressed = 0;
			tempdata = 0x34;
			keyPressed = true;
		}
		if(KP_5.Pressed && SerialCount < 9){
			KP_5.Count = KP_5.Pressed = 0;
			tempdata = 0x35;
			keyPressed = true;
		}
		if(KP_6.Pressed && SerialCount < 9){
			KP_6.Count = KP_6.Pressed = 0;
			tempdata = 0x36;
			keyPressed = true;
		}
		if(KP_7.Pressed && SerialCount < 9){
			KP_7.Count = KP_7.Pressed = 0;
			tempdata = 0x37;
			keyPressed = true;
		}
		if(KP_8.Pressed && SerialCount < 9){
			KP_8.Count = KP_8.Pressed = 0;
			tempdata = 0x38;
			keyPressed = true;
		}
		if(KP_9.Pressed && SerialCount < 9){
			KP_9.Count = KP_9.Pressed = 0;
			tempdata = 0x39;
			keyPressed = true;
		}
		if(KP_0.Pressed && SerialCount < 9){
			KP_0.Count = KP_0.Pressed = 0;
			tempdata = 0x30;
			keyPressed = true;
		}
		if (keyPressed) {
			SerialNumber[SerialCount++] = tempdata;
			if (SerialCount >= 9) {
				memmove(&SerialNumber[0], &SerialNumber[1], 8);
				SerialCount--;
			} else if (SerialCount) {
				LCD_ClearLine(4);
				LCD_setCursor(4,0);
				printT("\n");
				sprintf(debugTransmitBuffer, " * - Bspc   # - Ent");
				LCD_displayString(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
			}
			LCD_ClearLine(3);
			LCD_setCursor(3, 0);
			LCD_displayString(&SerialNumber[0], SerialCount);
			keyPressed = false;
		}
		if(KP_star.Pressed && SerialCount < 9){
			KP_star.Count = KP_star.Pressed = 0;
			if (SerialCount) {
				SerialNumber[SerialCount--] = 0x08;
				LCD_setCursor(3, SerialCount+1);
				sprintf(debugTransmitBuffer, "  ");
				LCD_displayString(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
				LCD_setCursor(3, SerialCount+1);
				sprintf(debugTransmitBuffer, "\x8 \x8");
				printT(&debugTransmitBuffer[0]);
				if (SerialCount == 0) {
					LCD_ClearLine(4);
					LCD_setCursor(4,0);
					sprintf(debugTransmitBuffer, " * - Esc    # - Ent");
					LCD_displayString(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
					LCD_setCursor(3,SerialCount+1);
				}
			} else {
				Quit_flag = true;
				break;
			}
		}
		if (BarcodeScanned == true) {
			BarcodeScanned = false;
			LCD_ClearLine(3);
			LCD_setCursor(3,0);
			LCD_displayString(&BarcodeBuffer[0], BarcodeCount);
			for (int i = 0; i < BarcodeCount; i++) {
				SerialNumber[i] = BarcodeBuffer[i];
			}
			printT(&SerialNumber);
			SerialCount = BarcodeCount;
			BarcodeCount = 0;
			USART3->CR1 |= (USART_CR1_RXNEIE);
		}
	}
	KP_hash.Count = KP_hash.Pressed = 0;
	if (!Quit_flag) {

		int i = 0;
		while (SerialCount != i) {
			tempSerialNumber = (tempSerialNumber * 10 + (SerialNumber[i++] - 0x30) );
		}
		LCD_ClearLine(2);
		LCD_ClearLine(3);
		LCD_ClearLine(4);
		LCD_CursorOn_Off(false);
		USART3->CR1 &= ~(USART_CR1_RXNEIE);
		sprintf(debugTransmitBuffer, "\n\n\n", 1);
		printT(&debugTransmitBuffer[0]);
		SerialCount = 0;
	}
	return tempSerialNumber;
}

bool ContinueWithCurrentSerial() {
	LCD_ClearLine(2);
	sprintf(lcdBuffer, "    S/N Detected    ");
	printT(&lcdBuffer);
	LCD_setCursor(2, 0);
	LCD_displayString(&lcdBuffer, strlen(lcdBuffer));

	LCD_ClearLine(3);
	sprintf(lcdBuffer, "      %d",BoardConnected.SerialNumber);
	printT(&lcdBuffer);
	LCD_setCursor(3, 0);
	LCD_displayString(&lcdBuffer, strlen(lcdBuffer));


	LCD_ClearLine(4);
	sprintf(lcdBuffer, "*-Renew       #-Keep");
	printT(&lcdBuffer);
	LCD_setCursor(4, 0);
	LCD_displayString(&lcdBuffer, strlen(lcdBuffer));
	while(1) {
		if(KP_hash.Pressed){
			KP_hash.Count = KP_hash.Pressed = 0;
			SET_BIT( BoardConnected.BSR, BOARD_SERIALISED );
			LCD_ClearLine(4);
			return true;
		}
		if(KP_star.Pressed){
			KP_star.Count = KP_star.Pressed = 0;
			CLEAR_BIT( BoardConnected.BSR, BOARD_SERIALISED );
			LCD_ClearLine(4);
			return false;
		}
	}
	return false;
}
