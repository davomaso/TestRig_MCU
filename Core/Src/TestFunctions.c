///*
// * TestFunctions.c
// *
// *  Created on: 31 Jul 2020
// *      Author: David
//
//	Each Test to be included here, As no input configurations on
//	iConfigure go up to 0x10 the outputs and inputs can be included
//	in the same .C file
// */
#include "main.h"
#include "Test.h"
#include "Board_Config.h"
#include <time.h>
#include "interogate_project.h"
#include "calibration.h"
#include "ADC_Variables.h"
#include "DAC_Variables.h"

_Bool twoWireLatching(TboardConfig*, uint8, _Bool);
float setCurrentTestDAC(uint8);
float setVoltageTestDAC(uint8, uint8);
float setAsyncPulseCount(TboardConfig*, uint8);
float setSDItwelveValue(uint8);
//void DAC_set(uint8, int);

void MUX_Sel(uint8, uint8);

extern void delay_us(int);
extern double round(double);

void TestFunction(TboardConfig *Board) {
	//	HAL_GPIO_WritePin(MUX_RS_GPIO_Port, MUX_RS_Pin, GPIO_PIN_SET);
	uint8 totalPortCount = 0;
	uint8 currPort;
	//====================== Analog Test Count ======================//
	for (currPort = 0; currPort < Board->latchPortCount; currPort++) {
		switch (Board->TestCode[totalPortCount]) {
		case TWO_WIRE_LATCHING:
			CHval[Board->GlobalTestNum][totalPortCount++] = twoWireLatching(Board, currPort, 1);
			break;
		case NOTEST:
			CHval[Board->GlobalTestNum][totalPortCount++] = twoWireLatching(Board, currPort, 0);
			break;
		}
	}
	//====================== Analog Test Count ======================//
	for (currPort = 0; currPort < Board->analogInputCount; currPort++) {
		switch (Board->TestCode[totalPortCount]) {
		case TWENTY_AMP:
			CHval[Board->GlobalTestNum][totalPortCount++] = setCurrentTestDAC(currPort);
			break;
		case THREE_VOLT:
			CHval[Board->GlobalTestNum][totalPortCount++] = setVoltageTestDAC(currPort, THREE_VOLT);
			break;
		case TWOFIVE_VOLT:
			CHval[Board->GlobalTestNum][totalPortCount++] = setVoltageTestDAC(currPort, TWOFIVE_VOLT);
			break;
		case ONE_VOLT:
			CHval[Board->GlobalTestNum][totalPortCount++] = setVoltageTestDAC(currPort, ONE_VOLT);
			break;
		case ASYNC_PULSE:
			CHval[Board->GlobalTestNum][totalPortCount++] = setAsyncPulseCount(Board, currPort);
			break;
		case SDI_TWELVE:
			CHval[Board->GlobalTestNum][totalPortCount++] = setSDItwelveValue(currPort);
			break;
		}
	}
	//====================== Digital Test Count ======================//
	for (currPort = 6; currPort < Board->digitalInputCout + 6; currPort++) {
		switch (Board->TestCode[totalPortCount]) {
		case ASYNC_PULSE:
			CHval[Board->GlobalTestNum][totalPortCount++] = setAsyncPulseCount(Board, currPort);
			MUX_Sel(currPort, Board->TestCode[totalPortCount]);
			break;
		}
	}
	if (Board->BoardType == b402x) {
		if (currPort == 9 && Board->TestCode[totalPortCount] == 0x01) {
			CHval[Board->GlobalTestNum][totalPortCount++] = 1; //RunOutputTest();
		} else
			CHval[Board->GlobalTestNum][totalPortCount++] = 0;
	}
	OutputsSet = true;
}
//	==================================================================================	//

