/*
 * SetVsMeasured.h
 *
 *  Created on: 11 Sep 2020
 *      Author: Admin
 */

#ifndef INC_SETVSMEASURED_H_
#define INC_SETVSMEASURED_H_

#include "Board_Config.h"

typedef enum {TRpassed = 'p', TRfailed = 'f'}Tresult;

void HandleResults(TboardConfig *, float *);
Tresult CompareSetMeasured(float *, float, float);

typedef enum {TTLatch = 'L', TTVoltage = 'V', TTCurrent = 'I', TTAsync = 'A', TTSDI = 'S', TTRS485 = 'R',TTNoTest = '0'}TtestType;
TtestType PortTypes[10];

#define GET_VOLTAGE_TEST_TOLERANCE(RANGE, VAL) ((RANGE * 0.0075) + (0.005 * VAL));
#define GET_CURRENT_TEST_TOLERANCE(VAL) ((20 * 0.005) + (0.005 * VAL) );

#endif /* INC_SETVSMEASURED_H_ */
