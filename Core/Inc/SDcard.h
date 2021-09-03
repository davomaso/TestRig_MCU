/*
 * SDcard.h
 *
 *  Created on: 2 Sep 2021
 *      Author: mason
 */

#ifndef INC_SDCARD_H_
#define INC_SDCARD_H_

#include "Board_Config.h"
#include "File_Handling.h"


FRESULT SDInit(TfileConfig *, TCHAR *);
_Bool FindBoardFile(TboardConfig *, TfileConfig *);

#endif /* INC_SDCARD_H_ */
