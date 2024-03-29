/*
 * Programming.c
 */
#include <main.h>
#include "fatfs.h"
#include "Init.h"
#include "LCD.h"
#include "Global_Variables.h"
#include "Programming.h"
#include "File_Handling.h"
#include "Communication.h"
#include "UART_Routine.h"

extern SD_HandleTypeDef hsd;

_Bool populatePageBuffer(uns_ch*, uint16*, uns_ch*, uint8*);
_Bool VerifyPage(uint8, uns_ch*);
_Bool EnableProgramming(void);
void SetSDclk(_Bool);

/*
 * sortLine() takes a full line of intel hex format data and removes the colon, and all unnecessary data for programming.
 * Given the data is read in ascii from the SD card ascii to hex function is required to get it in a form to program the device
 */
void sortLine(uns_ch *Line, uns_ch *lineBuffer, uint8 *Position) {
	/*
	 * Routine to sort through a line of INTEL hex format data
	 * Put the data into a line buffer that can be handled by the populate page buffer routine.
	 */
	uint8 len;
	*Position = 0;
	if (*Line++ == ':') {
		len = Ascii2hex((char*) Line);
		Line += 2;
		hexAddress = (Ascii2hex((char*) Line) << 8);
		Line += 2;
		hexAddress += Ascii2hex((char*) Line);
		Line += 2;
		hexRecType = Ascii2hex((char*) Line);
		Line += 2;
		while (len > 0) {
			len--;
			*lineBuffer = Ascii2hex((char*) Line);
			lineBuffer++;
			(*Position)++;
			Line += 2;
		}
		hexCheckSum = *Line++;
	}
}

/*
 * Populate the page buffer with a line of data, if the page is populated return true so the page
 * can be programmed to the target board.
 * Else return false if the whole line is loaded into the buffer so that more data can be loaded into a line
 * to be repopulated until the buffer is full.
 */
_Bool populatePageBuffer(uns_ch *Page, uint16 *PagePos, uns_ch *Line, uint8 *ByteCount) {
	if ((*PagePos + *ByteCount) <= MAX_PAGE_LENGTH) {
		memcpy(Page, Line, *ByteCount);
		*PagePos += *ByteCount;
		*ByteCount = 0;
		if (*PagePos == MAX_PAGE_LENGTH)
			return true;
		else
			return false;
	} else {
		uint8 dataLen;
		dataLen = MAX_PAGE_LENGTH - *PagePos;
		memcpy(Page, Line, dataLen);
		*ByteCount -= dataLen;
		memmove(Line, Line + dataLen, *ByteCount);
		*PagePos += dataLen;
		return true;
	}
	return false;
}

/*
 * Following successful programmming, and verification this routine is used to set the clk and lock bytes so the
 * target boards can operate with the expected 3-8Mhz external XTAL.
 */
void SetClkAndLck(TboardConfig *Board) {
	uint8 data[4];
	uint8 response[4];
	PollReady();
	//Fuse Low Byte
	data[0] = 0xAC;
	data[1] = 0xA0;
	data[2] = 0x00;
	data[3] = LOW_FUSE_BYTE;
	HAL_SPI_TransmitReceive(&hspi3, &data[0], &response[0], 4, HAL_MAX_DELAY);
	response[3] = 0x00;
	while (response[3] != LOW_FUSE_BYTE) {
		data[0] = 0x50;
		data[1] = 0x00;
		data[2] = 0x00;
		data[3] = 0x00;
		HAL_SPI_TransmitReceive(&hspi3, &data[0], &response[0], 4,
		HAL_MAX_DELAY);
	}

	//Fuse High Byte
	HAL_Delay(25);
	data[0] = 0xAC;
	data[1] = 0xA8;
	data[2] = 0x00;
	data[3] = HIGH_FUSE_BYTE;
	HAL_SPI_TransmitReceive(&hspi3, &data[0], &response[0], 4, HAL_MAX_DELAY);
	response[3] = 0x00;
	while (response[3] != HIGH_FUSE_BYTE) {
		data[0] = 0x58;
		data[1] = 0x08;		//Fuse Low Byte
		data[2] = 0x00;
		data[3] = 0x00;
		HAL_SPI_TransmitReceive(&hspi3, &data[0], &response[0], 4,
		HAL_MAX_DELAY);
	}

	//Fuse Extended Byte
	HAL_Delay(25);
	data[0] = 0xAC;
	data[1] = 0xA4;
	data[2] = 0x00;
	if ((Board->BoardType == b937x) || (Board->BoardType == b935x))
		data[3] = EXT_FUSE_BYTE_90;
	else
		data[3] = EXT_FUSE_BYTE_40;
	HAL_SPI_TransmitReceive(&hspi3, &data[0], &response[0], 4, HAL_MAX_DELAY);
	HAL_Delay(25);
	response[3] = 0x00;
	while ((response[3] != EXT_FUSE_BYTE_90) && (response[3] != EXT_FUSE_BYTE_40)) {
		data[0] = 0x50;
		data[1] = 0x08;
		data[2] = 0x00;
		if ((Board->BoardType == b937x) || (Board->BoardType == b935x))	// Ensure Brownout is set to 3.3V & 5V levels
			data[3] = EXT_FUSE_BYTE_90;
		else
			data[3] = EXT_FUSE_BYTE_40;
		HAL_SPI_TransmitReceive(&hspi3, &data[0], &response[0], 4,
		HAL_MAX_DELAY);
	}

	//Lock Byte
	data[0] = 0xAC;
	data[1] = 0xE0;
	data[2] = 0x00;
	data[3] = 0xFC;
	HAL_SPI_TransmitReceive(&hspi3, &data[0], &response[0], 4, HAL_MAX_DELAY);
	HAL_Delay(25);
	response[3] = 0x00;
	while (response[3] != 0xFC) {
		data[0] = 0x58;
		data[1] = 0x00;
		data[2] = 0x00;
		data[3] = 0x00;
		HAL_SPI_TransmitReceive(&hspi3, &data[0], &response[0], 4,
		HAL_MAX_DELAY);
	}
}

