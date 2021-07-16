/*
 * Programming.h
 *
 *  Created on: Dec 21, 2020
 *      Author: mason
 */

#ifndef SRC_PROGRAMMING_H_
#define SRC_PROGRAMMING_H_

#include <main.h>
#include "interogate_project.h"

char Ascii2hex(char *);

#define LOW 0
#define HIGH 1
uint16 current_page(void);
void write_flash_pages(uint8 *, int);
void spi_transaction(uint8, uint8, uint8, uint8);


#define STK_OK      0x10
#define STK_FAILED  0x11
#define STK_UNKNOWN 0x12
#define STK_INSYNC  0x14
#define STK_NOSYNC  0x15
#define CRC_EOP     0x20

#define MAX_LINE_LENGTH 100
#define MAX_PAGE_LENGTH 256

//uint8 data[4];
uint16 hexAddress;
uns_ch hexRecType;
uint8 hexCheckSum;
uint8 len;
char ProgrammingBuffer[256];
uint8 ProgrammingCount;

//_Bool Programming;
uint8 Program_CountDown;

uns_ch tempLine[100];
uint8 LeftOverLineDataPos;
uns_ch Command;
uint16 ProgressBarTarget;
uns_ch LineBuffer[MAX_LINE_LENGTH];
uint8 LineBufferPosition;
uns_ch PageBuffer[MAX_PAGE_LENGTH];
uint16 PageBufferPosition;
uint16 count;
uns_ch RecBuffer[MAX_PAGE_LENGTH];
uint16 page ;
uint8 Percentage;






uint32 fileSize;
uint16 eepromPagelen;
uint16 extendedPage;

#endif /* SRC_PROGRAMMING_H_ */
