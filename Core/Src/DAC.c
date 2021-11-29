/*
 * DAC.c
 *
 *  Created on: 25 Mar 2021
 *      Author: mason
 */
#include "main.h"
#include "DAC.h"
#include <Calibration.h>

//	=================================    DAC    =====================================//
/*
 * Set DAC to the port and value that is passed to the function, take the 16bit value and split into 2 8bit variables to be passed using the HAL spi routine
 * Depending on the port required pull the relevent CS low so the value can be written
 */
void DAC_set(uint8 Test_Port, uint16 DACvalue) {
	uns_ch DACdata[2];
	DACdata[0] = (DACvalue & 0xFF00) >> 8;
	DACdata[1] = DACvalue & 0xFF;
	switch (Test_Port) {
	case Port_1:
	case Port_2:
		HAL_GPIO_WritePin(DAC_CS1_GPIO_Port, DAC_CS1_Pin, GPIO_PIN_RESET);
		HAL_SPI_Transmit(&DAC_SPI, (uint8_t*) &DACdata, 2, 100);
		HAL_GPIO_WritePin(DAC_CS1_GPIO_Port, DAC_CS1_Pin, GPIO_PIN_SET);
		break;
	case Port_3:
	case Port_4:
		HAL_GPIO_WritePin(DAC_CS2_GPIO_Port, DAC_CS2_Pin, GPIO_PIN_RESET);
		HAL_SPI_Transmit(&DAC_SPI, (uint8_t*) &DACdata, 2, 100);
		HAL_GPIO_WritePin(DAC_CS2_GPIO_Port, DAC_CS2_Pin, GPIO_PIN_SET);
		break;
	case Port_5:
	case Port_6:
		HAL_GPIO_WritePin(DAC_CS3_GPIO_Port, DAC_CS3_Pin, GPIO_PIN_RESET);
		HAL_SPI_Transmit(&DAC_SPI, (uint8_t*) &DACdata, 2, 100);
		HAL_GPIO_WritePin(DAC_CS3_GPIO_Port, DAC_CS3_Pin, GPIO_PIN_SET);
		break;
	}
}

/*
 * Routine to set all the DAC to the same value, no port is to be passed to the routine as all ports should read the same value
 * No correction factor is added to each port
 */
void set_ALL_DAC(uint16 *DACvalue) {
	//DAC 1 set
	*DACvalue += 0x3000;	//OUTA Clear
	HAL_GPIO_WritePin(DAC_CS1_GPIO_Port, DAC_CS1_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&DAC_SPI, (uint8_t*) &DACvalue, 2, 100);
	HAL_GPIO_WritePin(DAC_CS1_GPIO_Port, DAC_CS1_Pin, GPIO_PIN_SET);
	*DACvalue &= 0XFFF;
	*DACvalue += 0xB000;	//OUTB Clear
	HAL_GPIO_WritePin(DAC_CS1_GPIO_Port, DAC_CS1_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&DAC_SPI, (uint8_t*) &DACvalue, 2, 100);
	HAL_GPIO_WritePin(DAC_CS1_GPIO_Port, DAC_CS1_Pin, GPIO_PIN_SET);
	//DAC 2 set
	*DACvalue &= 0XFFF;
	*DACvalue += 0x3000;	//OUTA Clear
	HAL_GPIO_WritePin(DAC_CS2_GPIO_Port, DAC_CS2_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&DAC_SPI, (uint8_t*) &DACvalue, 2, 100);
	HAL_GPIO_WritePin(DAC_CS2_GPIO_Port, DAC_CS2_Pin, GPIO_PIN_SET);
	*DACvalue &= 0XFFF;
	DACvalue += 0xB000;	//OUTB Clear
	HAL_GPIO_WritePin(DAC_CS2_GPIO_Port, DAC_CS2_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&DAC_SPI, (uint8_t*) &DACvalue, 2, 100);
	HAL_GPIO_WritePin(DAC_CS2_GPIO_Port, DAC_CS2_Pin, GPIO_PIN_SET);
	//DAC 3 set
	*DACvalue &= 0XFFF;
	*DACvalue += 0x3000;	//OUTA Clear
	HAL_GPIO_WritePin(DAC_CS3_GPIO_Port, DAC_CS3_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&DAC_SPI, (uint8_t*) &DACvalue, 2, 100);
	HAL_GPIO_WritePin(DAC_CS3_GPIO_Port, DAC_CS3_Pin, GPIO_PIN_SET);
	*DACvalue &= 0XFFF;
	*DACvalue += 0xB000;	//OUTB Clear
	HAL_GPIO_WritePin(DAC_CS3_GPIO_Port, DAC_CS3_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&DAC_SPI, (uint8_t*) &DACvalue, 2, 100);
	HAL_GPIO_WritePin(DAC_CS3_GPIO_Port, DAC_CS3_Pin, GPIO_PIN_SET);
}
	// Reset all DAC values back to 0, no output on voltage or current circuits
void reset_ALL_DAC() {
	uint8 DACvalue[2];
	//DAC 1 Clear
	DACvalue[0] = 0x30;	//OUTA Clear
	DACvalue[1] = 0x00;
	HAL_GPIO_WritePin(DAC_CS1_GPIO_Port, DAC_CS1_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&DAC_SPI, (uint8_t*) &DACvalue, 2, 100);
	HAL_GPIO_WritePin(DAC_CS1_GPIO_Port, DAC_CS1_Pin, GPIO_PIN_SET);
	DACvalue[0] = 0xB0;	//OUTB Clear
	DACvalue[1] = 0x00;
	HAL_GPIO_WritePin(DAC_CS1_GPIO_Port, DAC_CS1_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&DAC_SPI, (uint8_t*) &DACvalue, 2, 100);
	HAL_GPIO_WritePin(DAC_CS1_GPIO_Port, DAC_CS1_Pin, GPIO_PIN_SET);
	//DAC 2 Clear
	DACvalue[0] = 0x30;	//OUTA Clear
	DACvalue[1] = 0x00;
	HAL_GPIO_WritePin(DAC_CS2_GPIO_Port, DAC_CS2_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&DAC_SPI, (uint8_t*) &DACvalue, 2, 100);
	HAL_GPIO_WritePin(DAC_CS2_GPIO_Port, DAC_CS2_Pin, GPIO_PIN_SET);
	DACvalue[0] = 0xB0;	//OUTB Clear
	DACvalue[1] = 0x00;
	HAL_GPIO_WritePin(DAC_CS2_GPIO_Port, DAC_CS2_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&DAC_SPI, (uint8_t*) &DACvalue, 2, 100);
	HAL_GPIO_WritePin(DAC_CS2_GPIO_Port, DAC_CS2_Pin, GPIO_PIN_SET);
	//DAC 3 Clear
	DACvalue[0] = 0x30;	//OUTA Clear
	DACvalue[1] = 0x00;
	HAL_GPIO_WritePin(DAC_CS3_GPIO_Port, DAC_CS3_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&DAC_SPI, (uint8_t*) &DACvalue, 2, 100);
	HAL_GPIO_WritePin(DAC_CS3_GPIO_Port, DAC_CS3_Pin, GPIO_PIN_SET);
	DACvalue[0] = 0xB0;	//OUTB Clear
	DACvalue[1] = 0x00;
	HAL_GPIO_WritePin(DAC_CS3_GPIO_Port, DAC_CS3_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&DAC_SPI, (uint8_t*) &DACvalue, 2, 100);
	HAL_GPIO_WritePin(DAC_CS3_GPIO_Port, DAC_CS3_Pin, GPIO_PIN_SET);
}