/*
 * Find the version of firmware of the string that was passed to the routine.
 * Routine should take any string, searching through the string until a v is found
 * returning integers following the 'v' or 'V'
 */
uint8 findVer(char *data) {
	while (*data != 'v')
		data++;
	uint8 tempVer[2];
	uint8 Ver;
	if (*data++ == 'v') {
		tempVer[0] = (*data++ - 0x30);
		tempVer[1] = (*data++ - 0x30);
	}
	Ver = (tempVer[0] << 4) + tempVer[1];
	return Ver;
}

/*
 * Routine to convert ascii characters to hex so each byte read from the files on the SD card can be correctly
 * read prior to programming
 */
char Ascii2hex(char *ch) {
	char hex;
	if (*ch < 0x40)
		hex = ((*ch++ - 0x30) << 4);
	else
		hex = ((*ch++ - 55) << 4);
	if (*ch < 0x40)
		hex += (*ch++ - 0x30);
	else
		hex += (*ch++ - 55);
	return hex;
}

//routine to load byte prior to page write
void loadByte(uint8_t hilo, uint8 page, uint8 addr, uint8 data) {
	spi_transaction(0x40 + 8 * hilo, page, addr, data);
}

// Transmit 4 bytes of data on SPI3, routine used typically for programming the target devices
void spi_transaction(uint8 a, uint8 b, uint8 c, uint8 d) {
	uint8 data[4];
	data[0] = a;
	data[1] = b;
	data[2] = c;
	data[3] = d;
	HAL_SPI_Transmit(&hspi3, &data[0], 4, HAL_MAX_DELAY);
}

/*
 * Take buffer of 'length' bytes long and write page
 * Load each byte onto the device, incrementing the address from 0-255
 * Once limit is reached write the page with the page commit routine.
 *
 */
void PageWrite(uint8 *buff, uint16 length, uint8 page) {
	uint8 adr;
	for (adr = 0; adr < length; adr++) {
		loadByte(LOW, 0, adr, *buff++);
		loadByte(HIGH, 0, adr, *buff++);
	}
	pageCommit(page);
	PollReady();
}

// Poll board until ready or programming timeout occurs
void PollReady() {
	//Poll Ready
	uns_ch data[4];
	uns_ch receive[4];
	receive[3] = 0xFF;
	ProgrammingTimeOut = 1500;

	data[0] = 0xF0;
	data[1] = 0x00;
	data[2] = 0x00;
	data[3] = 0x00;
	while (((receive[3] & 0x01) != 0) && ProgrammingTimeOut) {
		HAL_SPI_TransmitReceive(&hspi3, &data[0], &receive[0], 4, HAL_MAX_DELAY);
	}
}

// Once page is full or EOF is reached, pagecommit command is required to write data to board
void pageCommit(uint8 currPage) {
	spi_transaction(0x4C, (currPage >> 1), ((currPage & 0x01) << 7), 0x00);
}

/*
 * Take user input to determine whether to continue with programming or continue with
 * the current firmware that is currently loaded onto the board.
 */
