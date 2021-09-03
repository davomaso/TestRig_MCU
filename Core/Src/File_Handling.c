#include <File_Handling.h>
#include "LCD.h"
#include "stm32f4xx_hal.h"
#include "Programming.h"

/* =============================>>>>>>>> NO CHANGES AFTER THIS LINE =====================================>>>>>>> */
FRESULT Mount_SD(TfileConfig *file, const TCHAR *path) {
	/*
	 * Routine to mount the SD card, returning FRESULT, if return is 0 then SD mounted correctly
	 * see ff.h for further explanation of FATFS FRESULT responses.
	 * An explanation of each of these responses can be found at http://elm-chan.org/fsw/ff/00index_e.html
	 */
	FRESULT res;
	res = f_mount(&(file->fatfs), path, 1);
	if (res != FR_OK)
		printT("ERROR!!! in mounting SD CARD...\n\n");
	else {
		printT("SD CARD mounted successfully...\n");
		LCD_printf("SD Card:", 3, 0);
	}
	return res;
}

FRESULT Unmount_SD(TfileConfig *file, const TCHAR *path) {
	/*
	 * Following any operation of reading/writing to the SD card the UNMOUNT function is required to disconnect the SD card
	 * This function wil l act similarly to the previous routine return an FRESULT determining whether the unmounting was successful.
	 */
	FRESULT res;
	res = f_mount(0, "", 1);
	if (res == FR_OK)
		printT("SD CARD UNMOUNTED successfully...\n\n\n");
	else
		printT("ERROR!!! in UNMOUNTING SD CARD\n\n\n");
}

FRESULT Scan_SD(char *pat) {
	/*
	 * Routine to scan through the SD card searching for the file passed to the routine.
	 * Similary to the previous routines this will return an FRESULT to determine correct operation
	 */
	UINT i;
	SDcard.fresult = f_opendir(&SDcard.directory, pat);
	if (SDcard.fresult == FR_OK) {
		for (;;) {
			SDcard.fresult = f_readdir(&SDcard.directory, &SDcard.fileInfo); /* Read a directory item */
			if (SDcard.fresult != FR_OK || SDcard.fileInfo.fname[0] == 0)
				break; /* Break on error or end of dir */
			if (SDcard.fileInfo.fattrib & AM_DIR) /* It is a directory */
			{
				if (!(strcmp("SYSTEM~1", SDcard.fileInfo.fname)))
					continue;
				char *buf = malloc(30 * sizeof(char));
				sprintf(buf, "Dir: %s\r\n", SDcard.fileInfo.fname);
				printT(buf);
				free(buf);
				i = strlen(pat);
				sprintf(pat, "/%s", SDcard.fileInfo.fname);
				SDcard.fresult = Scan_SD(pat); /* Enter the directory */
				if (SDcard.fresult != FR_OK)
					break;
//                pat[i] = 0;
			} else { /* It is a file. */
				char *buf = malloc(30 * sizeof(char));
				sprintf(buf, "File: %s/%s\n", pat, SDcard.fileInfo.fname);
				printT(buf);
				free(buf);
			}
		}
		f_closedir(&SDcard.directory);
	}
	free(pat);
	return SDcard.fresult;
}

FRESULT Open_Dir(TfileConfig *file, char *path) {	//TODO: Change this
	FRESULT res;
	res = f_opendir(&(file->directory), path);
	return res;
}

FRESULT Close_Dir(TfileConfig *file) {
	FRESULT res;
	res = f_closedir(&(file->directory));
	if (res != FR_OK) {
		char *buf = malloc(100 * sizeof(char));
		sprintf(buf, "ERROR!!! No. %d in closing directory *\n\n", res);
		printT(buf);
		free(buf);
		return res; //return result if error when opening file
	}
}
_Bool Find_File(TfileConfig *FAT, char *path) {
	/*
	 * As the previous routine scans for a specified file, this routine scans the SD card for a specific directory and file,
	 * this routine is typically for searching for the .hex board files as a string comparison is made to determine when the
	 * board file is found following the first 4 characters/ (the board type)
	 */
	uint8 i;
	FRESULT res;
	res = f_opendir(&FAT->directory, path);
	if (res == FR_OK) {
		for (;;) {
			res = f_readdir(&(FAT->directory), &(FAT->fileInfo)); /* Read a directory item */
			if (res != FR_OK || FAT->fileInfo.fname[0] == 0)
				break; /* Break on error or end of dir */
			if (FAT->fileInfo.fattrib & AM_DIR) { /* Check if it is a directory */
				if (!(strcmp("SYSTEM~1", FAT->fileInfo.fname)))
					continue; // Ensure directory is not SYSTEM~1
				char *buf = malloc(30 * sizeof(char));
				sprintf(buf, "Dir: %s\r\n", FAT->fileInfo.fname);
				printT(buf);
				free(buf);
				i = strlen(path);
				sprintf(&path[i], "/%s", FAT->fileInfo.fname);
				res = Scan_SD(path); /* Enter the directory */
				if (res != FR_OK)
					break;
				path[i] = 0;
			} else { /* If it is not a directory it is a file. */
				if (memcmp(&(FAT->fileInfo.fname), &(FAT->FILEname[0]), 4) == 0) {
					char *buf = malloc((_MAX_LFN + 1) * sizeof(char));
					sprintf(buf, "File: %s/%s\n", path, FAT->fileInfo.fname);
					printT(buf);
					free(buf);
					res = f_closedir(&FAT->directory);
					return (res == FR_OK);
				}
			}
		}
	}
	res = f_closedir(&FAT->directory);
	return 0;	// Function Failed!
}

