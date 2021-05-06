#include <File_Handling.h>
#include "stm32f4xx_hal.h"
#include "Programming.h"
#include "ff.h"

/* =============================>>>>>>>> NO CHANGES AFTER THIS LINE =====================================>>>>>>> */
void Mount_SD (const TCHAR* path) {
	/*
	 * Routine to mount the SD card, returning FRESULT, if return is 0 then SD mounted correctly
	 * see ff.h for further explanation of FATFS FRESULT responses.
	 * An explanation of each of these responses can be found at http://elm-chan.org/fsw/ff/00index_e.html
	 */
	fresult = f_mount(&fs, path, 1);
	if (fresult != FR_OK) {
		 printT("ERROR!!! in mounting SD CARD...\n\n");
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
	fresult = f_mount(NULL, path, 1);
	if (fresult == FR_OK) printT ("SD CARD UNMOUNTED successfully...\n\n\n");
	else printT("ERROR!!! in UNMOUNTING SD CARD\n\n\n");
}

FRESULT Scan_SD (char* pat) {
	/*
	 * Routine to scan through the SD card searching for the file passed to the routine.
	 * Similary to the previous routines this will return an FRESULT to determine correct operation
	 */
    DIR dir;
    UINT i;
    char *path = malloc(20*sizeof (char));
    sprintf (path, "%s",pat);
    fresult = f_opendir(&dir, path);
    if (fresult == FR_OK)
    {
        for (;;)
        {
            fresult = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (fresult != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
            if (fno.fattrib & AM_DIR)     /* It is a directory */
            {
            	if (!(strcmp ("SYSTEM~1", fno.fname))) continue;
            	char *buf = malloc(30*sizeof(char));
            	sprintf (buf, "Dir: %s\r\n", fno.fname);
            	printT(buf);
            	free(buf);
                i = strlen(path);
                sprintf(&path[i], "/%s", fno.fname);
                fresult = Scan_SD(path);                     /* Enter the directory */
                if (fresult != FR_OK) break;
                path[i] = 0;
            }
            else
            {   /* It is a file. */
           	   char *buf = malloc(30*sizeof(char));
               sprintf(buf,"File: %s/%s\n", path, fno.fname);
               printT(buf);
               free(buf);
            }
        }
        f_closedir(&dir);
    }
    free(path);
    return fresult;
}

_Bool Find_File (char* path, char* file) {
	/*
	 * As the previous routine scans for a specified file, this routine scans the SD card for a specific directory and file,
	 * this routine is typically for searching for the .hex board files as a string comparison is made to determine when the
	 * board file is found following the first 4 characters/ (the board type)
	 */
    DIR dir;
    FIL fil;
    UINT i;
    fresult = f_opendir(&dir, path);
    /* Open the directory */
    if(fresult != FR_OK) {
    	Mount_SD("/");
        if(fresult == FR_OK) {
        	f_opendir(&dir, path);
        }
    }
    if (fresult == FR_OK)
    {
    	if (!strcmp(fno.fname, file))
    		return true;
        for (;;)
        {
            fresult = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (fresult != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
            if (fno.fattrib & AM_DIR) {   /* Check if it is a directory */
            	if (!(strcmp ("SYSTEM~1", fno.fname))) continue; // Ensure directory is not SYSTEM~1
            	char *buf = malloc(30*sizeof(char));
            	sprintf (buf, "Dir: %s\r\n", fno.fname);
            	printT(buf);
            	free(buf);
                i = strlen(path);
                sprintf(&path[i], "/%s", fno.fname);
                fresult = Scan_SD(path);                     /* Enter the directory */
                if (fresult != FR_OK) break;
                path[i] = 0;
            } else {   /* If it is not a directory it is a file. */
               if (*file++ == fno.fname[0]) {					//Determine first character of file and compare to filename passed to function
                   if (*file++ == fno.fname[1]) {				//Determine second character of file and compare to filename passed to function
                       if (*file++ == fno.fname[2]) {			//Determine third character of file and compare to filename passed to function
                           if (*file++ == fno.fname[3]) {		//Determine fourth character of file and compare to filename passed to function
                           	   char *buf = malloc(30*sizeof(char));
                               sprintf(buf,"File: %s/%s\n", path, fno.fname);
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
        f_closedir(&dir);
    }
    free(path);
    return 0;
}

FRESULT Write_File (char *name, char *data) {
	/*
	 * Routine checks whether file exists, if it does the
	 */
	/**** check whether the file exists or not ****/
//	fresult = f_stat (name, &fno);
//	if (fresult != FR_OK) {
//		char *buf = malloc(100*sizeof(char));
//		sprintf (buf, "ERROR!!! *%s* does not exists\n\n", name);
//		printT (buf);
//	    free(buf);
//	    return fresult;	//return result if the file does not exist
//	} else {
	    /* Create a file with read write access and open it */
		//Check if FA_OPEN_EXISTING is required below
	    fresult = f_open(&fil, name, FA_CREATE_ALWAYS | FA_WRITE );	//Open file with write permissions, create the file even if existing so data is overwritten
	    if (fresult != FR_OK) {
	    	char *buf = malloc(100*sizeof(char));
	    	sprintf (buf, "ERROR!!! No. %d in opening file *%s*\n\n", fresult, name);
	    	printT(buf);
	        free(buf);
	        return fresult; //return result if error when opening file
	    } else {
	    	fresult = f_write(&fil, data, strlen(data), &bw);
	    	if (fresult != FR_OK) {
	    		char *buf = malloc(100*sizeof(char));
	    		sprintf (buf, "ERROR!!! No. %d while writing to the FILE *%s*\n\n", fresult, name);
	    		printT(buf);
	    		free(buf);
	    		return fresult; // if error when writing to file return the error
	    	}
	    }
	    Close_File(name);	//Following writing to the file close the file and return the result
	    return fresult;
	//}
}

FRESULT Read_File (char *name)
{
	/*
	 * Routine checks whether file exists, if true. The file is opened with read only permissions.
	 * The routine returns the contents of the file.
	 */

	/**** check whether the file exists or not ****/
	fresult = f_stat (name, &fno);
	if (fresult != FR_OK) {
		char *buf = malloc(100*sizeof(char));
		sprintf (buf, "ERRROR!!! *%s* does not exists\n\n", name);
		printT (buf);
		free(buf);
	    return fresult;
	} else {
		/* Open file to read */
		fresult = f_open(&fil, name, FA_READ);
		if (fresult != FR_OK) {
			char *buf = malloc(100*sizeof(char));
			sprintf (buf, "ERROR!!! No. %d in opening file *%s*\n\n", fresult, name);
			printT(buf);
		    free(buf);
		    return fresult;
		}
		char *buffer = malloc(sizeof(f_size(&fil)));
		fresult = f_read (&fil, buffer, f_size(&fil), &br);
		if (fresult != FR_OK) {
			char *buf = malloc(100*sizeof(char));
			free(buffer);
		 	sprintf (buf, "ERROR!!! No. %d in reading file *%s*\n\n", fresult, name);
		 	printT(buf);
		  	free(buf);
		} else {
			printT(buffer);
			free(buffer);
			/* Close file */
			fresult = f_close(&fil);
			if (fresult != FR_OK) {
				char *buf = malloc(100*sizeof(char));
				sprintf (buf, "ERROR!!! No. %d in closing file *%s*\n\n", fresult, name);
				printT(buf);
				free(buf);
			} else {
				char *buf = malloc(100*sizeof(char));
				sprintf (buf, "File *%s* CLOSED successfully\n", name);
				printT(buf);
				free(buf);
			}
		}
	    return fresult;
	}
}

FRESULT Create_File (char *name)
{
	/*
	 * f_stat used to determine whether file exists, if not f_open is used to create the file with
	 * read and write privileges. FRESULT returned to determine if successful.
	 */
	fresult = f_stat (name, &fno);
	if (fresult == FR_OK) {
		char *buf = malloc(100*sizeof(char));
		sprintf (buf, "ERROR!!! *%s* already exists!!!!\n use Update_File \n\n",name);
		printT(buf);
		free(buf);
	    return fresult;
	} else {
		fresult = f_open(&fil, name, FA_CREATE_ALWAYS|FA_READ|FA_WRITE);
		if (fresult != FR_OK) {
			char *buf = malloc(100*sizeof(char));
			sprintf (buf, "ERROR!!! No. %d in creating file *%s*\n\n", fresult, name);
			printT(buf);
			free(buf);
		    return fresult;
		} else {
			char *buf = malloc(100*sizeof(char));
			sprintf (buf, "*%s* created successfully\n Now use Write_File to write data\n",name);
			printT(buf);
			free(buf);
		}
	}
	Close_File(name);
    return fresult;
}

FRESULT Open_AppendFile(char * name) {
	/*
	 * First the routine opens the file passed to the function. Read and Write permissions are
	 * granted. Open Append allows for the creation of a file if it has not previously existed,
	 * with the read and write pointer pointed to EOF.
	 * As previous FRESULT is returned to inform of any errors
	 */
    fresult = f_open(&fil, name, FA_OPEN_APPEND | FA_WRITE | FA_READ);
    if (fresult != FR_OK) {
    	char *buf = malloc(100*sizeof(char));
    	sprintf (buf, "ERROR!!! No. %d in opening file *%s*\n\n", fresult, name);
    	printT(buf);
        free(buf);
    }
    return fresult;
}

void Close_File(char * name) {
	fresult = f_close(&fil);
	if (fresult != FR_OK) {
		char *buf = malloc(100*sizeof(char));
		sprintf (buf, "ERROR No. %d in closing file *%s*\n\n", fresult, name);
		printT(buf);
		free(buf);
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
	fresult = f_stat (name, &fno);
	if (fresult != FR_OK) {
		char *buf = malloc(100*sizeof(char));
		sprintf (buf, "ERROR!!! *%s* does not exists\n\n", name);
		printT (buf);
		free(buf);
	    return fresult;
	} else {
		 /* Create a file with read write access and open it */
	    /* Writing text */
	    fresult = f_write(&fil, data, strlen (data), &bw);
	    if (fresult != FR_OK) {
	    	char *buf = malloc(100*sizeof(char));
	    	sprintf (buf, "ERROR!!! No. %d in writing file *%s*\n\n", fresult, name);
	    	printT(buf);
	    	free(buf);
	    } else {
	    	char *buf = malloc(100*sizeof(char));
	    	sprintf (buf, "*%s* UPDATED successfully\n", name);
	    	printT(buf);
	    	free(buf);
	    }
	}
	delay_us(50);
	Close_File(name);
    return fresult;
}

FRESULT Create_Dir (char *name) {
	/*
	 * Creates directory with the name passed to the routine. As the other FATFS routines above
	 * if a process fails FRESULT returns a value regarding what the failure was
	 */
    fresult = f_mkdir(name);
    if (fresult == FR_OK) {
    	char *buf = malloc(100*sizeof(char));
    	sprintf (buf, "*%s* has been created successfully\n", name);
    	printT (buf);
    	free(buf);
    } else {
    	char *buf = malloc(100*sizeof(char));
    	sprintf (buf, "ERROR No. %d in creating directory *%s*\n\n", fresult,name);
    	printT(buf);
    	free(buf);
    }
    return fresult;
}
void OpenFile(char * fileName) {
	/*
	 * Routine to open file
	 * Failure if the file does not exist
	 * Process only allows for read only permissions
	 * If the file is found the size of the file is displayed on the terminal
	 */
	f_open(&fil, fileName, FA_READ | FA_OPEN_EXISTING);
	fileSize = f_size(&fil);
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

