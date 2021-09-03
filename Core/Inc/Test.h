/*
 * Test.h
 *
 *  Created on: 30 Jul 2020
 *      Author: Admin
 */
#ifndef INC_TEST_H_
#define INC_TEST_H_

#define SML_BUFFER 16
typedef unsigned short uint16;

#include "main.h"
#include "Board_Config.h"
#include "stdbool.h"

void ConfigInit(void);
void TestConfig935x(TboardConfig *);
void TestConfig937x(TboardConfig *);
void TestConfig401x(TboardConfig *);
void TestConfig402x(TboardConfig *);
void TestConfig422x(TboardConfig *);
void TestConfig427x(TboardConfig *);
void SetTestParam(TboardConfig *, uint8, uns_ch *, uint8 *);
void Set_Test(TboardConfig *, uint8, uint8);
_Bool CheckTestNumber(TboardConfig *);

float CHval[10][16];

_Bool SDIenabled;
_Bool RS485enabled;


#endif /* INC_TEST_H_ */
