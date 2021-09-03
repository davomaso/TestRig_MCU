/*
 * File_Handling_RTOS.h
 *
 *  Created on: 14-May-2020
 *      Author: Controllerstech
 */

#ifndef FILE_HANDLING_RTOS_H_
#define FILE_HANDLING_RTOS_H_

#include <main.h>
#include <ff.h>
#include "Board_Config.h"
#include "fatfs.h"
#include "string.h"
#include "stdio.h"

typedef struct{
	char FILEname[MAX_FILE_NAME_LENGTH];
	uint32 freeSpace;
	FATFS fatfs;
	DIR directory;
	FIL file;
	FILINFO fileInfo;
	FRESULT fresult;  // result
	UINT br, bw;
}TfileConfig;

TfileConfig SDcard;

FRESULT Mount_SD (TfileConfig *,const TCHAR *);
FRESULT Unmount_SD (TfileConfig *, const TCHAR *);
FRESULT Scan_SD(char *);
FRESULT Open_Dir(TfileConfig *, char *);
FRESULT Close_Dir(TfileConfig *);
_Bool Find_File(TfileConfig *, char *);
FRESULT Write_File(TfileConfig *, TCHAR *, char *);
FRESULT Read_File(TfileConfig *, char *);
FRESULT Create_File(TfileConfig *);
FRESULT Open_AppendFile(TfileConfig *);
FRESULT Close_File(TfileConfig *);
FRESULT Update_File(TfileConfig *, char *, char *);
FRESULT Create_Dir(TCHAR *);
void OpenFile(TfileConfig *);
uint32 Check_SD_Space(TfileConfig *);
FRESULT CreateResultsFile(TfileConfig *, TboardConfig *);
_Bool FetchLine(TfileConfig *, char *);

#endif /* FILE_HANDLING_RTOS_H_ */
