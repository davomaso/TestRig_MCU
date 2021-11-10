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
void initialiseTargetBoard(TboardConfig *);
void interrogateTargetBoard(void);
void configureTargetBoard(TboardConfig *);
void uploadSamplesTargetBoard(TboardConfig *);
void TargetBoardParamInit(_Bool);
uint32 ReadSerialNumber(uint8 *, uns_ch *, uint16 );
void setTimeOut(uint16);
void setTerminalTimeOut (uint16);
void runLatchTimeOut(uint16);
uint8 getCurrentVersion(TloomConnected);
void SDIinit(void);
void ASYNCinit(void);
void keypadInit(void);
void DebounceArrayInit(void);

#endif /* INC_INIT_H_ */