//	=================================   Two Wire   =================================	//
_Bool twoWireLatching(TboardConfig *Board, uint8 Test_Port, _Bool state) {
	switch (Test_Port) {
	case Port_1:
		if (state)
			SET_BIT(Board->LatchTestPort, LATCH_PORT_ONE);
		break;
	case Port_2:
		if (state)
			SET_BIT(Board->LatchTestPort, LATCH_PORT_TWO);
		break;
	case Port_3:
		if (state)
			SET_BIT(Board->LatchTestPort, LATCH_PORT_THREE);
		break;
	case Port_4:
		if (state)
			SET_BIT(Board->LatchTestPort, LATCH_PORT_FOUR);
		break;
	}
	return state;
}
//	==================================================================================	//

//	================================   20mA Current   ================================	//
float setCurrentTestDAC(uint8 Test_Port) {
	float current;
	uint16 Corrected_DACvalue;
	uint16 DAC_Value;
	//Correction Factor
	if (!Port[Test_Port].lowItestComplete) {
		current = 4.0;
		Port[Test_Port].lowItestComplete = true;
		DAC_Value = DAC_4amp;
		Corrected_DACvalue = DAC_Value + Port[Test_Port].CalibrationFactor[I_4];
	} else {
		current = 17.5;
		Port[Test_Port].lowItestComplete = true;
		DAC_Value = DAC_175amp;
		Corrected_DACvalue = DAC_Value + Port[Test_Port].CalibrationFactor[I_175];
	}
	if (Test_Port == Port_1 || Test_Port == Port_3 || Test_Port == Port_5)
		Corrected_DACvalue += 0x3000;
	else if (Test_Port == Port_2 || Test_Port == Port_4 || Test_Port == Port_6)
		Corrected_DACvalue += 0xB000;
	DAC_set(Test_Port, Corrected_DACvalue);
	MUX_Sel(Test_Port, TWENTY_AMP);
	return current;
}
//	==================================================================================	//

//	=================================   Three Volt   =================================	//
float setVoltageTestDAC(uint8 Test_Port, uint8 TestCode) {
	float voltage;
	uint16 Corrected_DACvalue;
	uint16 DAC_Value;
	switch (TestCode) {
	case ONE_VOLT:
		voltage = 0.50;
		DAC_Value = DAC_05volt;
		break;
	case TWOFIVE_VOLT:
		voltage = 2.4;
		DAC_Value = DAC_24volt;
		break;
	}
	if (TestCode == ONE_VOLT)
		Corrected_DACvalue = DAC_Value + Port[Test_Port].CalibrationFactor[V_05];
	else if (TestCode == TWOFIVE_VOLT)
		Corrected_DACvalue = DAC_Value + Port[Test_Port].CalibrationFactor[V_24];

	//	randDACvolt = round((Corrected_voltage * 4096 / 3.6864)); //round((voltage * 3448) / 3.014);
	if (Test_Port == Port_1 || Test_Port == Port_3 || Test_Port == Port_5)
		Corrected_DACvalue += 0x3000;
	else if (Test_Port == Port_2 || Test_Port == Port_4 || Test_Port == Port_6)
		Corrected_DACvalue += 0xB000;

	DAC_set(Test_Port, Corrected_DACvalue);
	MUX_Sel(Test_Port, THREE_VOLT);
	return voltage;
}
//	==================================================================================	//

//	===================================   ASYNC   ===================================	//
float setAsyncPulseCount(TboardConfig *Board, uint8 Test_Port) {
	HAL_GPIO_WritePin(Port[Test_Port].Async.Port, Port[Test_Port].Async.Pin, GPIO_PIN_RESET);
	MUX_Sel(Test_Port, ASYNC_PULSE);
	switch (Board->GlobalTestNum) {
	case 0:
	case 1:
		Port[Test_Port].Async.PulseCount = 5;
		break;
	case 2:
	case 3:
		Port[Test_Port].Async.PulseCount = 10;
		break;
	case 4:
	case 5:
		Port[Test_Port].Async.PulseCount = 15;
		break;
	}
	HAL_Delay(10);
	Port[Test_Port].Async.fcount = 5;
	Port[Test_Port].Async.PulseState = true;
	return (float) Port[Test_Port].Async.FilterEnabled ?
			Port[Test_Port].Async.PulseCount : (11 * Port[Test_Port].Async.PulseCount);
}
//	===================================================================================	//

