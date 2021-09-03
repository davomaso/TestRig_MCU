/*
 * SetVsMeasured.h
 *
 *  Created on: 11 Sep 2020
 *      Author: Admin
 */

#ifndef INC_SETVSMEASURED_H_
#define INC_SETVSMEASURED_H_

#include "Board_Config.h"
void CompareResults(TboardConfig *, float *);

typedef enum {TTLatch = 'L', TTVoltage = 'V', TTCurrent = 'I', TTAsync = 'A', TTSDI = 'S', TTRS485 = 'R',TTNo = '0'}TtestType;
TtestType PortTypes[10];
typedef enum {TRpassed = 'p', TRfailed = 'f'}Tresult;

#endif /* INC_SETVSMEASURED_H_ */
