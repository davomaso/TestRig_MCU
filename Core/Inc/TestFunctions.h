/*
 * TestFunctions.h
 *
 *  Created on: 2 Sep 2021
 *      Author: mason
 */

#ifndef INC_TESTFUNCTIONS_H_
#define INC_TESTFUNCTIONS_H_

#include "main.h"
#include "Board_Config.h"

void TestFunction(TboardConfig *);
_Bool twoWireLatching(TboardConfig *, uint8 , _Bool );
float setCurrentTestDAC(uint8 );
float setVoltageTestDAC(uint8 , uint8 );
float setAsyncPulseCount(TboardConfig *, uint8 );
float setSDItwelveValue(uint8 );
void MUX_Sel(uint8 , uint8 );
void reset_ALL_MUX(void);
void ADC_MUXsel(uint8 );
void PrintLatchResults(void);
void normaliseLatchResults(void);
void TransmitResults(TboardConfig*);

#endif /* INC_TESTFUNCTIONS_H_ */
