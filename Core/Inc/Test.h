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

void TestConfig935x(TboardConfig *);
void TestConfig937x(TboardConfig *);
void TestConfig401x(TboardConfig *);
void TestConfig402x(TboardConfig *);
void TestConfig422x(TboardConfig *);
void TestConfig427x(TboardConfig *);


float CHval[10][16];

_Bool SDIenabled;



#endif /* INC_TEST_H_ */
