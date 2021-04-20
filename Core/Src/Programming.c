/*
 * Programming.c
 */
#include <main.h>
#include "interogate_project.h"
#include "Programming.h"

void sortLine(uns_ch *Line, uns_ch *lineBuffer, uint8 * Position) {
	uint8 data[4];
	* Position = 0;
	if (*Line++ == ':') {
		len = Ascii2hex(Line);
		Line += 2;
		hexAddress = (Ascii2hex(Line) << 8);
		Line += 2;
		hexAddress += Ascii2hex(Line);
		Line += 2;
		hexRecType = Ascii2hex(Line);
		Line += 2;
		while(len > 0) {
					len--;
					*lineBuffer = Ascii2hex(Line);
					lineBuffer++;
					(*Position)++;
					Line += 2;
			}
		hexCheckSum = *Line++;
	}
}

_Bool populatePageBuffer(uns_ch * Page, uint8 * PagePos, uns_ch * Line, uint8 * LinePos ) {
	if ( (*PagePos + *LinePos) < MAX_PAGE_LENGTH ) {
		memcpy(Page, Line, *LinePos);
		*PagePos += *LinePos;
		*LinePos = 0;
		if (*PagePos == flashPagelen)
			return true;
		else
			return false;
	} else if ( (*PagePos + *LinePos ) > MAX_PAGE_LENGTH) {
		uint8  dataLen;
		 dataLen = MAX_PAGE_LENGTH - *PagePos;
		memcpy(Page, Line, dataLen);
		*LinePos -= dataLen;
		return true;
	}
	return false;
}

void SetClkAndLck() {
	uint8 data[4];
	uint8 response[4];
		//Set Clk to 8Mhz
	data[0] = 0xAC;
	data[1] = 0xA0;		//Fuse Low Byte
	data[2] = 0x00;
	data[3] = 0xDD;
	HAL_SPI_TransmitReceive(&hspi3, &data[0], &response[0], 4, HAL_MAX_DELAY);
	data[1] = 0xA8;		//Fuse High Byte
	data[3] = 0xD7;
	HAL_SPI_TransmitReceive(&hspi3, &data[0], &response[0], 4, HAL_MAX_DELAY);
	data[1] = 0xA4;		//Fuse Extended Byte
	data[3] = 0xFC;
	HAL_SPI_TransmitReceive(&hspi3, &data[0], &response[0], 4, HAL_MAX_DELAY);
}

