#include <main.h>
#include "stdio.h"
#include "string.h"
#include "stdbool.h"
#include "LCD.h"
#include "Delay.h"
#include "UART_Routine.h"

void LCD_init() {
//	HAL_Delay(50);
//	HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_RESET);
//	HAL_Delay(2);
//	HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_SET);
	printT((uns_ch*) "\nInitialising LCD Screen...\n\n");
	uint8 Byte = 0x0F;
	HAL_I2C_Mem_Write(&hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY);
	HAL_Delay(1);

	Byte = 0x3A;
	HAL_I2C_Mem_Write(&hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY);
	HAL_Delay(1);

	Byte = 0x09;
	HAL_I2C_Mem_Write(&hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY);
	HAL_Delay(1);

	Byte = 0x06;
	HAL_I2C_Mem_Write(&hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY);
	HAL_Delay(1);

	Byte = 0x1E;
	HAL_I2C_Mem_Write(&hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY);
	HAL_Delay(1);

	Byte = 0x39;
	HAL_I2C_Mem_Write(&hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY);
	HAL_Delay(1);

	Byte = 0x1B;
	HAL_I2C_Mem_Write(&hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY);
	HAL_Delay(1);

	Byte = 0x6E;
	HAL_I2C_Mem_Write(&hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY);
	HAL_Delay(1);

	Byte = 0x57;
	HAL_I2C_Mem_Write(&hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY);
	HAL_Delay(1);

	Byte = 0x72;
	HAL_I2C_Mem_Write(&hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY);
	HAL_Delay(1);

	Byte = 0x38;
	HAL_I2C_Mem_Write(&hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY);
	HAL_Delay(1);

	Byte = 0x0F;
	HAL_I2C_Mem_Write(&hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY);
	HAL_Delay(1);

	Byte = 0x01;
	HAL_I2C_Mem_Write(&hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY);
	HAL_Delay(1);

	//Set DDRAM
	Byte = 0x80;
	HAL_I2C_Mem_Write(&hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY);
	HAL_Delay(1);
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
	LCD_ClearLine(Row);
	LCD_setCursor(Row, Col);
	LCD_displayString((uint8 *) data, strlen((char*) data));
}

void LCD_displayString(uint8 *data, uint8 len) {
	for (int i = 0; i < len; i++) {
		HAL_I2C_Mem_Write(&hi2c1, (LCD_ADR << 1), 0xC0, 0x01, data++, 1, HAL_MAX_DELAY);
		delay_us(50);
	}
}
void LCD_CursorOn_Off(bool Cursor) {
	uint8 Byte;
	if (Cursor) {
		Byte = 0x0F;

	} else {
		Byte = 0x0C;
	}
	HAL_I2C_Mem_Write(&hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY);
	delay_us(50);
}

void LCD_Clear() {
	uint8 CLScommand = 0x01;
	HAL_I2C_Mem_Write(&hi2c1, (LCD_ADR << 1), 0x00, 0x01, &CLScommand, 0x01, HAL_MAX_DELAY);
	delay_us(50);
}

void TestRig_MainMenu() {
	LCD_Clear();
	LCD_printf((uns_ch*) "     Test Rig     ", 1, 0);
	LCD_setCursor(3, 0);
	LCD_CursorOn_Off(false);
}

void LCD_ClearLine(uint8 Line) {
	sprintf((char*) &debugTransmitBuffer[0], "                    ");
	LCD_setCursor(Line, 0);
	LCD_displayString((uns_ch*) &debugTransmitBuffer[0], strlen((char*)debugTransmitBuffer));
}

void ChangeCharacterSet(uns_ch Set) {
	uns_ch Byte;
	Byte = 0x3A;
	HAL_I2C_Mem_Write(&hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY);
	delay_us(50);
	Byte = 0x72;
	HAL_I2C_Mem_Write(&hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY);
	delay_us(50);
	if (Set == 'A')
		Byte = 0x00;
	else if (Set == 'B')
		Byte = 0x04;
	else if (Set == 'C')
		Byte = 0x0C;
	HAL_I2C_Mem_Write(&hi2c1, (LCD_ADR << 1), 0xC0, 0x01, &Byte, 0x01, HAL_MAX_DELAY);
	delay_us(50);
	Byte = 0x38;
	HAL_I2C_Mem_Write(&hi2c1, (LCD_ADR << 1), 0x00, 0x01, &Byte, 0x01, HAL_MAX_DELAY);
	delay_us(50);
//	LCD_setCursor(2, 2);
//	Byte = 0x1F;
//	LCD_printf(&Byte, 1);
}
