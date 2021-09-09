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

uint8 runLatchTest(TboardConfig *, uint8);
_Bool LatchingSolenoidDriverTest(TboardConfig *);
uint8 LatchTestParam(uint8, bool);
void LatchTestInit(void);

#endif /* INC_LATCHTEST_H_ */
