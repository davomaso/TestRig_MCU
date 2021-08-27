#include <main.h>
#include "interogate_project.h"
#include "calibration.h"

void read_correctionFactors(){
	int8 tempByte;
	for (uint8 i = Port_1; i <= Port_6;i++) {
		for (uint8 j = Port_1; j <= Port_6; j++) {
			uint8 ADR = (i*6)+j;
			HAL_I2C_Mem_Read(&hi2c1, (0x51 << 1), ADR, 0x01, &tempByte, 0x01, HAL_MAX_DELAY);
			Port[j].CalibrationFactor[i] = ~tempByte;
			}
		}
}
void write_correctionFactors(){
	uint8 tempByte;
	for (uint8 i = Port_1; i <= Port_6;i++) {
		for (uint8 j = Port_1; j <= Port_6; j++) {
			uint8 ADR = (i*6)+j;
			tempByte = ~(Port[j].CalibrationFactor[i]);
			HAL_I2C_Mem_Write(&hi2c1, (0x51 << 1) , ADR, 0x01, &tempByte, 0x01, HAL_MAX_DELAY);
			HAL_Delay(10);
			}
		}
	  sprintf(debugTransmitBuffer, "EPROM Write Complete\n");
	  CDC_Transmit_FS(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
}
