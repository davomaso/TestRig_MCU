/*
 * Init.h
 *
 *  Created on: 2 Sep 2021
 *      Author: mason
 */

#ifndef INC_INIT_H_
#define INC_INIT_H_

#include "Board_Config.h"

void TestRig_Init(void);
void TargetBoardParamInit(_Bool);
uint32 ReadSerialNumber(uint8 *, uns_ch *, uint16 );
void setTimeOut(uint16);
void setTerminalTimeOut (uint16);
void runLatchTimeOut(uint16);
uint8 getCurrentVersion(TloomConnected);
void SDIinit(void);
void ASYNCinit(void);
void currentTestInit(void);
void keypadInit(void);
void DebounceArrayInit(void);
void PrintTestRigFirmwareVersion(void);

#endif /* INC_INIT_H_ */
