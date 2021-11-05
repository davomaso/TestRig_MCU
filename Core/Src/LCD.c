#include <main.h>
#include "stdio.h"
#include "string.h"
#include "stdbool.h"
#include "LCD.h"
#include "Delay.h"
#include "UART_Routine.h"

void LCD_init() {
		// For more information regrading commands sent, see EA-DOGM204-A datasheet
	printT((uns_ch*) "\nInitialising LCD Screen...\n\n");
	uint8 Byte = 0x0F;
	HAL_I2C_Mem_Write(&hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY);	// Display on, Cursor on, Blink On
	HAL_Delay(1);

	Byte = 0x3A;
	HAL_I2C_Mem_Write(&hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY);	// 8 Bit data length
	HAL_Delay(1);

	Byte = 0x09;
	HAL_I2C_Mem_Write(&hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY);	// 4 Line display
	HAL_Delay(1);

	Byte = 0x06;
	HAL_I2C_Mem_Write(&hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY);	// Bottom view
	HAL_Delay(1);

	Byte = 0x1E;
	HAL_I2C_Mem_Write(&hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY);	// Bias Setting = 1
	HAL_Delay(1);

	Byte = 0x39;
	HAL_I2C_Mem_Write(&hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY);	// 8 bit data length extension Bit RE=0; IS=1
	HAL_Delay(1);

	Byte = 0x1B;
	HAL_I2C_Mem_Write(&hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY);	// BS0=1 -> Bias=1/6
	HAL_Delay(1);

	Byte = 0x6E;
	HAL_I2C_Mem_Write(&hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY);	// Devider on and set value
	HAL_Delay(1);

	Byte = 0x57;
	HAL_I2C_Mem_Write(&hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY); // Booster on and set contrast (DB1=C5, DB0=C4)
	HAL_Delay(1);

	Byte = 0x72;
	HAL_I2C_Mem_Write(&hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY);	// Set contrast (DB3-DB0=C3-C0)
	HAL_Delay(1);

	Byte = 0x38;
	HAL_I2C_Mem_Write(&hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY); // 8 bit data length extension Bit RE=0; IS=0
	HAL_Delay(1);

	Byte = 0x0F;
	HAL_I2C_Mem_Write(&hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY); // Display on, cursor on, blink on
	HAL_Delay(1);

	Byte = 0x01;
	HAL_I2C_Mem_Write(&hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY); // Clear LCD screen entirely
	HAL_Delay(1);

	//Set DDRAM
	Byte = 0x80;
	HAL_I2C_Mem_Write(&hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY);
	HAL_Delay(1);

		// Print initial test rig screen
	LCD_CursorOn_Off(false);
	LCD_printf((uns_ch*) "Test Rig      ", 1, 0);
	sprintf((char*) &lcdBuffer, "Firmware: v%.2f", FIRMWARE_VERSION);
	LCD_printf((uns_ch*) &lcdBuffer, 2, 0);
}

void LCD_setCursor(uint8 Row, uint8 Col) {
	uint8 Byte;
	Byte = 0x80;
	if (Row > 4)
		Row = 4;
	if (Col > 20)
		Col = 20;
	Byte |= Row ? ((Row - 1) << 5) : Row;
	Byte |= Col ? (Col - 1) : Col;
	HAL_I2C_Mem_Write(&hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY);
	HAL_Delay(1);
}

void LCD_printf(uns_ch *data, uint8 Row, uint8 Col) {
	LCD_ClearLine(Row);											// Clear line which data is to be placed on
	LCD_setCursor(Row, Col);									// Set cursor to the row and col where data should be written
	LCD_displayString((uint8 *) data, strlen((char*) data));	// Print to LCD screen
}

void LCD_displayString(uint8 *data, uint8 len) {
	for (int i = 0; i < len; i++) {
		HAL_I2C_Mem_Write(&hi2c1, (LCD_ADR << 1), 0xC0, 0x01, data++, 1, HAL_MAX_DELAY);
		delay_us(75); //TODO: Set to 50us usually run at 75us to check if the added delay will fix the LCD freezing
	}
}
void LCD_CursorOn_Off(bool Cursor) {
	uint8 Byte;
	if (Cursor) {
		Byte = 0x0F;	// Cursor on

	} else {
		Byte = 0x0C;	// Cursor off
	}
	HAL_I2C_Mem_Write(&hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY);
	delay_us(50);
}

void LCD_Clear() {
	uint8 CLScommand = 0x01;	// Clear LCD screen entirely
	HAL_I2C_Mem_Write(&hi2c1, (LCD_ADR << 1), 0x00, 0x01, &CLScommand, 0x01, HAL_MAX_DELAY);
	delay_us(50);
}

void LCD_ClearLine(uint8 Line) {
	sprintf((char*) &debugTransmitBuffer[0], "                    ");		// Replaces the entire line with spaces/blanks
	LCD_setCursor(Line, 0);
	LCD_displayString((uns_ch*) &debugTransmitBuffer[0], strlen((char*)debugTransmitBuffer));
}

void ChangeCharacterSet(uns_ch Set) {
	// Certain commands are required to be rerun to change the character set
	uns_ch Byte;
	Byte = 0x3A;
	HAL_I2C_Mem_Write(&hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY);		// 8 bit data length extension Bit RE=1; REV=0
	delay_us(50);
	Byte = 0x72;
	HAL_I2C_Mem_Write(&hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY);		// Set contrast (DB3-DB0=C3-C0)
	delay_us(50);
	if (Set == 'A')
		Byte = 0x00;
	else if (Set == 'B')
		Byte = 0x04;
	else if (Set == 'C')
		Byte = 0x0C;
	HAL_I2C_Mem_Write(&hi2c1, (LCD_ADR << 1), 0xC0, 0x01, &Byte, 0x01, HAL_MAX_DELAY);		// Change Character Set
	delay_us(50);
	Byte = 0x38;
	HAL_I2C_Mem_Write(&hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY);		// 8-Bit data length extension Bit RE=0
	delay_us(50);
}