FRESULT Write_File(TfileConfig *file, TCHAR *name, char *data) {
	/*
	 * Routine checks whether file exists, if it does the
	 */
	/**** check whether the file exists or not ****/
	/* Create a file with read write access and open it */
	//Check if FA_OPEN_EXISTING is required below
	FRESULT res;
	res = f_write(&(file->file), data, strlen(data), &(file->bw));
	if (res != FR_OK) {
		char *buf = malloc(100 * sizeof(char));
		sprintf(buf, "ERROR!!! No. %d in opening file *%s*\n\n", res, name);
		printT(buf);
		free(buf);
	}
	return res;
}

FRESULT Read_File(TfileConfig *file, char *name) {
	/*
	 * Routine checks whether file exists, if true. The file is opened with read only permissions.
	 * The routine returns the contents of the file.
	 */

	/**** check whether the file exists or not ****/
	FRESULT res;
	/* Open file to read */
	res = f_open(&(file->file), name, FA_OPEN_EXISTING | FA_READ);
	if (res != FR_OK) {
		char *buf = malloc(100 * sizeof(char));
		sprintf(buf, "ERROR!!! No. %d in opening file *%s*\n\n", res, name);
		printT(buf);
		free(buf);
		return res;
	}
	char *buffer = malloc(sizeof(f_size(&(file->file))));
	res = f_read(&(file->file), buffer, f_size(&(file->file)), &(file->br));
	if (res != FR_OK) {
		char *buf = malloc(100 * sizeof(char));
		free(buffer);
		sprintf(buf, "ERROR!!! No. %d in reading file *%s*\n\n", res, name);
		printT(buf);
		free(buf);
	} else {
		printT(buffer);
		free(buffer);
		/* Close file */
		res = f_close(&(file->file));
		if (res != FR_OK) {
			char *buf = malloc(100 * sizeof(char));
			sprintf(buf, "ERROR!!! No. %d in closing file *%s*\n\n", res, name);
			printT(buf);
			free(buf);
		} else {
			char *buf = malloc(100 * sizeof(char));
			sprintf(buf, "File *%s* CLOSED successfully\n", name);
			printT(buf);
			free(buf);
		}
	}
	return res;
}

FRESULT Create_File(TfileConfig *file) {
	/*
	 * f_stat used to determine whether file exists, if not f_open is used to create the file with
	 * read and write privileges. FRESULT returned to determine if successful.
	 */
	FRESULT res;
	res = f_open(&(file->file), &(file->FILEname[0]), FA_CREATE_ALWAYS | FA_READ | FA_WRITE);
	if (res != FR_OK) {
		char *buf = malloc(100 * sizeof(char));
		sprintf(buf, "ERROR!!! No. %d in creating file *%s*\n\n", res, &(file->FILEname[0]));
		printT(buf);
		free(buf);
		return res;
	} else {
		char *buf = malloc(100 * sizeof(char));
		sprintf(buf, "*%s* created successfully\n Now use Write_File to write data\n", &(file->FILEname[0]));
		printT(buf);
		free(buf);
	}
	return res;
}

FRESULT Open_AppendFile(TfileConfig *file) {
	/*
	 * First the routine opens the file passed to the function. Read and Write permissions are
	 * granted. Open Append allows for the creation of a file if it has not previously existed,
	 * with the read and write pointer pointed to EOF.
	 * As previous FRESULT is returned to inform of any errors
	 */
	FRESULT res;
	res = f_open(&(file->file), &(file->FILEname[0]), FA_OPEN_APPEND | FA_WRITE | FA_READ);
	if (res != FR_OK) {
		char *buf = malloc(100 * sizeof(char));
		sprintf(buf, "ERROR!!! No. %d in opening file *%s*\n\n", res, (file->FILEname));
		printT(buf);
		free(buf);
	}
	return res;
}

FRESULT Close_File(TfileConfig *file) {
	FRESULT res;
	res = f_close(&(file->file));
	if (res != FR_OK) {
		char *buf = malloc(100 * sizeof(char));
		sprintf(buf, "ERROR No. %d in closing file *%s*\n\n", res, (file->FILEname));
		printT(buf);
		free(buf);
	} else {
		char *buf = malloc(100 * sizeof(char));
		sprintf(buf, "File *%s* CLOSED successfully\n\n", (file->FILEname[0]));
		printT(buf);
		free(buf);
	}
	return res;
}

