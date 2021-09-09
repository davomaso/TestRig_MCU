/*
 * Programming.h
 *
 *  Created on: Dec 21, 2020
 *      Author: mason
 */

#ifndef SRC_PROGRAMMING_H_
#define SRC_PROGRAMMING_H_

#include <main.h>
#include "Global_Variables.h"



/*
 * Programming.c
 */
#include <main.h>
#include "fatfs.h"
#include "Global_Variables.h"
#include "Programming.h"
#include "File_Handling.h"

extern SD_HandleTypeDef hsd;

void sortLine(uns_ch *, uns_ch *, uint8 *);
_Bool populatePageBuffer(uns_ch *, uint16 *, uns_ch *, uint8 *);
void SetClkAndLck(TboardConfig *);
void ProgrammingInit(void);
uint8 findVer(char *);
char Ascii2hex(char *);
void loadByte(uint8_t, uint8, uint8, uint8);
void spi_transaction(uint8, uint8, uint8, uint8);
void PageWrite(uint8 *, uint16, uint8);
void PollReady(void);
void pageCommit(uint8);
_Bool ContinueWithCurrentProgram(void);
_Bool EnableProgramming(void);
_Bool VerifyPage(uint8, uns_ch *);
void ProgressBar(uint8);
void SetSDclk(_Bool);

#define LOW 0
#define HIGH 1


#define MAX_LINE_LENGTH 100
#define MAX_PAGE_LENGTH 256

//uint8 data[4];
uint16 hexAddress;
uns_ch hexRecType;
uint8 hexCheckSum;
char ProgrammingBuffer[256];
uint8 ProgrammingCount;

//_Bool Programming;
uint8 Program_CountDown;

uns_ch Command;
uint16 ProgressBarTarget;
uns_ch LineBuffer[MAX_LINE_LENGTH];
uint8 LineBufferCount;
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
