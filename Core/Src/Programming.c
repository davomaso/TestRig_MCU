/*
 * Programming.c
 */
#include <main.h>
#include "fatfs.h"
#include "interogate_project.h"
#include "Programming.h"
#include "File_Handling.h"

_Bool populatePageBuffer(uns_ch*, uint16*,uns_ch*,uint8*);
_Bool VerifyPage(uint8 , uns_ch *);

void ProgramTargetBoard (TboardConfig * Board) {
	uns_ch tempLine[100];
	uint8 Pos;
	uns_ch Command;
	uint16 ProgressBarTarget;
	uns_ch LineBuffer[MAX_LINE_LENGTH];
	uint8 LineBufferPosition;
	uns_ch PageBuffer[MAX_PAGE_LENGTH];
	uint16 PageBufferPosition = 0;
	uint16 count = 0;
	uns_ch RecBuffer[MAX_PAGE_LENGTH];
	uint16 page = 0;
	uint8 Percentage = 0;
	char * fileLocation = malloc(MAX_FILE_NAME_LENGTH * sizeof(char));

	ProgrammingInit();

	if (FindBoardFile(Board, fileLocation)) {
		OpenFile(fileLocation);
		TestRig_MainMenu();
		LCD_setCursor(2, 5);
		sprintf(debugTransmitBuffer, "Programming");
		LCD_printf(&debugTransmitBuffer, strlen(debugTransmitBuffer));
		// File size divide by two as .hex file is in ascii character form
		// Divide by the length of each page
		// Multiply by 70% to get an approximation of the actual data to transmit to the target board
		ProgressBarTarget = round( (float) ((fileSize / 2) / flashPagelen) * 0.7);
		while (f_gets(&tempLine, sizeof(tempLine), &SDcard.file)) {
			sortLine(&tempLine, &LineBuffer[0], &LineBufferPosition);
			Pos = LineBufferPosition;
			if (populatePageBuffer(&PageBuffer[PageBufferPosition], &PageBufferPosition, &LineBuffer, &LineBufferPosition)) {
				uns_ch data[4];
				if (page == 0) {
					data[0] = 0x4D;
					data[1] = 0x00;
					data[2] = 0x00;
					data[3] = 0x00;
					HAL_SPI_Transmit(&hspi3, &data, 4, HAL_MAX_DELAY);
				}
				PageWrite(&PageBuffer[0], flashPagelen / 2, page);
				if (!VerifyPage(page, &PageBuffer[0])) {
					PageWrite(&PageBuffer[0], flashPagelen / 2, page);
					if (!VerifyPage(page, &PageBuffer[0])) {
						printT("Failed to Program Board\n");
						break;
					}
				}
				PageBufferPosition = 0;
				if (LineBufferPosition)
					populatePageBuffer(&PageBuffer[PageBufferPosition], &PageBufferPosition, &LineBuffer[Pos - LineBufferPosition], &LineBufferPosition);
				Percentage = (uint8) ((page / (float) ProgressBarTarget) * 100);
				ProgressBar(Percentage);
				page++;
			}
		}
		while (PageBufferPosition < MAX_PAGE_LENGTH) {
			PageBuffer[PageBufferPosition++] = 0xFF;
		}
		PageWrite(&PageBuffer[0], flashPagelen / 2, page);
		if (!VerifyPage(page, &PageBuffer[0])) {
			PageWrite(&PageBuffer[0], flashPagelen / 2, page);
			if (!VerifyPage(page, &PageBuffer[0])) {
				printT("Failed to Program Board\n");
			}
		}
		printT("Programming Done\n");
		Close_File(fileLocation);

		PageBufferPosition = page = 0;
		SPI3->CR1 &= ~(SPI_BAUDRATEPRESCALER_64);
		SPI3->CR1 |= (0xFF & SPI_BAUDRATEPRESCALER_256);
		SetClkAndLck();
		//HAL_Delay(100); // check if required
		HAL_GPIO_WritePin(TB_Reset_GPIO_Port, TB_Reset_Pin, GPIO_PIN_SET);

		SET_BIT(BoardConnected.BSR, BOARD_PROGRAMMED);
		Command = 0x08;
		SetPara(Command);
		communication_array(Command, &Para, Paralen);
		CurrentState = csInterogating;
		ProcessState = psWaiting;
	} else {
		printT("Could not find hex file...\n");
		CurrentState = csProgramming;
		ProcessState = psFailed;
	}
}

