#include <main.h>
#include "interogate_project.h"
#include "calibration.h"

void read_correctionFactors(){
	int8 tempByte;
	for (uint8 i = Port_1; i < Port_6;i++) {
		for (uint8 j = 0; j < 6; j++) {
			  HAL_I2C_Mem_Read(&hi2c1, (0x51 << 1), (i*6)+j, 0x01, &tempByte, 0x01, HAL_MAX_DELAY);
			  Port[i].CalibrationFactor[j] = ~tempByte;
			}
		}
}
void write_correctionFactors(){
	uint8 tempByte;
	for (uint8 i = Port_1; i < Port_6;i++) {
		for (uint8 j = 0; j < 6; j++) {
			  tempByte = ~(Port[i].CalibrationFactor[j]);
			  HAL_I2C_Mem_Write(&hi2c1, (0x51 << 1) , i, 0x01, &tempByte, 0x01, HAL_MAX_DELAY);
			  HAL_Delay(5);
			}
		}
	  sprintf(debugTransmitBuffer, "EPROM Write Complete\n");
	  CDC_Transmit_FS(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
}
