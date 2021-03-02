/*
 * Programming.c
 */
#include <main.h>
#include "interogate_project.h"
#include "Programming.h"

void sortLine(char *Line) {
	uint8 data[4];
	if (*Line++ == ':') {
		len = Ascii2hex(Line);
		Line += 2;
		hexAddress = (Ascii2hex(Line) << 8);
		Line += 2;
		hexAddress += Ascii2hex(Line);
		Line += 2;
		hexRecType = Ascii2hex(Line);
		Line += 2;
		if (hexRecType == 0x01) {
			write_flash_pages(&ProgrammingBuffer[0], ProgrammingCount);
		} else {
				while(len > 0) {
					len--;
					ProgrammingBuffer[ProgrammingCount++] = Ascii2hex(Line);
					Line += 2;
					if (ProgrammingCount == 0xFF) {
						//Write Page
						write_flash_pages(&ProgrammingBuffer[0], ProgrammingCount/2);
						ProgrammingCount = 0;
				}
			}
		}
		hexCheckSum = *Line++;
	}
}

void SetClkAndLck() {
	uint8 data[4];
	uint8 response[4];
		//Set Clk to 8Mhz
	data[0] = 0xAC;
	data[1] = 0xA0;
	data[2] = 0x00;
	data[3] = 0xDD;
	HAL_SPI_TransmitReceive(&hspi3, &data[0], &response[0], 4, HAL_MAX_DELAY);
	data[1] = 0xA8;
	data[3] = 0xD7;
	HAL_SPI_TransmitReceive(&hspi3, &data[0], &response[0], 4, HAL_MAX_DELAY);
	data[1] = 0xA4;
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
		data[1] = 0xA0;
		data[2] = 0x00;
		data[3] = 0xDD;
		HAL_SPI_TransmitReceive(&hspi3, &data[0], &response[0], 4, HAL_MAX_DELAY);
		data[1] = 0xA8;
		data[3] = 0xD7;
		HAL_SPI_TransmitReceive(&hspi3, &data[0], &response[0], 4, HAL_MAX_DELAY);
		data[1] = 0xA4;
		data[3] = 0xFC;
		HAL_SPI_TransmitReceive(&hspi3, &data[0], &response[0], 4, HAL_MAX_DELAY);
		SPI3->CR1 &= ~(SPI_BAUDRATEPRESCALER_256);
		SPI3->CR1 |= (0xFF & SPI_BAUDRATEPRESCALER_8);
//		int byte = SPI3->CR1;
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

void loadByte (uint8_t hilo, uint8 addr, uint8_t * data) {
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

void write_flash_pages(uint8 *buff, int length) {
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
