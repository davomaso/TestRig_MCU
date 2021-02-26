#include <main.h>
#include "LCD.h"

void LCD_init() {
	HAL_Delay(50);
	HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_RESET);
	HAL_Delay(2);
	HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_SET);
	sprintf(Buffer, "\nInitialising LCD Screen...\n\n");
	HAL_UART_Transmit(&T_UART, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);
	uint8 Byte = 0x0F;
	HAL_I2C_Mem_Write( &hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY);
	HAL_Delay(1);

	Byte = 0x3A;
	HAL_I2C_Mem_Write( &hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY);
	HAL_Delay(1);

	Byte = 0x09;
	HAL_I2C_Mem_Write( &hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY);
	HAL_Delay(1);

	Byte = 0x06;
	HAL_I2C_Mem_Write( &hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY);
	HAL_Delay(1);

	Byte = 0x1E;
	HAL_I2C_Mem_Write( &hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY);
	HAL_Delay(1);

	Byte = 0x39;
	HAL_I2C_Mem_Write( &hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY);
	HAL_Delay(1);

	Byte = 0x1B;
	HAL_I2C_Mem_Write( &hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY);
	HAL_Delay(1);

	Byte = 0x6E;
	HAL_I2C_Mem_Write( &hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY);
	HAL_Delay(1);

	Byte = 0x57;
	HAL_I2C_Mem_Write( &hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY);
	HAL_Delay(1);

	Byte = 0x72;
	HAL_I2C_Mem_Write( &hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY);
	HAL_Delay(1);

	Byte = 0x38;
	HAL_I2C_Mem_Write( &hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY);
	HAL_Delay(1);

	Byte = 0x0F;
	HAL_I2C_Mem_Write( &hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY);
	HAL_Delay(1);

	Byte = 0x01;
	HAL_I2C_Mem_Write( &hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY);
	HAL_Delay(1);

		//Set DDRAM
	Byte = 0x80;
	HAL_I2C_Mem_Write( &hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY);
	HAL_Delay(1);
}

void LCD_setCursor(uint8 Row, uint8 Col) {
	Byte = 0x80;
	if (Row > 4) Row = 4;
	if (Col > 20) Col = 20;
	Byte |= Row ? ((Row - 1) << 5) : Row;
	Byte |= Col ? (Col- 1): Col;
	HAL_I2C_Mem_Write( &hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY);
	HAL_Delay(1);
}

void LCD_printf(uint8 *data, uint8 len) {
	for (int i = 0; i < len; i++) {
		HAL_I2C_Mem_Write( &hi2c1, (LCD_ADR << 1), 0xC0, 0x01, data++, 1, HAL_MAX_DELAY);
		delay_us(50);
	}
}
void LCD_CursorOn_Off(bool Cursor) {
	if(Cursor) {
		Byte = 0x0F;

	} else {
		Byte = 0x0C;
	}
	HAL_I2C_Mem_Write( &hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY);
	delay_us(50);
}

void LCD_Clear() {
	Byte = 0x01;
	HAL_I2C_Mem_Write( &hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY);
	delay_us(50);
}

void TestRig_MainMenu() {
	  LCD_Clear();
	  LCD_setCursor(1, 6);
	  sprintf(Buffer,"Test Rig");
	  LCD_printf(&Buffer[0], strlen(Buffer));
	  LCD_setCursor(3, 0);
	  LCD_CursorOn_Off(false);
	  LoomConnected = None;
	  scan_loom();
}

void LCD_ClearLine(uint8 Line) {
	sprintf(Buffer, "                    ");
	LCD_setCursor(Line, 0);
	LCD_printf(&Buffer[0], strlen(Buffer));
}