void sortLine(uns_ch *Line, uns_ch *lineBuffer, uint8 *Position) {
	/*
	 * Routine to sort through a line of INTEL hex format data
	 * Put the data into a line buffer that can be handled by the populate page buffer routine.
	 */
	uint8 data[4];
	*Position = 0;
	if (*Line++ == ':') {
		len = Ascii2hex(Line);
		Line += 2;
		hexAddress = (Ascii2hex(Line) << 8);
		Line += 2;
		hexAddress += Ascii2hex(Line);
		Line += 2;
		hexRecType = Ascii2hex(Line);
		Line += 2;
		while (len > 0) {
			len--;
			*lineBuffer = Ascii2hex(Line);
			lineBuffer++;
			(*Position)++;
			Line += 2;
		}
		hexCheckSum = *Line++;
	}
}

_Bool populatePageBuffer(uns_ch *Page, uint16 *PagePos, uns_ch *Line, uint8 *LinePos) {
	/*
	 * Populate the page buffer with a line of data, if the page is populated return true so the page
	 * can be programmed to the target board.
	 * Else return false if the whole line is loaded into the buffer so that more data can be loaded into a line
	 * to be repopulated until the buffer is full.
	 */
	if ((*PagePos + *LinePos) <= MAX_PAGE_LENGTH) {
		memcpy(Page, Line, *LinePos);
		*PagePos += *LinePos;
		*LinePos = 0;
		if (*PagePos == flashPagelen)
			return true;
		else
			return false;
	} else if ((*PagePos + *LinePos) > MAX_PAGE_LENGTH) {
		uint8 dataLen;
		dataLen = MAX_PAGE_LENGTH - *PagePos;
		memcpy(Page, Line, dataLen);
		*LinePos -= dataLen;
		return true;
	}
	return false;
}

