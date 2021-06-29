#include <File_Handling.h>
#include <ff.h>
#include "stm32f4xx_hal.h"
#include "Programming.h"


/* =============================>>>>>>>> NO CHANGES AFTER THIS LINE =====================================>>>>>>> */
void Mount_SD (const TCHAR* path) {
	/*
	 * Routine to mount the SD card, returning FRESULT, if return is 0 then SD mounted correctly
	 * see ff.h for further explanation of FATFS FRESULT responses.
	 * An explanation of each of these responses can be found at http://elm-chan.org/fsw/ff/00index_e.html
	 */
	SDcard.fresult = f_mount(&SDcard.fatfs, path, 1);
	if (SDcard.fresult != FR_OK) {
		 printT("ERROR!!! in mounting SD CARD...\n\n");
//		 if (fresult == FR_DISK_ERR)
//			 Unmount_SD("/");
	} else {
		printT("SD CARD mounted successfully...\n");
		LCD_setCursor(2, 0);
		sprintf(debugTransmitBuffer, " SD card Connected  ");
		LCD_printf(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
	}
}

void Unmount_SD (const TCHAR* path) {
	/*
	 * Following any operation of reading/writing to the SD card the UNMOUNT function is required to disconnect the SD card
	 * This function wil l act similarly to the previous routine return an FRESULT determining whether the unmounting was successful.
	 */
	SDcard.fresult = f_mount(0, "", 1);
	if (SDcard.fresult == FR_OK) printT ("SD CARD UNMOUNTED successfully...\n\n\n");
	else printT("ERROR!!! in UNMOUNTING SD CARD\n\n\n");
}

FRESULT Scan_SD (char* pat) {
	/*
	 * Routine to scan through the SD card searching for the file passed to the routine.
	 * Similary to the previous routines this will return an FRESULT to determine correct operation
	 */
//    DIR dir;
    UINT i;
//    char *path = malloc(20*sizeof (char));
//    sprintf (path, "%s",pat);
    SDcard.fresult = f_opendir(&SDcard.directory, pat);
    if (SDcard.fresult == FR_OK)
    {
        for (;;)
        {
        	SDcard.fresult = f_readdir(&SDcard.directory, &SDcard.fileInfo);                   /* Read a directory item */
            if (SDcard.fresult != FR_OK || SDcard.fileInfo.fname[0] == 0) break;  /* Break on error or end of dir */
            if (SDcard.fileInfo.fattrib & AM_DIR)     /* It is a directory */
            {
            	if (!(strcmp ("SYSTEM~1", SDcard.fileInfo.fname))) continue;
            	char *buf = malloc(30*sizeof(char));
            	sprintf (buf, "Dir: %s\r\n", SDcard.fileInfo.fname);
            	printT(buf);
            	free(buf);
                i = strlen(pat);
                sprintf(pat, "/%s", SDcard.fileInfo.fname);
                SDcard.fresult = Scan_SD(pat);                     /* Enter the directory */
                if (SDcard.fresult != FR_OK) break;
//                pat[i] = 0;
            }
            else
            {   /* It is a file. */
           	   char *buf = malloc(30*sizeof(char));
               sprintf(buf,"File: %s/%s\n", pat, SDcard.fileInfo.fname);
               printT(buf);
               free(buf);
            }
        }
        f_closedir(&SDcard.directory);
    }
    free(pat);
    return SDcard.fresult;
}

_Bool Open_Dir(TfileConfig * FAT, char * path) {
   	FAT->fresult = f_opendir(&FAT->directory, path);
   	f_chdir("/FIRMWARE");
}

void CloseDirectory() {
	SDcard.fresult = f_closedir(&SDcard.directory);
    if (SDcard.fresult != FR_OK) {
    	char *buf = malloc(100*sizeof(char));
    	sprintf (buf, "ERROR!!! No. %d in closing directory *\n\n", SDcard.fresult);
    	printT(buf);
        free(buf);
        return SDcard.fresult; //return result if error when opening file
    }
}
_Bool Find_File (TfileConfig * FAT, char* path, char* file) {
	/*
	 * As the previous routine scans for a specified file, this routine scans the SD card for a specific directory and file,
	 * this routine is typically for searching for the .hex board files as a string comparison is made to determine when the
	 * board file is found following the first 4 characters/ (the board type)
	 */
    uint8 i;
//    if (FAT->directory.dir != path)
    FAT->fresult = f_opendir(&FAT->directory, path);

    if (FAT->fresult == FR_OK)
    {
        for (;;)
        {
        	FAT->fresult = f_readdir(&FAT->directory, &FAT->fileInfo);                   /* Read a directory item */
            if (FAT->fresult != FR_OK || FAT->fileInfo.fname[0] == 0) break;  /* Break on error or end of dir */
            if (FAT->fileInfo.fattrib & AM_DIR) {   /* Check if it is a directory */
            	if (!(strcmp ("SYSTEM~1", FAT->fileInfo.fname))) continue; // Ensure directory is not SYSTEM~1
            	char *buf = malloc(30*sizeof(char));
            	sprintf (buf, "Dir: %s\r\n", FAT->fileInfo.fname);
            	printT(buf);
            	free(buf);
                i = strlen(path);
                sprintf(&path[i], "/%s", FAT->fileInfo.fname);
                FAT->fresult = Scan_SD(path);                     /* Enter the directory */
                if (FAT->fresult != FR_OK) break;
                path[i] = 0;
            } else {   /* If it is not a directory it is a file. */
               if (*file++ == FAT->fileInfo.fname[0]) {					//Determine first character of file and compare to filename passed to function
                   if (*file++ == FAT->fileInfo.fname[1]) {				//Determine second character of file and compare to filename passed to function
                       if (*file++ == FAT->fileInfo.fname[2]) {			//Determine third character of file and compare to filename passed to function
                           if (*file++ == FAT->fileInfo.fname[3]) {		//Determine fourth character of file and compare to filename passed to function
                           	   char *buf = malloc(30*sizeof(char));
                               sprintf(buf,"File: %s/%s\n", path, FAT->fileInfo.fname);
                               printT(buf);
                               free(buf);
                               return 1;
                           }
                           file--;
                       	  }
                         file--;
                       }
                    file--;
                   }
                file--;
            }
        }
        f_closedir(&FAT->directory);
    }
    free(path);
    return 0;
}

FRESULT Write_File ( char *name, char *data) {
	/*
	 * Routine checks whether file exists, if it does the
	 */
	/**** check whether the file exists or not ****/
	    /* Create a file with read write access and open it */
		//Check if FA_OPEN_EXISTING is required below
	SDcard.fresult = f_open(&SDcard.file, name, FA_CREATE_ALWAYS | FA_WRITE );	//Open file with write permissions, create the file even if existing so data is overwritten
	    if (SDcard.fresult != FR_OK) {
	    	char *buf = malloc(100*sizeof(char));
	    	sprintf (buf, "ERROR!!! No. %d in opening file *%s*\n\n", SDcard.fresult, name);
	    	printT(buf);
	        free(buf);
	        return SDcard.fresult; //return result if error when opening file
	    } else {
	    	SDcard.fresult = f_write(&SDcard.file, data, strlen(data), &SDcard.bw);
	    	if (SDcard.fresult != FR_OK) {
	    		char *buf = malloc(100*sizeof(char));
	    		sprintf (buf, "ERROR!!! No. %d while writing to the FILE *%s*\n\n", SDcard.fresult, name);
	    		printT(buf);
	    		free(buf);
	    		return SDcard.fresult; // if error when writing to file return the error
	    	}
	    }
	    Close_File(name);	//Following writing to the file close the file and return the result
	    return SDcard.fresult;
	//}
}

FRESULT Read_File (char *name)
{
	/*
	 * Routine checks whether file exists, if true. The file is opened with read only permissions.
	 * The routine returns the contents of the file.
	 */

	/**** check whether the file exists or not ****/
	SDcard.fresult = f_stat (name, &SDcard.fileInfo);
	if (SDcard.fresult != FR_OK) {
		char *buf = malloc(100*sizeof(char));
		sprintf (buf, "ERRROR!!! *%s* does not exists\n\n", name);
		printT (buf);
		free(buf);
	    return SDcard.fresult;
	} else {
		/* Open file to read */
		SDcard.fresult = f_open(&SDcard.file, name, FA_READ);
		if (SDcard.fresult != FR_OK) {
			char *buf = malloc(100*sizeof(char));
			sprintf (buf, "ERROR!!! No. %d in opening file *%s*\n\n", SDcard.fresult, name);
			printT(buf);
		    free(buf);
		    return SDcard.fresult;
		}
		char *buffer = malloc(sizeof(f_size(&SDcard.file)));
		SDcard.fresult = f_read (&SDcard.file, buffer, f_size(&SDcard.file), &SDcard.br);
		if (SDcard.fresult != FR_OK) {
			char *buf = malloc(100*sizeof(char));
			free(buffer);
		 	sprintf (buf, "ERROR!!! No. %d in reading file *%s*\n\n", SDcard.fresult, name);
		 	printT(buf);
		  	free(buf);
		} else {
			printT(buffer);
			free(buffer);
			/* Close file */
			SDcard.fresult = f_close(&SDcard.file);
			if (SDcard.fresult != FR_OK) {
				char *buf = malloc(100*sizeof(char));
				sprintf (buf, "ERROR!!! No. %d in closing file *%s*\n\n", SDcard.fresult, name);
				printT(buf);
				free(buf);
			} else {
				char *buf = malloc(100*sizeof(char));
				sprintf (buf, "File *%s* CLOSED successfully\n", name);
				printT(buf);
				free(buf);
			}
		}
	    return SDcard.fresult;
	}
}

FRESULT Create_File (char *name)
{
	/*
	 * f_stat used to determine whether file exists, if not f_open is used to create the file with
	 * read and write privileges. FRESULT returned to determine if successful.
	 */
	SDcard.fresult = f_stat (name, &SDcard.fileInfo);
	if (SDcard.fresult == FR_OK) {
		char *buf = malloc(100*sizeof(char));
		sprintf (buf, "ERROR!!! *%s* already exists!!!!\n use Update_File \n\n",name);
		printT(buf);
		free(buf);
	    return SDcard.fresult;
	} else {
		SDcard.fresult = f_open(&SDcard.file, name, FA_CREATE_ALWAYS|FA_READ|FA_WRITE);
		if (SDcard.fresult != FR_OK) {
			char *buf = malloc(100*sizeof(char));
			sprintf (buf, "ERROR!!! No. %d in creating file *%s*\n\n", SDcard.fresult, name);
			printT(buf);
			free(buf);
		    return SDcard.fresult;
		} else {
			char *buf = malloc(100*sizeof(char));
			sprintf (buf, "*%s* created successfully\n Now use Write_File to write data\n",name);
			printT(buf);
			free(buf);
		}
	}
	Close_File(name);
    return SDcard.fresult;
}

FRESULT Open_AppendFile(char * name) {
	/*
	 * First the routine opens the file passed to the function. Read and Write permissions are
	 * granted. Open Append allows for the creation of a file if it has not previously existed,
	 * with the read and write pointer pointed to EOF.
	 * As previous FRESULT is returned to inform of any errors
	 */
	SDcard.fresult = f_open(&SDcard.file, name, FA_OPEN_APPEND | FA_WRITE | FA_READ);
    if (SDcard.fresult != FR_OK) {
    	char *buf = malloc(100*sizeof(char));
    	sprintf (buf, "ERROR!!! No. %d in opening file *%s*\n\n", SDcard.fresult, name);
    	printT(buf);
        free(buf);
    }
    return SDcard.fresult;
}

void Close_File(char * name) {
	SDcard.fresult = f_close(&SDcard.file);
	if (SDcard.fresult != FR_OK) {
		char *buf = malloc(100*sizeof(char));
		sprintf (buf, "ERROR No. %d in closing file *%s*\n\n", SDcard.fresult, name);
		printT(buf);
		free(buf);
		if (SDcard.fresult == 1 || SDcard.fresult == 9 || SDcard.fresult == 16)
			Close_File(name);
	} else {
		char *buf = malloc(100*sizeof(char));
		sprintf (buf, "File *%s* CLOSED successfully\n", name);
		printT(buf);
		free(buf);
	}
}



FRESULT Update_File (char *name, char *data)
{
	/**** check whether the file exists or not ****/
	SDcard.fresult = f_stat (name, &SDcard.fileInfo);
	if (SDcard.fresult != FR_OK) {
		char *buf = malloc(100*sizeof(char));
		sprintf (buf, "ERROR!!! *%s* does not exists\n\n", name);
		printT (buf);
		free(buf);
	    return SDcard.fresult;
	} else {
		 /* Create a file with read write access and open it */
	    /* Writing text */
		SDcard.fresult = f_write(&SDcard.file, data, strlen (data), &SDcard.bw);
	    if (SDcard.fresult != FR_OK) {
	    	char *buf = malloc(100*sizeof(char));
	    	sprintf (buf, "ERROR!!! No. %d in writing file *%s*\n\n", SDcard.fresult, name);
	    	printT(buf);
	    	free(buf);
	    } else {
	    	char *buf = malloc(100*sizeof(char));
	    	sprintf (buf, "*%s* UPDATED successfully\n", name);
	    	printT(buf);
	    	free(buf);
	    }
	}
//	delay_us(50);
	Close_File(name);
    return SDcard.fresult;
}

FRESULT Create_Dir (char *name) {
	/*
	 * Creates directory with the name passed to the routine. As the other FATFS routines above
	 * if a process fails FRESULT returns a value regarding what the failure was
	 */
	SDcard.fresult = f_mkdir(name);
    if (SDcard.fresult == FR_OK) {
    	char *buf = malloc(100*sizeof(char));
    	sprintf (buf, "*%s* has been created successfully\n", name);
    	printT (buf);
    	free(buf);
    } else {
    	char *buf = malloc(100*sizeof(char));
    	sprintf (buf, "ERROR No. %d in creating directory *%s*\n\n", SDcard.fresult,name);
    	printT(buf);
    	free(buf);
    }
    return SDcard.fresult;
}
void OpenFile(char * fileName) {
	/*
	 * Routine to open file
	 * Failure if the file does not exist
	 * Process only allows for read only permissions
	 * If the file is found the size of the file is displayed on the terminal
	 */
	f_open(&SDcard.file, fileName, FA_READ | FA_OPEN_EXISTING);
	fileSize = f_size(&SDcard.file);
	sprintf(debugTransmitBuffer, "File Size: ");
	printT(&debugTransmitBuffer[0]);
	sprintf(debugTransmitBuffer,"%.03f kB", (float)fileSize/1000);
	printT(&debugTransmitBuffer[0]);
}

void Check_SD_Space ()
{
	/*
	 * Routine required to print the FreeSpace left on the SD card to both the terminal and LCD
	 * Routine finds the number of FAT entries and the number of remaining clusters. Displaying the
	 * total space of the SD card on the terminal, and the free space to both the terminal and LCD.
	 *
	 */

    /* Check free space */
    f_getfree("", &fre_clust, &pfs);

    total = (uint32_t)((pfs->n_fatent - 2) * pfs->csize * 0.5);
    char *buf = malloc(30*sizeof(char));
    sprintf (buf, "SD CARD Total Size: \t%lu\n",total);
    printT(buf);
    free(buf);
    free_space = (uint32_t)(fre_clust * pfs->csize * 0.5);
    buf = malloc(30*sizeof(char));
    sprintf (buf, "SD CARD Free Space: \t%lu\n",free_space);
    printT(buf);
    free(buf);
    free_space /= 1000;
    LCD_setCursor(3, 0);
    sprintf (debugTransmitBuffer, "%d mb Free", free_space);
    LCD_printf(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
}