//	===================================   SDI-12    ===================================	//
float setSDItwelveValue(uint8 Test_Port) {
	MUX_Sel(Test_Port, SDI_TWELVE);
	SDSstate = SDSundef;
	Port[Test_Port].Sdi.Enabled = true;
	Port[Test_Port].Sdi.Address = Test_Port;
	return Port[Test_Port].Sdi.setValue;
}
//	===================================================================================	//

//	===================================    MUX    ===================================	//
void MUX_Sel(uint8 Test_Port, uint8 Test) {
	_Bool MuxState = HAL_GPIO_ReadPin(MUX_A0_GPIO_Port, MUX_A0_Pin);
	if (Test_Port <= Port_6) {
		switch (Test_Port) {
		case Port_1:
			HAL_GPIO_WritePin(MUX_WRodd1_GPIO_Port, MUX_WRodd1_Pin, GPIO_PIN_RESET);
			break;
		case Port_2:
			HAL_GPIO_WritePin(MUX_WReven1_GPIO_Port, MUX_WReven1_Pin, GPIO_PIN_RESET);
			break;
		case Port_3:
			HAL_GPIO_WritePin(MUX_WRodd2_GPIO_Port, MUX_WRodd2_Pin, GPIO_PIN_RESET);
			break;
		case Port_4:
			HAL_GPIO_WritePin(MUX_WReven2_GPIO_Port, MUX_WReven2_Pin, GPIO_PIN_RESET);
			break;
		case Port_5:
			HAL_GPIO_WritePin(MUX_WRodd3_GPIO_Port, MUX_WRodd3_Pin, GPIO_PIN_RESET);
			break;
		case Port_6:
			HAL_GPIO_WritePin(MUX_WReven3_GPIO_Port, MUX_WReven3_Pin, GPIO_PIN_RESET);
			break;
		}
		delay_us(5);
		switch (Test) {
		case SDI_TWELVE:
			HAL_GPIO_WritePin(MUX_A0_GPIO_Port, MUX_A0_Pin, GPIO_PIN_RESET); //MUX Address = 1
			HAL_GPIO_WritePin(MUX_A1_GPIO_Port, MUX_A1_Pin, GPIO_PIN_RESET);
			break;
		case THREE_VOLT:
			HAL_GPIO_WritePin(MUX_A0_GPIO_Port, MUX_A0_Pin, GPIO_PIN_SET); //MUX Address = 2
			HAL_GPIO_WritePin(MUX_A1_GPIO_Port, MUX_A1_Pin, GPIO_PIN_RESET);
			break;
		case TWENTY_AMP:
			HAL_GPIO_WritePin(MUX_A0_GPIO_Port, MUX_A0_Pin, GPIO_PIN_RESET); //MUX Address = 3
			HAL_GPIO_WritePin(MUX_A1_GPIO_Port, MUX_A1_Pin, GPIO_PIN_SET);
			break;
		case ASYNC_PULSE:
			HAL_GPIO_WritePin(MUX_A0_GPIO_Port, MUX_A0_Pin, GPIO_PIN_SET); //MUX Address = 4
			HAL_GPIO_WritePin(MUX_A1_GPIO_Port, MUX_A1_Pin, GPIO_PIN_SET);
			break;
		}
		delay_us(5);
		switch (Test_Port) {
		case Port_1:
			HAL_GPIO_WritePin(MUX_WRodd1_GPIO_Port, MUX_WRodd1_Pin, GPIO_PIN_SET);
			break;
		case Port_2:
			HAL_GPIO_WritePin(MUX_WReven1_GPIO_Port, MUX_WReven1_Pin, GPIO_PIN_SET);
			break;
		case Port_3:
			HAL_GPIO_WritePin(MUX_WRodd2_GPIO_Port, MUX_WRodd2_Pin, GPIO_PIN_SET);
			break;
		case Port_4:
			HAL_GPIO_WritePin(MUX_WReven2_GPIO_Port, MUX_WReven2_Pin, GPIO_PIN_SET);
			break;
		case Port_5:
			HAL_GPIO_WritePin(MUX_WRodd3_GPIO_Port, MUX_WRodd3_Pin, GPIO_PIN_SET);
			break;
		case Port_6:
			HAL_GPIO_WritePin(MUX_WReven3_GPIO_Port, MUX_WReven3_Pin, GPIO_PIN_SET);
			break;
		}
		if (MuxState)
			HAL_GPIO_WritePin(MUX_A0_GPIO_Port, MUX_A0_Pin, GPIO_PIN_SET);
		else
			HAL_GPIO_WritePin(MUX_A0_GPIO_Port, MUX_A0_Pin, GPIO_PIN_RESET);
	}

}

