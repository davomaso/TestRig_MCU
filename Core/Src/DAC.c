/*
 * DAC.c
 *
 *  Created on: 25 Mar 2021
 *      Author: mason
 */
#include "main.h"

void DAC_set(uint8,uint16 *);
void reset_ALL_DAC(void);
void set_ALL_DAC(int *);

//	=================================    DAC    =====================================//
void DAC_set(uint8 Test_Port, uint16 * DACvalue) {
	switch (Test_Port) {
	case 1:
	case 2:
		HAL_GPIO_WritePin(DAC_CS1_GPIO_Port, DAC_CS1_Pin, GPIO_PIN_RESET);
		HAL_SPI_Transmit(&DAC_SPI, &DACvalue, 2, 100);
		HAL_GPIO_WritePin(DAC_CS1_GPIO_Port, DAC_CS1_Pin, GPIO_PIN_SET);
		break;
	case 3:
	case 4:
		HAL_GPIO_WritePin(DAC_CS2_GPIO_Port, DAC_CS2_Pin, GPIO_PIN_RESET);
		HAL_SPI_Transmit(&DAC_SPI, &DACvalue, 2, 100);
		HAL_GPIO_WritePin(DAC_CS2_GPIO_Port, DAC_CS2_Pin, GPIO_PIN_SET);
		break;
	case 5:
	case 6:
		HAL_GPIO_WritePin(DAC_CS3_GPIO_Port, DAC_CS3_Pin, GPIO_PIN_RESET);
		HAL_SPI_Transmit(&DAC_SPI, &DACvalue, 2, 100);
		HAL_GPIO_WritePin(DAC_CS3_GPIO_Port, DAC_CS3_Pin, GPIO_PIN_SET);
		break;
	}
}

void set_ALL_DAC(int * DACvalue) {
	//DAC 1 set
	*DACvalue += 0x3000;	//OUTA Clear
	HAL_GPIO_WritePin(DAC_CS1_GPIO_Port, DAC_CS1_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&DAC_SPI, &DACvalue, 2, 100);
	HAL_GPIO_WritePin(DAC_CS1_GPIO_Port, DAC_CS1_Pin, GPIO_PIN_SET);
	*DACvalue += 0xB000;	//OUTB Clear
	HAL_GPIO_WritePin(DAC_CS1_GPIO_Port, DAC_CS1_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&DAC_SPI, &DACvalue, 2, 100);
	HAL_GPIO_WritePin(DAC_CS1_GPIO_Port, DAC_CS1_Pin, GPIO_PIN_SET);
	//DAC 2 set
	*DACvalue += 0x3000;	//OUTA Clear
	HAL_GPIO_WritePin(DAC_CS2_GPIO_Port, DAC_CS2_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&DAC_SPI, &DACvalue, 2, 100);
	HAL_GPIO_WritePin(DAC_CS2_GPIO_Port, DAC_CS2_Pin, GPIO_PIN_SET);
	DACvalue = 0xB000;	//OUTB Clear
	HAL_GPIO_WritePin(DAC_CS2_GPIO_Port, DAC_CS2_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&DAC_SPI, &DACvalue, 2, 100);
	HAL_GPIO_WritePin(DAC_CS2_GPIO_Port, DAC_CS2_Pin, GPIO_PIN_SET);
	//DAC 3 set
	*DACvalue += 0x3000;	//OUTA Clear
	HAL_GPIO_WritePin(DAC_CS3_GPIO_Port, DAC_CS3_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&DAC_SPI, &DACvalue, 2, 100);
	HAL_GPIO_WritePin(DAC_CS3_GPIO_Port, DAC_CS3_Pin, GPIO_PIN_SET);
	*DACvalue += 0xB000;	//OUTB Clear
	HAL_GPIO_WritePin(DAC_CS3_GPIO_Port, DAC_CS3_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&DAC_SPI, &DACvalue, 2, 100);
	HAL_GPIO_WritePin(DAC_CS3_GPIO_Port, DAC_CS3_Pin, GPIO_PIN_SET);
}

void reset_ALL_DAC() {
	int DACvalue;
	//DAC 1 Clear
	DACvalue = 0x3000;	//OUTA Clear
	HAL_GPIO_WritePin(DAC_CS1_GPIO_Port, DAC_CS1_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&DAC_SPI, &DACvalue, 2, 100);
	HAL_GPIO_WritePin(DAC_CS1_GPIO_Port, DAC_CS1_Pin, GPIO_PIN_SET);
	DACvalue = 0xB000;	//OUTB Clear
	HAL_GPIO_WritePin(DAC_CS1_GPIO_Port, DAC_CS1_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&DAC_SPI, &DACvalue, 2, 100);
	HAL_GPIO_WritePin(DAC_CS1_GPIO_Port, DAC_CS1_Pin, GPIO_PIN_SET);
	//DAC 2 Clear
	DACvalue = 0x3000;	//OUTA Clear
	HAL_GPIO_WritePin(DAC_CS2_GPIO_Port, DAC_CS2_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&DAC_SPI, &DACvalue, 2, 100);
	HAL_GPIO_WritePin(DAC_CS2_GPIO_Port, DAC_CS2_Pin, GPIO_PIN_SET);
	DACvalue = 0xB000;	//OUTB Clear
	HAL_GPIO_WritePin(DAC_CS2_GPIO_Port, DAC_CS2_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&DAC_SPI, &DACvalue, 2, 100);
	HAL_GPIO_WritePin(DAC_CS2_GPIO_Port, DAC_CS2_Pin, GPIO_PIN_SET);
	//DAC 3 Clear
	DACvalue = 0x3000;	//OUTA Clear
	HAL_GPIO_WritePin(DAC_CS3_GPIO_Port, DAC_CS3_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&DAC_SPI, &DACvalue, 2, 100);
	HAL_GPIO_WritePin(DAC_CS3_GPIO_Port, DAC_CS3_Pin, GPIO_PIN_SET);
	DACvalue = 0xB000;	//OUTB Clear
	HAL_GPIO_WritePin(DAC_CS3_GPIO_Port, DAC_CS3_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&DAC_SPI, &DACvalue, 2, 100);
	HAL_GPIO_WritePin(DAC_CS3_GPIO_Port, DAC_CS3_Pin, GPIO_PIN_SET);
}