void ProgrammingInit() {
	uint8 data[4];
	uint8 response[4];
	uint8 SignatureByte[3];

	HAL_GPIO_WritePin(TB_Reset_GPIO_Port, TB_Reset_Pin, GPIO_PIN_RESET);
	HAL_Delay(50);

		//Enable Programming
	while(response[2] != 0x53) {
		data[0] = 0xAC;
		data[1] = 0x53;
       	data[2] = 0x00;
		data[3] = 0x00;
		HAL_SPI_TransmitReceive(&hspi3, &data[0],&response[0], 4, HAL_MAX_DELAY);
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
	while(1) {
		HAL_SPI_TransmitReceive(&hspi3, &data[0], &response[0], 4, HAL_MAX_DELAY);
		if( ((response[3] & 0x01) == 0) )
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
	if ( (SignatureByte[0] == 0x1E) && (SignatureByte[1] == 0x96) && (SignatureByte[2] == 0x09) ) {
		TBmicro = Tatmega644;
		flashPagelen = 256;
		eepromPagelen = 8;
	} else if ((SignatureByte[0] == 0x1E) && (SignatureByte[1] == 0x96) && (SignatureByte[2] == 0x0A)) {
		TBmicro = Tatmega644p;
		flashPagelen = 256;
		eepromPagelen = 8;
	} else {
		TBmicro = Tnone;
	}
	if ( (TBmicro == Tatmega644) || (TBmicro == Tatmega644p) ) {
			//Set Clk to 8Mhz
		data[0] = 0xAC;
		data[1] = 0xA0;		//Fuse Low Byte
		data[2] = 0x00;
		data[3] = 0xE2;
		HAL_SPI_TransmitReceive(&hspi3, &data[0], &response[0], 4, HAL_MAX_DELAY);
		HAL_Delay(20);
		data[1] = 0xA8;		//Fuse High Byte
		data[3] = 0x91;
		HAL_SPI_TransmitReceive(&hspi3, &data[0], &response[0], 4, HAL_MAX_DELAY);
		HAL_Delay(20);
		data[1] = 0xA4;		//Fuse Extended Byte
		data[3] = 0xF8;
		HAL_SPI_TransmitReceive(&hspi3, &data[0], &response[0], 4, HAL_MAX_DELAY);
		HAL_Delay(20);
		SPI3->CR1 &= ~(SPI_BAUDRATEPRESCALER_256);
		SPI3->CR1 |= (0xFF & SPI_BAUDRATEPRESCALER_32);
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

uint8 findVer(char * data) {
	while (*data != 'v') data++;
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
	char hex;
	if (*ch < 0x40)
		hex = ( (*ch++ - 0x30) << 4);
	else
		hex = ( (*ch++ - 55) << 4);
	if (*ch < 0x40)
		hex += (*ch++ - 0x30);
	else
		hex += (*ch++ - 55);
	return hex;
}

void loadByte (uint8_t hilo, uint8 addr, uint8 * data) {
  spi_transaction(0x40 + 8 * hilo, 0x00, addr, *data);
}

void spi_transaction(uint8 a, uint8 b, uint8 c, uint8 d) {
  uint8 data[4];
  data[0] = a;
  data[1] = b;
  data[2] = c;
  data[3] = d;
  HAL_SPI_Transmit(&hspi3, &data[0], 4, HAL_MAX_DELAY);
}

void PageWrite(uint8 *buff, int length) {
  int adr = 0;
  while (adr < length) {
    loadByte(LOW, adr, buff++);
    loadByte(HIGH, adr, buff++);
    adr++;
  }
  pageCommit(page);
  page++;
}

void pageCommit(uint16 addr) {
  spi_transaction(0x4C, addr , 0x00, 0x00);
  HAL_Delay(5);
}
	//Only required if the microcontroller runs more than 64Kbytes of flash memory
//void WriteExtendedPage() {
//  if (flashPagelen == 32) {
//	  spi_transaction(0x4D, 0x00, extendedPage++, 0x00);
//  }  else if (flashPagelen == 64) {
//	  spi_transaction(0x4D, 0x00, extendedPage++, 0x00);
//  }  else if (flashPagelen == 128) {
//	  spi_transaction(0x4D, 0x00, extendedPage++, 0x00);
//  }  else if (flashPagelen == 256) {
//	  spi_transaction(0x4D, 0x00, extendedPage++, 0x00);
//  }
//}
_Bool ContinueWithCurrentProgram(){
	LCD_ClearLine(2);
	sprintf(lcdBuffer, "  Update Program?");
	LCD_setCursor(2, 0);
	LCD_printf(&lcdBuffer, strlen(lcdBuffer));

	LCD_ClearLine(3);
	sprintf(lcdBuffer, "      %x",BoardConnected.Version);
	LCD_setCursor(3, 0);
	LCD_printf(&lcdBuffer, strlen(lcdBuffer));

	LCD_ClearLine(4);
	sprintf(lcdBuffer, "*-Keep   #-Reprogram");
	LCD_setCursor(4, 0);
	LCD_printf(&lcdBuffer, strlen(lcdBuffer));
	while(1) {
		if(KP_hash.Pressed){
			KP_hash.Count = KP_hash.Pressed = 0;
			CLEAR_BIT( BoardConnected.BSR, BOARD_PROGRAMMED );
			return true;
		}
		if(KP_star.Pressed){
			KP_star.Count = KP_star.Pressed = 0;
			SET_BIT( BoardConnected.BSR, BOARD_PROGRAMMED );
			return false;
		}
	}
	return false;
}

_Bool EnableProgramming() {
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
	while(ProcessState) {	//TODO: Add timeout functionality here if no response is received
		HAL_SPI_TransmitReceive(&hspi3, &data[0], &response[0], 4, HAL_MAX_DELAY);
		if( ((response[3] & 0x01) == 0) )
			return true;
		}
	return false;
}

