/*
 * SetVsMeasured.h
 *
 *  Created on: 11 Sep 2020
 *      Author: Admin
 */

#ifndef INC_SETVSMEASURED_H_
#define INC_SETVSMEASURED_H_

#include "Board_Config.h"

void CompareResults(TboardConfig *,int*,float*);

float fMeasured;
float comp_max;
float comp_min;

_Bool TestResults[15];
float SetResults[10];
float MeasuredResults[10];
uint8 PortNum;

typedef enum {TTLatch = 'L', TTVoltage = 'V', TTCurrent = 'I', TTAsync = 'A', TTSDI = 'S', TTRS485 = 'R',TTNo = '0'}TtestType;
TtestType PortTypes[10];
typedef enum {TRpassed = 'p', TRfailed = 'f'}Tresult;
Tresult TresultStatus;

#endif /* INC_SETVSMEASURED_H_ */