void reset_ALL_MUX() {
	for (uint8 i = Port_1; i <= Port_9; i++) {
		HAL_GPIO_WritePin(Port[i].Async.Port, Port[i].Async.Pin, GPIO_PIN_RESET);
		MUX_Sel(i, ASYNC_PULSE);
	}
}
//	=================================================================================//

//	=================================================================================//
void ADC_MUXsel(uint8 ADCport) {
	switch (ADCport) {
	case Port_1:
		HAL_GPIO_WritePin(ADC_MUX_A_GPIO_Port, ADC_MUX_A_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(ADC_MUX_B_GPIO_Port, ADC_MUX_B_Pin, GPIO_PIN_RESET);
		break;
	case Port_2:
		HAL_GPIO_WritePin(ADC_MUX_A_GPIO_Port, ADC_MUX_A_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ADC_MUX_B_GPIO_Port, ADC_MUX_B_Pin, GPIO_PIN_RESET);
		break;
	case Port_3:
		HAL_GPIO_WritePin(ADC_MUX_A_GPIO_Port, ADC_MUX_A_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(ADC_MUX_B_GPIO_Port, ADC_MUX_B_Pin, GPIO_PIN_SET);
		break;
	case Port_4:
		HAL_GPIO_WritePin(ADC_MUX_A_GPIO_Port, ADC_MUX_A_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ADC_MUX_B_GPIO_Port, ADC_MUX_B_Pin, GPIO_PIN_SET);
		break;
	}
}
//	=================================================================================//

//	============================    Print Results    ================================//
void PrintLatchResults() {
	float LatchCurrent1;
	float LatchCurrent2;
	printT("\n\n =======================  Latch Test  =======================\n\n");
	sprintf(&debugTransmitBuffer, "\n==============   Port A Latch time:   High: %d Low: %d   ==============\n",
			LatchPortA.HighPulseWidth, LatchPortA.LowPulseWidth);
	printT(&debugTransmitBuffer);

	sprintf(&debugTransmitBuffer, "\n==============   Port A Voltage:   High: %.3f Low: %.3f   ==============\n",
			LatchPortA.highVoltage, LatchPortA.lowVoltage);
	printT(&debugTransmitBuffer);

	//Port B Latch Time
	sprintf(&debugTransmitBuffer, "\n==============   Port B Latch time:   High: %d  Low: %d   ==============\n",
			LatchPortB.HighPulseWidth, LatchPortB.LowPulseWidth);
	printT(&debugTransmitBuffer);

	sprintf(&debugTransmitBuffer, "\n==============   Port B Voltage:   High: %.3f Low: %.3f   ==============\n\n",
			LatchPortB.highVoltage, LatchPortB.lowVoltage);
	printT(&debugTransmitBuffer);

	sprintf(&debugTransmitBuffer, "\n==============   Vin Voltage:   AVG: %.3f Min: %.3f   ==============\n",
			Vin.average, Vin.lowVoltage);
	printT(&debugTransmitBuffer);

	sprintf(&debugTransmitBuffer, "\n==============   Fuse Voltage:   AVG: %.3f Min: %.3f   ==============\n",
			Vfuse.average, Vfuse.lowVoltage);
	printT(&debugTransmitBuffer);

	sprintf(&debugTransmitBuffer, "\n==============   MOSFET 1 Voltage:   High: %.3f Low: %.3f   ==============\n",
			MOSFETvoltageA.highVoltage, MOSFETvoltageA.lowVoltage);
	printT(&debugTransmitBuffer);

	sprintf(&debugTransmitBuffer, "\n==============   MOSFET 2 Voltage:   High: %.3f Low: %.3f   ==============\n",
			MOSFETvoltageB.highVoltage, MOSFETvoltageB.lowVoltage);
	printT(&debugTransmitBuffer);

	LatchCurrent2 = LatchPortB.highVoltage - LatchPortA.lowVoltage;
	LatchCurrent1 = LatchPortA.highVoltage - LatchPortB.lowVoltage;
	LatchCurrent1 /= ADC_Rcurrent;
	LatchCurrent2 /= ADC_Rcurrent;

	sprintf(&debugTransmitBuffer, "\n==============   Lactch Current:   Pulse 1: %f Pulse 2: %f   ==============\n",
			LatchCurrent1, LatchCurrent2);
	printT(&debugTransmitBuffer);
}

void normaliseLatchResults() {
	//Port A Voltage
	LatchPortA.highVoltage = LatchPortA.HighPulseWidth > 0 ? LatchPortA.highVoltage / LatchPortA.HighPulseWidth : 0;
	LatchPortA.highVoltage *= (ADC_MAX_INPUT_VOLTAGE / ADC_RESOLUTION); //Previous value was 16.17 when resistor is set to 150
	LatchPortA.lowVoltage /= LatchPortA.LowPulseWidth;
	LatchPortA.lowVoltage *= (ADC_MAX_INPUT_VOLTAGE / ADC_RESOLUTION);

	//Port B Voltage
	LatchPortB.highVoltage = LatchPortB.HighPulseWidth > 0 ? LatchPortB.highVoltage / LatchPortB.HighPulseWidth : 0;
	LatchPortB.highVoltage *= (ADC_MAX_INPUT_VOLTAGE / ADC_RESOLUTION);
	LatchPortB.lowVoltage /= LatchPortB.LowPulseWidth;
	LatchPortB.lowVoltage *= (ADC_MAX_INPUT_VOLTAGE / ADC_RESOLUTION);

	//Vin Voltage
	Vin.lowVoltage *= (MAX_SOURCE_VALUE / ADC_RESOLUTION);
	Vin.average *= (MAX_SOURCE_VALUE / ADC_RESOLUTION);
	//Fuse Voltage
	Vfuse.lowVoltage *= (MAX_SOURCE_VALUE / ADC_RESOLUTION);
	Vfuse.average *= (MAX_SOURCE_VALUE / ADC_RESOLUTION);
	//MOSFET Voltage
	MOSFETvoltageA.lowVoltage *= (ADC_MAX_INPUT_VOLTAGE / ADC_RESOLUTION);
	MOSFETvoltageA.highVoltage *= (ADC_MAX_INPUT_VOLTAGE / ADC_RESOLUTION);
	MOSFETvoltageB.lowVoltage *= (ADC_MAX_INPUT_VOLTAGE / ADC_RESOLUTION);
	MOSFETvoltageB.highVoltage *= (ADC_MAX_INPUT_VOLTAGE / ADC_RESOLUTION);

	//Store Values
	LatchRes.tOn =
			(LatchPortA.HighPulseWidth >= LatchPortB.LowPulseWidth) ?
					LatchPortA.HighPulseWidth : LatchPortB.LowPulseWidth;
	LatchRes.tOff =
			(LatchPortB.HighPulseWidth >= LatchPortA.LowPulseWidth) ?
					LatchPortB.HighPulseWidth : LatchPortA.LowPulseWidth;

}

void TransmitResults() {
	printT("==============   ADC Average Results   ==============");
	for (int i = 0; i < LatchCountTimer; i++) {
		sprintf(debugTransmitBuffer, "%d,%d,%d,%d\n", i, LatchPortA.avg_Buffer[i], LatchPortB.avg_Buffer[i],
				Vfuse.avg_Buffer[i]);
		printT(&debugTransmitBuffer);
	}
}
//	=================================================================================//
//	=================================================================================//
//	=================================================================================//
