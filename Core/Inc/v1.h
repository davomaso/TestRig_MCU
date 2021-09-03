/*
 * v1.h
 *
 *  Created on: 2 Sep 2021
 *      Author: mason
 */

#ifndef INC_V1_H_
#define INC_V1_H_

/*
 * v1.c
 *
 *  Created on: 30 Jun 2021
 *      Author: mason
 */
#include "main.h"
#include "Global_Variables.h"

void clearTestStatusLED(void);
void testInputVoltage(void);
void testSolarCharger(void);
void checkFuseVoltage(TboardConfig *);
void CheckPowerRegisters(TboardConfig *);
void TestComplete(TboardConfig *);

#endif /* INC_V1_H_ */
