/*
 * ADC.c
 *
 *  Created on: Mar 23, 2021
 *      Author: mason
 */

#include "main.h"
#include "Global_Variables.h"
#include "ADC.h"
#include "string.h"

void populateSetSampleVoltages() {
	static float tempArray[] = { 12.0, 12.0, 3.0, 10.5, 10.5, 12.0, 13.6 };
	memcpy(&setSampleVoltages, &tempArray, sizeof(float) * 7);
}

//ADC Port A
void ADC_Ch0sel() {
	ADC_ChannelConfTypeDef sConfig = { 0 };

	sConfig.Channel = ADC_CHANNEL_0;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_144CYCLES;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
		Error_Handler();
	}
}

//ADC Port B
void ADC_Ch1sel() {
	ADC_ChannelConfTypeDef sConfig = { 0 };

	sConfig.Channel = ADC_CHANNEL_1;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_144CYCLES;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
		Error_Handler();
	}
}
//Vin
void ADC_Ch2sel() {
	ADC_ChannelConfTypeDef sConfig = { 0 };

	sConfig.Channel = ADC_CHANNEL_2;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
		Error_Handler();
	}
}

//Vfuse
void ADC_Ch3sel() {
	ADC_ChannelConfTypeDef sConfig = { 0 };

	sConfig.Channel = ADC_CHANNEL_3;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
		Error_Handler();
	}
}
//12V
void ADC_Ch4sel() {
	ADC_ChannelConfTypeDef sConfig = { 0 };

	sConfig.Channel = ADC_CHANNEL_4;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
		Error_Handler();
	}
}

//Vuser
void ADC_Ch5sel() {
	ADC_ChannelConfTypeDef sConfig = { 0 };

	sConfig.Channel = ADC_CHANNEL_5;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
		Error_Handler();
	}
}

uint32_t getVinVoltage() {
	uint16 sampleVal;
	ADC_Ch2sel();
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 100);
	sampleVal = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_Stop(&hadc1);
	return sampleVal;
}

uint32_t getFuseVoltage() {
	uint16 sampleVal;
	ADC_Ch3sel();
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 100);
	sampleVal = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_Stop(&hadc1);
	return sampleVal;
}

uint32_t getOutputVoltage() {
	uint16 sampleVal;
	ADC_Ch4sel();
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 100);
	sampleVal = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_Stop(&hadc1);
	return sampleVal;
}

uint32_t getSampleVoltage() {
	uint16 sampleVal;
	ADC_Ch5sel();
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 100);
	sampleVal = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_Stop(&hadc1);
	return sampleVal;
}

_Bool compareSampleVoltage(TboardConfig *Board, float *currSample, float *setVoltage) {
	float tolerance = *setVoltage * 0.1;
	if ((Board->BoardType == b935x || Board->BoardType == b937x) && (Board->GlobalTestNum > V_3))
		return true;
	if ((Board->BoardType == b422x) && (Board->GlobalTestNum > V_12output))	// No sample voltages on 422x boards
		return true;
	if ( (Board->BoardType == b427x) && (Board->GlobalTestNum > V_12))		// Only 12V sample voltage available on 427x boards
		return true;
	else if (Board->GlobalTestNum > V_trim)									// Single and dual boards sample 12v output, 3v & 12v sample
		return true;
	else {
		Board->VoltageBuffer[Board->GlobalTestNum] = *currSample;
		if ((*currSample < (*setVoltage + tolerance)) && (*currSample > (*setVoltage - tolerance))) {
			return true;
		} else
			return false;
	}
}
