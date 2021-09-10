#include <File_Handling.h>
#include "LCD.h"
#include "stm32f4xx_hal.h"
#include "Programming.h"
#include "UART_Routine.h"

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
		printT((uns_ch*) "ERROR!!! in mounting SD CARD...\n\n");
	else {
		printT((uns_ch*) "SD CARD mounted successfully...\n");
		LCD_printf((uns_ch*) "SD Card:", 3, 0);
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
		printT((uns_ch*) "SD CARD UNMOUNTED successfully...\n\n\n");
	else
		printT((uns_ch*) "ERROR!!! in UNMOUNTING SD CARD\n\n\n");
	return res;
}

FRESULT Scan_SD(char *pat) {
	/*
	 * Routine to scan through the SD card searching for the file passed to the routine.
	 * Similary to the previous routines this will return an FRESULT to determine correct operation
	 */
	UINT i;
	i = 0;
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
				char *buf = malloc(255 * sizeof(char));
				sprintf(buf, "Dir: %s\r\n", SDcard.fileInfo.fname);
				printT((uns_ch*) buf);
				free(buf);
				i = strlen(pat);
				sprintf(pat, "/%s", SDcard.fileInfo.fname);
				SDcard.fresult = Scan_SD(pat); /* Enter the directory */
				if (SDcard.fresult != FR_OK)
					break;
//                pat[i] = 0;
			} else { /* It is a file. */
				char *buf = malloc(255 * sizeof(char));
				sprintf(buf, "File: %s/%s\n", pat, SDcard.fileInfo.fname);
				printT((uns_ch*) buf);
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
		printT((uns_ch*) buf);
		free(buf);
		return res; //return result if error when opening file
	}
	return res;
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
			if (TestRigMode == VerboseMode) {
				printT((uns_ch*) &(FAT->fileInfo.fname[0]));
				printT("\r\n");
			}
			if (res != FR_OK || FAT->fileInfo.fname[0] == 0)
				break; /* Break on error or end of dir */
			if (FAT->fileInfo.fattrib & AM_DIR) { /* Check if it is a directory */
				if (!(strcmp("SYSTEM~1", FAT->fileInfo.fname)))
					continue; // Ensure directory is not SYSTEM~1
				char *buf = malloc(255 * sizeof(char));
				sprintf(buf, "Dir: %s\r\n", FAT->fileInfo.fname);
				printT((uns_ch*) buf);
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
					free(buf);
					HAL_Delay(2);
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
		printT((uns_ch*) buf);
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
		printT((uns_ch*) buf);
		free(buf);
		return res;
	}
	char *buffer = malloc(sizeof(f_size(&(file->file))));
	res = f_read(&(file->file), buffer, f_size(&(file->file)), &(file->br));
	if (res != FR_OK) {
		char *buf = malloc(100 * sizeof(char));
		free(buffer);
		sprintf(buf, "ERROR!!! No. %d in reading file *%s*\n\n", res, name);
		printT((uns_ch*) buf);
		free(buf);
	} else {
		printT((uns_ch*) buffer);
		free(buffer);
		/* Close file */
		res = f_close(&(file->file));
		if (res != FR_OK) {
			char *buf = malloc(100 * sizeof(char));
			sprintf(buf, "ERROR!!! No. %d in closing file *%s*\n\n", res, name);
			printT((uns_ch*) buf);
			free(buf);
		} else if (TestRigMode == VerboseMode) {
			char *buf = malloc(100 * sizeof(char));
			sprintf(buf, "File *%s* CLOSED successfully\n", name);
			printT((uns_ch*) buf);
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
	res = f_open(&(file->file), &(file->FILEname[0]), FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
	if (res != FR_OK) {
		char *buf = malloc(100 * sizeof(char));
		sprintf(buf, "ERROR!!! No. %d in creating file *%s*\n\n", res, &(file->FILEname[0]));
		printT((uns_ch*) buf);
		free(buf);
		return res;
	} else if (TestRigMode == VerboseMode) {
		char *buf = malloc(100 * sizeof(char));
		sprintf(buf, "*%s* created successfully\n Now use Write_File to write data\n", &(file->FILEname[0]));
		printT((uns_ch*) buf);
		free(buf);
	}
	return res;
}

FRESULT Open_AppendFile(TfileConfig *FAT) {
	/*
	 * First the routine opens the file passed to the function. Read and Write permissions are
	 * granted. Open Append allows for the creation of a file if it has not previously existed,
	 * with the read and write pointer pointed to EOF.
	 * As previous FRESULT is returned to inform of any errors
	 */
	FRESULT res;
	res = f_open(&(FAT->file), &(FAT->FILEname[0]), FA_OPEN_APPEND | FA_WRITE | FA_READ);
	if (res != FR_OK) {
		char *buf = malloc(100 * sizeof(char));
		sprintf(buf, "ERROR!!! No. %d in opening file *%s*\n\n", res, (FAT->FILEname));
		printT((uns_ch*) buf);
		free(buf);
	}
	return res;
}

FRESULT Close_File(TfileConfig *FAT) {
	FRESULT res;
	res = f_close(&(FAT->file));
	if (res != FR_OK) {
		char *buf = malloc(100 * sizeof(char));
		sprintf(buf, "ERROR No. %d in closing file *%s*\n\n", res, (FAT->FILEname));
		printT((uns_ch*) buf);
		free(buf);
	} else if (TestRigMode == VerboseMode) {
		char *buf = malloc(100 * sizeof(char));
		sprintf(buf, "File *%s* CLOSED successfully\n\n", (char*) &(FAT->FILEname[0]));
		printT((uns_ch*) buf);
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
		printT((uns_ch*) buf);
		free(buf);
	} else if (TestRigMode == VerboseMode) {
		char *buf = malloc(100 * sizeof(char));
		sprintf(buf, "*%s* UPDATED successfully\n", name);
		printT((uns_ch*) buf);
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
		if (TestRigMode == VerboseMode) {
			char *buf = malloc(100 * sizeof(char));
			sprintf(buf, "*%s* has been created successfully\n", name);
			printT((uns_ch*) buf);
			free(buf);
		}
	} else {
		char *buf = malloc(100 * sizeof(char));
		sprintf(buf, "ERROR No. %d in creating directory *%s*\n\n", res, name);
		printT((uns_ch*) buf);
		free(buf);
	}
	return res;
}
FRESULT OpenFile(TfileConfig *FAT) {
	/*
	 * Routine to open file
	 * Failure if the file does not exist
	 * Process only allows for read only permissions
	 * If the file is found the size of the file is displayed on the terminal
	 */
	FRESULT res;
	res = f_open(&(FAT->file), &(FAT->FILEname[0]), FA_OPEN_EXISTING | FA_READ);
	if (res != FR_OK) {
		sprintf((char*) &debugTransmitBuffer[0], "Error %d attempting to open %s", res, (char*) &(FAT->FILEname));
		printT(&debugTransmitBuffer[0]);
	} else {
		fileSize = (FAT->file).obj.objsize;
		if (TestRigMode == VerboseMode) {
			sprintf((char*) &debugTransmitBuffer[0], "File Size: ");
			printT(&debugTransmitBuffer[0]);
			sprintf((char*) &debugTransmitBuffer[0], "%.03f kB\n", (float) fileSize / 1000);
			printT(&debugTransmitBuffer[0]);
			sprintf((char*) &debugTransmitBuffer[0], "***  %s Opened Successfully  ***\n", (char*) &(FAT->FILEname));
			printT(&debugTransmitBuffer[0]);
		}
	}
	return res;
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
	printT((uns_ch*) buf);
	free_space = (uint32) (fre_clust * pfs->csize * 0.5);
	sprintf(buf, "SD CARD Free Space: \t%lu\n", free_space);
	printT((uns_ch*) buf);
	free(buf);
	free_space /= 1000;
	LCD_setCursor(4, 0);
	sprintf((char*) &debugTransmitBuffer[0], "%lu MB Free", free_space);
	LCD_displayString((uint8*) &debugTransmitBuffer[0], strlen((char*) &debugTransmitBuffer[0]));
	return free_space;
}

FRESULT CreateResultsFile(TfileConfig *FAT, TboardConfig *Board) {

	FRESULT res;
	sprintf(&(FAT->FILEname[0]), "/TEST_RESULTS/%lu_%x", Board->SerialNumber, Board->BoardType);
	res = Create_Dir(&(FAT->FILEname));
	sprintf(&(FAT->FILEname[0]), "/TEST_RESULTS/%lu_%x/%lu.CSV", Board->SerialNumber, Board->BoardType,
				Board->SerialNumber);
	res = Create_File(FAT);
	if (res == FR_OK) {
		sprintf((char*) &debugTransmitBuffer[0],
				"Board,Test,Port,TestType,Pass/Fail,Set,Measured, ton, VonH, VonL, MOSonH, MOSonL, toff, VoffH, VoffL, MOSoffHigh, MOSoffLow, VinAVG, VinLow, VfuseAVG, VfuseLow \r\n");
		res = Write_File(FAT, (TCHAR*) &(FAT->FILEname), (char*) &debugTransmitBuffer[0]);
	}
	return res;
}

FRESULT WriteVoltages(TboardConfig *Board, TfileConfig *FAT) {
	FRESULT res;
	sprintf(FAT->FILEname, "/TEST_RESULTS/%lu_%x/%lu.CSV", Board->SerialNumber, Board->BoardType, Board->SerialNumber);
	res = Open_AppendFile(FAT);
	if (res == FR_OK) {
		if ((Board->BoardType == b427x) || (Board->BoardType == b422x))
			sprintf((char*) &TestResultsBuffer,
					"Voltages\nSolar, %.3f\n Input, %.3f\n12V Sample, %.3f\n3V Sample, %.3f\n",
					Board->VoltageBuffer[V_SOLAR], Board->VoltageBuffer[V_INPUT], Board->VoltageBuffer[V_12],
					Board->VoltageBuffer[V_3]);
		else
			sprintf((char*) &TestResultsBuffer, "Voltages\nInput, %.3f\n12V Sample, %.3f\n3V Sample, %.3f\n",
					Board->VoltageBuffer[V_INPUT], Board->VoltageBuffer[V_12], Board->VoltageBuffer[V_3]);

		res = Update_File(FAT, (char*) FAT->FILEname[0], &TestResultsBuffer[0]);
		if (res == FR_OK)
			res = Close_File(FAT);
	}
	return res;
}

