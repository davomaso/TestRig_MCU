/*  SDcard.c
 *  Created on: 17 Dec 2020
 *  Author: David  */
#include "SDcard.h"
#include "File_Handling.h"

void WriteSDresults(TtestType *Code, float *Set, float *Measured) {
	PortCount = BoardConnected.latchPortCount + BoardConnected.analogInputCount + BoardConnected.digitalInputCout;
	Open_Afile(&FILEname[0]);
	for (int i = 0; i < PortCount;i++) {
		if (i < BoardConnected.latchPortCount) {
			sprintf(debugTransmitBuffer, "%d,%d,L%d,%d,%.3f,%.3f\n", BoardConnected.BoardType,BoardConnected.GlobalTestNum, (i+1) ,*Code++,*Set++,*Measured++);
			Update_File(&FILEname[0], &debugTransmitBuffer[0]);
		} else {
			sprintf(debugTransmitBuffer, "%d,%d,%d,%d,%.3f,%.3f\n", BoardConnected.BoardType,BoardConnected.GlobalTestNum, ((i+1)-BoardConnected.latchPortCount) ,*Code++,*Set++,*Measured++);
			Update_File(&FILEname[0], &debugTransmitBuffer[0]);
		}
		HAL_Delay(50);
	}
	Close_File(&FILEname[0]);
}

void SDfileInit() {
	Mount_SD("/");
	Check_SD_Space();
//	Create_Dir("TEST_RESULTS");
}

void FindBoardFile(TboardConfig *Board, char * fileLocation) {	//TODO: Change to _Bool
	   	char *buf = malloc(30*sizeof(char));
	   	if(Board->Subclass)
	   		sprintf(buf,"%x%c", Board->BoardType, Board->Subclass);
	   	else sprintf(buf, '%x', Board->BoardType);

		if (Find_File("/FIRMWARE", buf)) {
				if (findVer(&fno.fname[0]) > Board->Version) {
					char * tempDir;
					tempDir = "/FIRMWARE/";
//					fileLocation = malloc(1+strlen(&fno.fname[0])+strlen(tempDir));
					strcpy(fileLocation, tempDir);
					strcat(fileLocation, fno.fname);
					return true;
				} else {
					//TODO: Toggle user if board should be reprogrammed
					sprintf(debugTransmitBuffer, "    Correct Version");
					LCD_printf(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
					printT(debugTransmitBuffer[0]);
					}
			} else {
			sprintf(debugTransmitBuffer, "hex file not found");
			LCD_printf(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
			printT(debugTransmitBuffer[0]);
			return false;
			}
		return false;
}