void SetClkAndLck() {
	/*
	 * Following successful programmming, and verification this routine is used to set the clk and lock bytes so the
	 * target boards can operate with the expected 3-8Mhz external XTAL.
	 */

	uint8 data[4];
	uint8 response[4];
	PollReady();
	//Fuse Low Byte
	data[0] = 0xAC;
	data[1] = 0xA0;
	data[2] = 0x00;
	data[3] = 0xDD;
	HAL_SPI_TransmitReceive(&hspi3, &data[0], &response[0], 4, HAL_MAX_DELAY);
	response[3] = 0x00;
	while (response[3] != 0xDD) {
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
	data[3] = 0xD7;
	HAL_SPI_TransmitReceive(&hspi3, &data[0], &response[0], 4, HAL_MAX_DELAY);
	response[3] = 0x00;
	while (response[3] != 0xD7) {
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
	data[3] = 0xFD;
	HAL_SPI_TransmitReceive(&hspi3, &data[0], &response[0], 4, HAL_MAX_DELAY);
	HAL_Delay(25);
	response[3] = 0x00;
	while (response[3] != 0xFD) {
		data[0] = 0x50;
		data[1] = 0x08;
		data[2] = 0x00;
		data[3] = 0x00;
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

void ProgrammingInit() {
	/*
	 * Routine to enable programming
	 * Poll the device until ready, erase device, and change clk to 8Mhz to program at a greater speed
	 */
	uint8 data[4];
	uint8 response[4];
	response[2] = 0;
	uint8 SignatureByte[3];
	SPI3->CR1 &= ~(SPI_BAUDRATEPRESCALER_64);
	SPI3->CR1 |= (0xFF & SPI_BAUDRATEPRESCALER_256);

	HAL_GPIO_WritePin(TB_Reset_GPIO_Port, TB_Reset_Pin, GPIO_PIN_RESET);
	HAL_Delay(50);

	//Enable Programming
	while (response[2] != 0x53) {
		data[0] = 0xAC;
		data[1] = 0x53;
		data[2] = 0x00;
		data[3] = 0x00;
		HAL_SPI_TransmitReceive(&hspi3, &data[0], &response[0], 4, HAL_MAX_DELAY);
	}
	//Erase Chip
	data[0] = 0xAC;
	data[1] = 0x80;
	data[2] = 0x00;
	data[3] = 0x00;
	HAL_SPI_Transmit(&hspi3, &data[0], 4, HAL_MAX_DELAY);
	//Poll RDY
	data[0] = 0xF0;
	data[1] = 0x00;
	while (1) {
		HAL_SPI_TransmitReceive(&hspi3, &data[0], &response[0], 4, HAL_MAX_DELAY);
		if (((response[3] & 0x01) == 0))
			break;
	}
	//Read Signature byte
	data[0] = 0x30;
	HAL_SPI_TransmitReceive(&hspi3, &data[0], &response[0], 4, HAL_MAX_DELAY);
	data[2]++;
	SignatureByte[0] = response[3];
	HAL_SPI_TransmitReceive(&hspi3, &data[0], &response[0], 4, HAL_MAX_DELAY);
	SignatureByte[1] = response[3];
	data[2]++;
	HAL_SPI_TransmitReceive(&hspi3, &data[0], &response[0], 4, HAL_MAX_DELAY);
	SignatureByte[2] = response[3];

	//Find Signature Byte
	if ((SignatureByte[0] == 0x1E) && (SignatureByte[1] == 0x96)
			&& (SignatureByte[2] == 0x09)) {
		TBmicro = Tatmega644;
		flashPagelen = 256;
		eepromPagelen = 8;
	} else if ((SignatureByte[0] == 0x1E) && (SignatureByte[1] == 0x96)
			&& (SignatureByte[2] == 0x0A)) {
		TBmicro = Tatmega644p;
		flashPagelen = 256;
		eepromPagelen = 8;
	} else {
		TBmicro = Tnone;
	}
	if ((TBmicro == Tatmega644) || (TBmicro == Tatmega644p)) {
		//Set Clk to 8Mhz
		data[0] = 0xAC;
		data[1] = 0xA0;		//Fuse Low Byte
		data[2] = 0x00;
		data[3] = 0xD2;
		HAL_SPI_TransmitReceive(&hspi3, &data[0], &response[0], 4, HAL_MAX_DELAY);
		HAL_Delay(20);
		data[1] = 0xA8;		//Fuse High Byte
		data[3] = 0xD7;
		HAL_SPI_TransmitReceive(&hspi3, &data[0], &response[0], 4, HAL_MAX_DELAY);
		HAL_Delay(20);
		data[1] = 0xA4;		//Fuse Extended Byte
		data[3] = 0xFD;
		HAL_SPI_TransmitReceive(&hspi3, &data[0], &response[0], 4, HAL_MAX_DELAY);
		HAL_Delay(20);
		SPI3->CR1 &= ~(SPI_BAUDRATEPRESCALER_256);
		SPI3->CR1 |= (0xFF & SPI_BAUDRATEPRESCALER_64);
		ProgrammingCount = 0;
	}
	// Read Fuse Bits
	data[0] = 0x50;
	data[1] = 0x00;
	data[2] = 0x00;
	data[3] = 0x00;
	HAL_SPI_TransmitReceive(&hspi3, &data[0], &response[0], 4, HAL_MAX_DELAY);
	data[1] = 0x08;
	HAL_SPI_TransmitReceive(&hspi3, &data[0], &response[0], 4, HAL_MAX_DELAY);
	data[0] = 0x58;
	HAL_SPI_TransmitReceive(&hspi3, &data[0], &response[0], 4, HAL_MAX_DELAY);
}

uint8 findVer(char *data) {
	/*
	 * Find the version of firmware of the string that was passed to the routine.
	 * Routine should take any string, searching through the string until a v is found
	 * returning integers following the 'v' or 'V'
	 */
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

char Ascii2hex(char *ch) {
	/*
	 * Routine to convert ascii characters to hex so each byte read from the files on the SD card can be correctly
	 * read prior to programming
	 */
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

void loadByte(uint8_t hilo, uint8 page, uint8 addr, uint8 data) {
	spi_transaction(0x40 + 8 * hilo, page, addr, data);
}

void spi_transaction(uint8 a, uint8 b, uint8 c, uint8 d) {
	// Transmit 4 bytes of data on SPI3, routine used typically for programming the target devices
	uint8 data[4];
	data[0] = a;
	data[1] = b;
	data[2] = c;
	data[3] = d;
	HAL_SPI_Transmit(&hspi3, &data[0], 4, HAL_MAX_DELAY);
}

void PageWrite(uint8 *buff, uint16 length, uint8 page) {
	/*
	 * Take buffer of 'length' bytes long and write page
	 *
	 * Load each byte onto the device, incrementing the address from 0-255
	 * Once limit is reached write the page with the page commit routine.
	 *
	 */
	uint8 adr;
	for (adr = 0; adr < length; adr++) {
		loadByte(LOW, 0, adr, *buff++);
		loadByte(HIGH, 0, adr, *buff++);
	}
	pageCommit(page);
	PollReady();
}

void PollReady() {
	//Poll Ready
	uns_ch data[4];
	uns_ch receive[4];
	receive[3] = 0xFF;

	data[0] = 0xF0;
	data[1] = 0x00;
	data[2] = 0x00;
	data[3] = 0x00;
	while ((receive[3] & 0x01) != 0) {
		HAL_SPI_TransmitReceive(&hspi3, &data[0], &receive[0], 4, HAL_MAX_DELAY);
	}
}

void pageCommit(uint8 currPage) {
	spi_transaction(0x4C, (currPage >> 1), ( (currPage & 0x01) << 7), 0x00);
}

_Bool ContinueWithCurrentProgram() {
	/*
	 * Take user input to determine whether to continue with programming or continue with
	 * the current firmware that is currently loaded onto the board.
	 */
	LCD_ClearLine(2);
	sprintf(lcdBuffer, "  Update Program?");
	printT(&lcdBuffer);
	LCD_setCursor(2, 0);
	LCD_printf(&lcdBuffer, strlen(lcdBuffer));

	LCD_ClearLine(3);
	sprintf(lcdBuffer, "Current Version:  %x", BoardConnected.Version);
	LCD_setCursor(3, 0);
	LCD_printf(&lcdBuffer, strlen(lcdBuffer));
	printT(&lcdBuffer);
//	printT(" \n");

	LCD_ClearLine(4);
	sprintf(lcdBuffer, "*-Reprogram   #-Keep");
	printT(&lcdBuffer);
//	printT(" \n");
	LCD_setCursor(4, 0);
	LCD_printf(&lcdBuffer, strlen(lcdBuffer));
	while (1) {
		if (KP_hash.Pressed) {
			KP_hash.Count = KP_hash.Pressed = 0;
			SET_BIT(BoardConnected.BSR, BOARD_PROGRAMMED);
			return false;
		}
		if (KP_star.Pressed) {
			KP_star.Count = KP_star.Pressed = 0;
			CLEAR_BIT(BoardConnected.BSR, BOARD_PROGRAMMED);
			return true;
		}
	}
	return false;
}

_Bool EnableProgramming() {
	/*
	 * Routine to enable programming and poll until ready
	 */
	uns_ch data[4];
	uns_ch response[4];
	//Reenable programming prior to writing a page,
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
	while (ProcessState) {//TODO: Add timeout functionality here if no response is received
		HAL_SPI_TransmitReceive(&hspi3, &data[0], &response[0], 4, HAL_MAX_DELAY);
		if (((response[3] & 0x01) == 0))
			return true;
	}
	return false;
}

_Bool VerifyPage(uint8 page, uns_ch *PageByte) {
	/*
	 * Routine to verify the data programmed to the device was correct comparing to the page buffer
	 */
	uns_ch LowByte;
	uns_ch HighByte;
	uns_ch data[4];
	uns_ch receive[4];
	uns_ch RecBuffer[MAX_PAGE_LENGTH];
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

void ProgressBar(uint8 Percentage) {
	/*
	 * Passing a percentage to this routine will update the progress bar present on the LCD screen
	 */
	uns_ch Byte;
	uns_ch ProgressBarBlock = 0x1F;
	uns_ch HalfProgressBarBlock = 0xD9;
	uint8 Pos;
	if (Percentage <= 100) {
		if (!(Percentage & 0x01)) {
			LCD_setCursor(4, 9);
			sprintf(debugTransmitBuffer, "%d", Percentage);
			LCD_printf(&debugTransmitBuffer, strlen(debugTransmitBuffer));
			Byte = 0x25;
			LCD_printf(&Byte, 1);
			switch (Percentage % 10) {
			case 0:
				Byte = 0xD0;
				LCD_setCursor(3, (5 + (Percentage / 10)));
				LCD_printf(&Byte, 1);
				Pos++;
				break;
			case 2:
				Byte = 0xD4;
				LCD_setCursor(3, (6 + (Percentage / 10)));
				LCD_printf(&Byte, 1);
				break;
			case 4:
				Byte = 0xD3;
				LCD_setCursor(3, (6 + (Percentage / 10)));
				LCD_printf(&Byte, 1);
				break;
			case 6:
				Byte = 0xD2;
				LCD_setCursor(3, (6 + (Percentage / 10)));
				LCD_printf(&Byte, 1);
				break;
			case 8:
				Byte = 0xD1;
				LCD_setCursor(3, (6 + (Percentage / 10)));
				LCD_printf(&Byte, 1);
				break;
			}
		}
	}

}
