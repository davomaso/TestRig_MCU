/*
 * LCD.h
 *
 *  Created on: 2 Sep 2021
 *      Author: mason
 */

#ifndef INC_LCD_H_
#define INC_LCD_H_

#include <main.h>

void LCD_init(void);
void LCD_setCursor(uint8, uint8);
void LCD_printf(uns_ch*, uint8, uint8 );
void LCD_displayString(uint8 *, uint8);
void LCD_CursorOn_Off(bool);
void LCD_Clear(void);
void TestRig_MainMenu();
void LCD_ClearLine(uint8);
void ChangeCharacterSet(uns_ch);

#endif /* INC_LCD_H_ */
