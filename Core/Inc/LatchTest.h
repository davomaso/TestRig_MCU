/*
 * LatchTest.h
 *
 *  Created on: 2 Sep 2021
 *      Author: mason
 */

#ifndef INC_LATCHTEST_H_
#define INC_LATCHTEST_H_

#include "main.h"
#include "Board_Config.h"

#define LATCH_TIME_OUT 2048 	// Approximately 2s before the latch test fails

uint8 runLatchTest(TboardConfig *, uint8);
_Bool LatchingSolenoidDriverTest(TboardConfig *);
uint8 LatchTestParam(uint8, bool);
void LatchTestInit(void);
void HandleLatchSample(void);
void PrintLatchResults(void);
void normaliseLatchResults(void);
void TransmitResults(TboardConfig*);

#endif /* INC_LATCHTEST_H_ */
