#include <main.h>
#include "interogate_project.h"
#include "calibration.h"

void read_correctionFactors(){
	for (int i = 0; i <= 35; i++) {
		  int8 tempByte;
		  HAL_I2C_Mem_Read(&hi2c1, (0x51 << 1), i, 0x01, &tempByte, 0x01, HAL_MAX_DELAY);
		  HAL_Delay(5);
		  CorrectionFactors[i] = ~tempByte;
	}
	int8 * ptr = &CorrectionFactors[0];
	for (int i = 0; i < 6; i++) {
		Port1.CalibrationFactor[i] = *ptr++;
		Port2.CalibrationFactor[i] = *ptr++;
		Port3.CalibrationFactor[i] = *ptr++;
		Port4.CalibrationFactor[i] = *ptr++;
		Port5.CalibrationFactor[i] = *ptr++;
		Port6.CalibrationFactor[i] = *ptr++;
	}
}
void write_correctionFactors(){
	for (int i = 0; i < 36; i++) {
		HAL_I2C_Mem_Write(&hi2c1, (0x51 << 1) , i, 0x01, &CorrectionFactors[i], 0x01, HAL_MAX_DELAY);
		HAL_Delay(5);
	}
	  sprintf(Buffer, "EPROM Write Complete\n");
	  CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
}
