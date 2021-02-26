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
void write_flash_pages(int, uint8 *);
void spi_transaction(uint8, uint8, uint8, uint8);


#define STK_OK      0x10
#define STK_FAILED  0x11
#define STK_UNKNOWN 0x12
#define STK_INSYNC  0x14
#define STK_NOSYNC  0x15
#define CRC_EOP     0x20

//uint8 data[4];
uint16 hexAddress;
uns_ch hexRecType;
uint8 hexCheckSum;
uint8 len;
char ProgrammingBuffer[256];
uint16 ProgrammingCount;

_Bool Programming;
uint8 Program_CountDown;
typedef enum {Tatmega644 = 644, Tatmega644p = 645, Tnone = 0}TtbMicroController;

TtbMicroController TBmicro;

uint16 flashPagelen;
uint16 eepromPagelen;
uint16 here;


#endif /* SRC_PROGRAMMING_H_ */