FRESULT Update_File(TfileConfig *file, char *name, char *data) {
	/**** check whether the file exists or not ****/
	FRESULT res;
	res = f_write(&(file->file), data, strlen(data), &(file->bw));
	if (res != FR_OK) {
		char *buf = malloc(100 * sizeof(char));
		sprintf(buf, "ERROR!!! No. %d in writing file *%s*\n\n", res, name);
		printT(buf);
		free(buf);
	} else {
		char *buf = malloc(100 * sizeof(char));
		sprintf(buf, "*%s* UPDATED successfully\n", name);
		printT(buf);
		free(buf);
	}
	return res;
}

FRESULT Create_Dir(TCHAR *name) {
	/*
	 * Creates directory with the name passed to the routine. As the other FATFS routines above
	 * if a process fails FRESULT returns a value regarding what the failure was
	 */
	FRESULT res;
	res = f_mkdir(name);
	if (res == FR_OK) {
		char *buf = malloc(100 * sizeof(char));
		sprintf(buf, "*%s* has been created successfully\n", name);
		printT(buf);
		free(buf);
	} else {
		char *buf = malloc(100 * sizeof(char));
		sprintf(buf, "ERROR No. %d in creating directory *%s*\n\n", res, name);
		printT(buf);
		free(buf);
	}
	return res;
}
void OpenFile(TfileConfig *file) {
	/*
	 * Routine to open file
	 * Failure if the file does not exist
	 * Process only allows for read only permissions
	 * If the file is found the size of the file is displayed on the terminal
	 */
	FRESULT res;
	uint8 Retry = 0;
	res = f_open(&(file->file), &(file->FILEname[0]), FA_OPEN_EXISTING | FA_READ);
	HAL_Delay(100);
	while (Retry < 5) {
		if (res != FR_OK) {
			res = f_close(&(file->file));
			HAL_Delay(100);
			res = f_open(&(file->file), &(file->FILEname), FA_OPEN_EXISTING | FA_READ);
			HAL_Delay(100);
			Retry++;
		} else
			break;
	}
	if (res != FR_OK) {
		sprintf(&debugTransmitBuffer[0], "Error %d attempting to open file", res);
		printT(&debugTransmitBuffer[0]);
	} else {
		fileSize = (file->file).obj.objsize;
		sprintf((char*)&debugTransmitBuffer[0], "File Size: ");
		printT(&debugTransmitBuffer[0]);
		sprintf((char*)&debugTransmitBuffer[0], "%.03f kB\n", (float) fileSize / 1000);
		printT(&debugTransmitBuffer[0]);
		sprintf((char*) &debugTransmitBuffer[0], "***  %s Opened Successfully  ***\n", (char *) &(file->FILEname) );
		printT(&debugTransmitBuffer[0]);
	}
}

uint32 Check_SD_Space(TfileConfig *file) {
	/*
	 * Routine required to print the FreeSpace left on the SD card to both the terminal and LCD
	 * Routine finds the number of FAT entries and the number of remaining clusters. Displaying the
	 * total space of the SD card on the terminal, and the free space to both the terminal and LCD.
	 *
	 */
	/**** capacity related *****/
	FATFS *pfs;
	DWORD fre_clust;
	uint32 total, free_space;
	/* Check free space */

	f_getfree("", &fre_clust, &pfs);

	total = (uint32) ((pfs->n_fatent - 2) * pfs->csize * 0.5);
	char *buf = malloc(255 * sizeof(char));
	sprintf(buf, "SD CARD Total Size: \t%lu\n", total);
	printT(buf);
	free_space = (uint32) (fre_clust * pfs->csize * 0.5);
	sprintf(buf, "SD CARD Free Space: \t%lu\n", free_space);
	printT(buf);
	free(buf);
	free_space /= 1000;
	LCD_setCursor(4, 0);
	sprintf((char*) &debugTransmitBuffer[0], "%lu MB Free", free_space);
	LCD_displayString((char*) &debugTransmitBuffer[0], strlen((char*) &debugTransmitBuffer[0]));
	return free_space;
}

FRESULT CreateResultsFile(TfileConfig *file, TboardConfig *Board) {
	sprintf(&(file->FILEname[0]), "/TEST_RESULTS/%lu.CSV", Board->SerialNumber);
	HAL_Delay(100);
	FRESULT res;
	res = Create_File(file);
	if (res == 0) {
		sprintf((char *)&debugTransmitBuffer[0],
				"Board,Test,Port,TestType,Pass/Fail,Set,Measured, ton, toff, V1h, V2l, V2h, V1l, VinAVG, VinLow, VfuseAVG, VfuseLow, MOSonHigh, MOSonLow, MOSoffHigh, MOSoffLow\r\n");
		HAL_Delay(100);
		res = Write_File(&SDcard, &SDcard.FILEname, &debugTransmitBuffer[0]);
	}
	return res;
}

_Bool FetchLine(TfileConfig *file, char *TempLine) {
	f_gets(TempLine, MAX_LINE_LENGTH, &(file->file));
	if (strlen(TempLine))
		return 1;
	else
		return 0;
}
