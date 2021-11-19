/*
 * ADC.h
 *
 *  Created on: 2 Sep 2021
 *      Author: mason
 */

#ifndef INC_ADC_H_
#define INC_ADC_H_
#include "Board_Config.h"

void populateSetSampleVoltages(void);

void ADC_Ch0sel(void);
void ADC_Ch1sel(void);
void ADC_Ch2sel(void);
void ADC_Ch3sel(void);
void ADC_Ch4sel(void);
void ADC_Ch5sel(void);

uint32_t getVinVoltage(void);
uint32_t getFuseVoltage(void);
uint32_t getOutputVoltage(void);
uint32_t getSampleVoltage(void);
_Bool compareSampleVoltage(TboardConfig *, float *, float *);


#endif /* INC_ADC_H_ */
