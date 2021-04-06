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

void CheckTestNumber(uint8, uint8);

void TestConfig935x(TboardConfig *);
void TestConfig937x(TboardConfig *);
void TestConfig401x(TboardConfig *);
void TestConfig402x(TboardConfig *);
void TestConfig422x(TboardConfig *);
void TestConfig427x(TboardConfig *);


	//Test Quantities to pass to the SetTestParam function
uint8 TestNum;
uint8 ParamNum;
uint8 PortNum;
uint8 LatchNum;
uint8 AnalogNum;
uint8 DigitalNum;

uint8 LatchState[4];

uint8 currPort;		//Current Port to be set
float CHval[10][16];
_Bool input_port;

_Bool SDIenabled;



#endif /* INC_TEST_H_ */
