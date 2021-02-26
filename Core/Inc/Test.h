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

TboardConfig TestConfig935x();
TboardConfig TestConfig937x();
TboardConfig TestConfig401x();
TboardConfig TestConfig402x();
TboardConfig TestConfig422x();
TboardConfig TestConfig427x();


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

unsigned char TestCode[SML_BUFFER];
uint8 Test_Port;

#endif /* INC_TEST_H_ */
