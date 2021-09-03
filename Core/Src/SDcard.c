/*  SDcard.c
 *  Created on: 17 Dec 2020
 *  Author: David  */
#include "main.h"
#include "stdbool.h"
#include "Board_Config.h"
#include "File_Handling.h"
#include "UART_Routine.h"
#include "SetVsMeasured.h"
#include "LCD.h"

FRESULT SDInit(TfileConfig *file, TCHAR *path) {
	FRESULT res;
	res = Mount_SD(file, path);
	if (res == FR_OK) {
		SDcard.freeSpace = Check_SD_Space(&SDcard);
		res = Create_Dir("/TEST_RESULTS");
	}
	return res;
}

_Bool FindBoardFile(TboardConfig *Board, TfileConfig *FAT) {
	if (Board->Subclass)
		sprintf(&(FAT->FILEname[0]), "%x%c", Board->BoardType, Board->Subclass);
	else
		sprintf(&(FAT->FILEname[0]), "%x", Board->BoardType);
	if (Find_File(FAT, "FIRMWARE")) {
		strcpy(&(FAT->FILEname[0]), "/FIRMWARE/");
		strcat((char*) &(FAT->FILEname[0]), (char*) &(FAT->fileInfo).fname);
		return true;
	} else {
		sprintf((char*) &debugTransmitBuffer[0], "hex FAT not found");
		LCD_displayString((uns_ch*) &debugTransmitBuffer[0], strlen((char*) debugTransmitBuffer));
		printT((uns_ch*) &debugTransmitBuffer[0]);
		return false;
	}
	return false;
}
