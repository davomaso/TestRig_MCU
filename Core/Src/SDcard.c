/*  SDcard.c
 *  Created on: 17 Dec 2020
 *  Author: David  */
#include "SDcard.h"

void WriteSDresults(TtestType *Code, float *Set, float *Measured) {
	PortCount = BoardConnected.latchPortCount + BoardConnected.analogInputCount + BoardConnected.digitalInputCout;
	Open_Afile(&FILEname[0]);
	for (int i = 0; i < PortCount;i++) {
		if (i < BoardConnected.latchPortCount) {
			sprintf(Buffer, "%d,%d,L%d,%d,%.3f,%.3f\n", BoardConnected.BoardType,BoardConnected.GlobalTestNum, (i+1) ,*Code++,*Set++,*Measured++);
			Update_File(&FILEname[0], &Buffer[0]);
		} else {
			sprintf(Buffer, "%d,%d,%d,%d,%.3f,%.3f\n", BoardConnected.BoardType,BoardConnected.GlobalTestNum, ((i+1)-BoardConnected.latchPortCount) ,*Code++,*Set++,*Measured++);
			Update_File(&FILEname[0], &Buffer[0]);
		}
		HAL_Delay(50);
	}
	Close_File(&FILEname[0]);
}

void SDfileInit() {
	Mount_SD("/");
	Check_SD_Space();
	Create_Dir("TEST_RESULTS");
}