_Bool ContinueWithCurrentProgram() {
	LCD_ClearLine(2);
	sprintf((char*) &lcdBuffer[0], "  Update Program?");
	printT((uns_ch*) &lcdBuffer[0]);
	LCD_setCursor(2, 0);
	LCD_displayString((uns_ch*) &lcdBuffer, strlen((char*) lcdBuffer));

	LCD_ClearLine(3);
	sprintf((char*) lcdBuffer, "Current Version:  %x", BoardConnected.Version);
	LCD_setCursor(3, 0);
	LCD_displayString((uns_ch*) &lcdBuffer, strlen((char*) lcdBuffer));
	printT((uns_ch*) &lcdBuffer);

	LCD_ClearLine(4);
	sprintf((char*) &lcdBuffer[0], "*-Reprogram   #-Keep");
	printT(&lcdBuffer[0]);
	LCD_setCursor(4, 0);
	LCD_displayString((uns_ch*) &lcdBuffer, strlen((char*) lcdBuffer));
	while (1) {
		if (KP[hash].Pressed) {
			KP[hash].Pressed = 0;
			SET_BIT(BoardConnected.BSR, BOARD_PROGRAMMED);
			return false;
		}
		if (KP[star].Pressed) {
			KP[star].Pressed = 0;
			CLEAR_BIT(BoardConnected.BSR, BOARD_PROGRAMMED);
			return true;
		}
	}
	return false;
}

/*
 * Routine to enable programming and poll until ready
 */
_Bool EnableProgramming() {
	uns_ch data[4];
	uns_ch response[4];
	//Reenable programming prior to writing a page,
	HAL_Delay(5);
	HAL_GPIO_WritePin(TB_Reset_GPIO_Port, TB_Reset_Pin, GPIO_PIN_RESET);
	HAL_Delay(20);
	//Enable Programming
	data[0] = 0xAC;
	data[1] = 0x53;
	data[2] = 0x00;
	data[3] = 0x00;
	HAL_SPI_Transmit(&hspi3, &data[0], 4, HAL_MAX_DELAY);
	//Poll Ready
	data[0] = 0xF0;
	data[1] = 0x00;
	ProgrammingTimeOut = 1500;
	while (ProcessState && ProgrammingTimeOut) {
		HAL_SPI_TransmitReceive(&hspi3, &data[0], &response[0], 4, HAL_MAX_DELAY);
		if (((response[3] & 0x01) == 0)) {
			return true;
		}
	}
	return false;
}


// Routine to verify the data programmed to the device was correct comparing to the page buffer
_Bool VerifyPage(uint8 page, uns_ch *PageByte) {
	uns_ch LowByte;
	uns_ch HighByte;
	uns_ch data[4];
	uns_ch receive[4];
	for (uint8 i = 0; i < 128; i++) {
		data[0] = 0x20;
		data[1] = (page >> 1);
		data[2] = ((page & 0x01) << 7) + i;
		data[3] = 0x00;
		HAL_SPI_TransmitReceive(&hspi3, &data[0], &receive[0], 4, HAL_MAX_DELAY); //
		LowByte = receive[3];
		RecBuffer[i * 2] = LowByte;
		data[0] = 0x28;
		HAL_SPI_TransmitReceive(&hspi3, &data[0], &receive[0], 4, HAL_MAX_DELAY); //
		HighByte = receive[3];
		RecBuffer[i * 2 + 1] = HighByte;
		if ((LowByte != *PageByte++) || (HighByte != *PageByte++)) {
			return false;
		}
	}
	return true;
}

/*
 * Passing a percentage to this routine will update the progress bar present on the LCD screen
 */
void ProgressBar(uint8 Percentage) {
	if (Percentage <= 100) {
		uns_ch Byte;
			LCD_setCursor(4, 9);
			if (!(Percentage % 5)) {
				sprintf((char*) &debugTransmitBuffer[0], "%d", Percentage);
				LCD_displayString((uns_ch*) &debugTransmitBuffer, strlen((char*) debugTransmitBuffer));
				Byte = 0x25;
				LCD_displayString(&Byte, 1);
			}
			switch (Percentage % 10) {
			case 0:
				Byte = 0xD0;
				LCD_setCursor(3, (5 + (Percentage / 10)));
				LCD_displayString(&Byte, 1);
				break;
			case 2:
				Byte = 0xD4;
				LCD_setCursor(3, (6 + (Percentage / 10)));
				LCD_displayString(&Byte, 1);
				break;
			case 4:
				Byte = 0xD3;
				LCD_setCursor(3, (6 + (Percentage / 10)));
				LCD_displayString(&Byte, 1);
				break;
			case 6:
				Byte = 0xD2;
				LCD_setCursor(3, (6 + (Percentage / 10)));
				LCD_displayString(&Byte, 1);
				break;
			case 8:
				Byte = 0xD1;
				LCD_setCursor(3, (6 + (Percentage / 10)));
				LCD_displayString(&Byte, 1);
				break;
			}
	}

}

// Set the speed at which the SDcard runs, slower speed is required for initialisation
void SetSDclk(_Bool OnOff) {
	if (OnOff) {
		hsd.Instance->CLKCR &= 0xFF00;	// clear the clock adjustment bits
		hsd.Instance->CLKCR |= 4;	// reduce the clk divider for faster operation
	} else {
		hsd.Instance->CLKCR &= 0xFF00;
		hsd.Instance->CLKCR |= 16;
	}
}
