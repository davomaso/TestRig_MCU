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

int twoWireLatching(uint8,bool);
float twentyAmp(uint8);
float threeVolt(uint8, uint8);
float asyncPulse(uint8);
float sdiTwelve(uint8);
//void DAC_set(uint8, int);
void set_ALL_DAC(int);
void MUX_Sel(uint8, uint8);
void DAC_set(uint8,int);

extern void delay_us(int);
extern double round(double);

void TestFunction(uint8 *TestCode, uint8 LatchportCount, uint8 analogportCount, uint8 digitalportCount) {
	HAL_GPIO_WritePin(MUX_RS_GPIO_Port, MUX_RS_Pin, GPIO_PIN_SET);
	uint8 totalPortCount = 0;
	//====================== Analog Test Count ======================//
	for (currPort = 0; currPort < LatchportCount; currPort++) {
		switch (*TestCode) {
				case TWO_WIRE_LATCHING:
					CHval[GlobalTestNum][totalPortCount++] = twoWireLatching(currPort,1);
					break;
				case NOTEST:
					CHval[GlobalTestNum][totalPortCount++] = twoWireLatching(currPort,0);
					break;
		}
		TestCode++;
	}
	//====================== Analog Test Count ======================//
	for (currPort = 1; currPort <= analogportCount; currPort++) {
		switch (*TestCode) {
		case TWENTY_AMP:
			CHval[GlobalTestNum][totalPortCount++] = twentyAmp(currPort);
			MUX_Sel(currPort, *TestCode);
			break;
		case THREE_VOLT:
			CHval[GlobalTestNum][totalPortCount++] = threeVolt(currPort, *TestCode);
			MUX_Sel(currPort, *TestCode);
			break;
		case TWOFIVE_VOLT:
			CHval[GlobalTestNum][totalPortCount++] = threeVolt(currPort, *TestCode);
			MUX_Sel(currPort, *TestCode);
			break;
		case ONE_VOLT:
			CHval[GlobalTestNum][totalPortCount++] = threeVolt(currPort, *TestCode);
			MUX_Sel(currPort, *TestCode);
			break;
		case ASYNC_PULSE:
			CHval[GlobalTestNum][totalPortCount++] = asyncPulse(currPort);
			break;
		case SDI_TWELVE:
			CHval[GlobalTestNum][totalPortCount++] = sdiTwelve(currPort);
			break;
		}
		TestCode++;
	}
	//====================== Digital Test Count ======================//
	for (currPort = 7; currPort <= digitalportCount+7; currPort++) {
		switch (*TestCode) {
		case ASYNC_PULSE:
			CHval[GlobalTestNum][totalPortCount++] = asyncPulse(currPort);
			break;
		}
		TestCode++;
	}
}
//	==================================================================================	//


//	=================================   Two Wire   =================================	//
int twoWireLatching(uint8 Test_Port,_Bool state) {
		switch(Test_Port){
		case 0:
			LatchPort1 = (Test_Port == 0) && state ? true : false;
			break;
		case 1:
			LatchPort2 = (Test_Port == 1) && state ? true : false;
			break;
		case 2:
			LatchPort3 = (Test_Port == 2) && state ? true : false;
			break;
		case 3:
			LatchPort4 = (Test_Port == 3) && state ? true : false;
			break;
		}
	return state;
}
//	==================================================================================	//


//	================================   20mA Current   ================================	//
float twentyAmp(uint8 Test_Port) {
	float current;
	float corrected_current;
	int randDACcurrent;
	//Correction Factor
	switch (Test_Port) {
	case 1:
		if(!Port1.lowItestComplete){
			current = 4;
			Port1.lowItestComplete = true;
			randDACcurrent = round((current * 4096) / 27.7573);
			corrected_current = randDACcurrent + Port1.CalibrationFactor[I_4];
		} else {
			current = 17.5;
			randDACcurrent = round((current * 4096) / 27.7573);
			corrected_current = randDACcurrent + Port1.CalibrationFactor[I_175];
		}
		break;
	case 2:
		if(!Port2.lowItestComplete){
			current = 4;
			Port2.lowItestComplete = true;
			randDACcurrent = round((current * 4096) / 27.7573);
			corrected_current = randDACcurrent + Port2.CalibrationFactor[I_4];
		} else {
			current = 17.5;
			randDACcurrent = round((current * 4096) / 27.7573);
			corrected_current = randDACcurrent + Port2.CalibrationFactor[I_175];
		}
		break;
	case 3:
		if(!Port3.lowItestComplete){
			current = 4;
			Port3.lowItestComplete = true;
			randDACcurrent = round((current * 4096) / 27.7573);
			corrected_current = randDACcurrent + Port3.CalibrationFactor[I_4];
		} else {
			current = 17.5;
			randDACcurrent = round((current * 4096) / 27.7573);
			corrected_current = randDACcurrent + Port3.CalibrationFactor[I_175];
		}
		break;
	case 4:
		if(!Port4.lowItestComplete){
			current = 4;
			Port4.lowItestComplete = true;
			randDACcurrent = round((current * 4096) / 27.7573);
			corrected_current = randDACcurrent + Port4.CalibrationFactor[I_4];
		} else {
			current = 17.5;
			randDACcurrent = round((current * 4096) / 27.7573);
			corrected_current = randDACcurrent + Port4.CalibrationFactor[I_175];
		}
		break;
	case 5:
		if(!Port5.lowItestComplete){
			current = 4;
			Port5.lowItestComplete = true;
			randDACcurrent = round((current * 4096) / 27.7573);
			corrected_current = randDACcurrent + Port5.CalibrationFactor[I_4];
		} else {
			current = 17.5;
			randDACcurrent = round((current * 4096) / 27.7573);
			corrected_current = randDACcurrent + Port5.CalibrationFactor[I_175];
		}
		break;
	case 6:
		if(!Port6.lowItestComplete){
			current = 4;
			Port6.lowItestComplete = true;
			randDACcurrent = round((current * 4096) / 27.7573);
			corrected_current = randDACcurrent + Port6.CalibrationFactor[I_4];
		} else {
			current = 17.5;
			randDACcurrent = round((current * 4096) / 27.7573);
			corrected_current = randDACcurrent + Port6.CalibrationFactor[I_175];
		}
		break;
	}

	if (Test_Port == 1 || Test_Port == 3 || Test_Port == 5)
		randDACcurrent += 0x3000;
	else if (Test_Port == 2 || Test_Port == 4 || Test_Port == 6)
		randDACcurrent += 0xB000;
	DAC_set(Test_Port, randDACcurrent);
	MUX_Sel(Test_Port, TWENTY_AMP);
	return current;
}
//	==================================================================================	//


//	=================================   Three Volt   =================================	//
float threeVolt(uint8 Test_Port, uint8 TestCode) {
	float voltage;
	float Corrected_voltage;
	int randDACvolt;
	switch (TestCode) {
		case ONE_VOLT:
				voltage = 0.5;
			break;
		case TWOFIVE_VOLT:
				voltage = 2.4;
			break;
		case THREE_VOLT:
				voltage = 2.5;
			break;
	}
	randDACvolt = round(((voltage/ 3.6864) * 4096));
	switch (Test_Port) {
	case 1:
		if (TestCode == ONE_VOLT) {
			Corrected_voltage = randDACvolt + Port1.CalibrationFactor[V_05];
		} else if (TestCode == TWOFIVE_VOLT) {
			Corrected_voltage = randDACvolt + Port1.CalibrationFactor[V_24];
		}
		break;
	case 2:
		if (TestCode == ONE_VOLT) {
			Corrected_voltage = randDACvolt + Port2.CalibrationFactor[V_05];
		} else if (TestCode == TWOFIVE_VOLT) {
			Corrected_voltage = randDACvolt + Port2.CalibrationFactor[V_24];
		}
		break;
	case 3:
		if (TestCode == ONE_VOLT) {
			Corrected_voltage = randDACvolt + Port3.CalibrationFactor[V_05];
		} else if (TestCode == TWOFIVE_VOLT) {
			Corrected_voltage = randDACvolt + Port3.CalibrationFactor[V_24];
		}
		break;
	case 4:
		if (TestCode == ONE_VOLT) {
			Corrected_voltage = randDACvolt + Port4.CalibrationFactor[V_05];
		} else if (TestCode == TWOFIVE_VOLT) {
			Corrected_voltage = randDACvolt + Port4.CalibrationFactor[V_24];
		}
		break;
	case 5:
		if (TestCode == ONE_VOLT) {
			Corrected_voltage = randDACvolt + Port5.CalibrationFactor[V_05];
		} else if (TestCode == TWOFIVE_VOLT) {
			Corrected_voltage = randDACvolt + Port5.CalibrationFactor[V_24];
		}
		break;
	case 6:
		if (TestCode == ONE_VOLT) {
			Corrected_voltage = randDACvolt + Port6.CalibrationFactor[V_05];
		} else if (TestCode == TWOFIVE_VOLT) {
			Corrected_voltage = randDACvolt + Port6.CalibrationFactor[V_24];
		}
		break;
	}

//	randDACvolt = round((Corrected_voltage * 4096 / 3.6864)); //round((voltage * 3448) / 3.014);
	if (Test_Port == 1 || Test_Port == 3 || Test_Port == 5)
		randDACvolt = 0x3000 + Corrected_voltage;
	else if (Test_Port == 2 || Test_Port == 4 || Test_Port == 6)
		randDACvolt = 0xB000 + Corrected_voltage;

	DAC_set(Test_Port, randDACvolt);
	MUX_Sel(Test_Port, THREE_VOLT);
	return voltage;
}
//	==================================================================================	//


//	===================================   ASYNC   ===================================	//
float asyncPulse(uint8 Test_Port) {
	switch (Test_Port) {
	//Port 2
	case 1:
		switch (GlobalTestNum) {
		case 1:
		case 2:
			Async_Port1.PulseCount = 12;
			break;
		case 3:
		case 4:
			Async_Port1.PulseCount = 4;
			break;
		case 5:
		case 6:
			Async_Port1.PulseCount = 6;
			break;
		}
		MUX_Sel(Test_Port, ASYNC_PULSE);
		Async_Port1.fcount = 5;
		Async_Port1.PulseState = true;
		return (float) Async_Port1.FilterEnabled ?  Async_Port1.PulseCount : (11*Async_Port1.PulseCount);
		break;
		//Port 3
	case 2:
		switch (GlobalTestNum) {
		case 1:
		case 2:
			Async_Port2.PulseCount = 5;
			break;
		case 3:
		case 4:
			Async_Port2.PulseCount = 5;
			break;
		case 5:
		case 6:
			Async_Port2.PulseCount = 10;
			break;
		}
		MUX_Sel(Test_Port, ASYNC_PULSE);
		Async_Port2.fcount = 5;
		Async_Port2.PulseState = true;
		return (float) Async_Port2.FilterEnabled ?  Async_Port2.PulseCount : (11*Async_Port2.PulseCount);
		break;
		//Port 4
	case 3:
		switch (GlobalTestNum) {
		case 1:
		case 2:
			Async_Port3.PulseCount = 8;
			break;
		case 3:
		case 4:
			Async_Port3.PulseCount = 10;
			break;
		case 5:
		case 6:
			Async_Port3.PulseCount = 7;
			break;
		}
		MUX_Sel(Test_Port, ASYNC_PULSE);
		Async_Port3.fcount = 5;
		Async_Port3.PulseState = true;
		return (float) Async_Port3.FilterEnabled ?  Async_Port3.PulseCount : (11*Async_Port3.PulseCount);
		break;
		//Port 5
	case 4:
		switch (GlobalTestNum) {
		case 1:
		case 2:
			Async_Port4.PulseCount = 10;
			break;
		case 3:
		case 4:
			Async_Port4.PulseCount = 8;
			break;
		case 5:
		case 6:
			Async_Port4.PulseCount = 12;
			break;
		}
		MUX_Sel(Test_Port, ASYNC_PULSE);
		Async_Port4.fcount = 5;
		Async_Port4.PulseState = true;
		return (float) Async_Port4.FilterEnabled ?  Async_Port4.PulseCount : (11*Async_Port4.PulseCount);
		break;
	case 5:
		switch (GlobalTestNum) {
		case 1:
		case 2:
			Async_Port5.PulseCount = 10;
			break;
		case 3:
		case 4:
			Async_Port5.PulseCount = 8;
			break;
		case 5:
		case 6:
			Async_Port5.PulseCount = 12;
			break;
		}
		MUX_Sel(Test_Port, ASYNC_PULSE);
		Async_Port5.fcount = 5;
		Async_Port5.PulseState = true;
		return (float) Async_Port5.FilterEnabled ?  Async_Port5.PulseCount : (11*Async_Port5.PulseCount);
			break;

	case 6:
		switch (GlobalTestNum) {
		case 1:
		case 2:
			Async_Port6.PulseCount = 10;
			break;
		case 3:
		case 4:
			Async_Port6.PulseCount = 8;
			break;
		case 5:
		case 6:
			Async_Port6.PulseCount = 12;
			break;
		}
		MUX_Sel(Test_Port, ASYNC_PULSE);
		Async_Port6.fcount = 5;
		Async_Port6.PulseState = true;
		return (float) Async_Port6.FilterEnabled ?  Async_Port6.PulseCount : (11*Async_Port6.PulseCount);
		break;

	case 7:
		switch (GlobalTestNum) {
		case 1:
		case 2:
			Async_Port7.PulseCount = 5;
			break;
		case 3:
		case 4:
			Async_Port7.PulseCount = 5;
			break;
		case 5:
		case 6:
			Async_Port7.PulseCount = 5;
			break;
		}
		MUX_Sel(Test_Port, ASYNC_PULSE);
		Async_Port7.fcount = 5;
		Async_Port7.PulseState = true;
		return (float) Async_Port7.PulseCount;
		break;

	case 8:
		switch (GlobalTestNum) {
		case 1:
		case 2:
			Async_Port8.PulseCount = 7;
			break;
		case 3:
		case 4:
			Async_Port8.PulseCount = 7;
			break;
		case 5:
		case 6:
			Async_Port8.PulseCount = 7;
			break;
		}
		MUX_Sel(Test_Port, ASYNC_PULSE);
		Async_Port8.fcount = 5;
		Async_Port8.PulseState = true;
		return (float) Async_Port8.PulseCount;
		break;
	case 9:
		switch (GlobalTestNum) {
		case 1:
		case 2:
			Async_Port9.PulseCount = 7;
			break;
		case 3:
		case 4:
			Async_Port9.PulseCount = 7;
			break;
		case 5:
		case 6:
			Async_Port9.PulseCount = 7;
			break;
		}
		MUX_Sel(Test_Port, ASYNC_PULSE);
		Async_Port9.fcount = 5;
		Async_Port9.PulseState = true;
		return (float) Async_Port9.PulseCount;
		break;
	}
	return 0;
}
//	===================================================================================	//


//	===================================   SDI-12    ===================================	//
float sdiTwelve(uint8 Test_Port){
	MUX_Sel(Test_Port, SDI_TWELVE);
	SDSstate = SDSundef;
	switch (Test_Port) {
	case 1:
		SDI_Port1.Enabled = true;
		SDI_Port1.Address = Test_Port + 0x30;
		SDI_Port1.setValue = 7.0644;
		return SDI_Port1.setValue;
		break;
	case 2:
		SDI_Port2.Enabled = true;
		SDI_Port2.Address = Test_Port + 0x30;
		SDI_Port2.setValue = 9.5447;
		return SDI_Port2.setValue;
		break;
	case 3:
		SDI_Port3.Enabled = true;
		SDI_Port3.Address = Test_Port + 0x30;
		SDI_Port3.setValue = 4.4085;
		return SDI_Port3.setValue;
		break;
	case 4:
		SDI_Port4.Enabled = true;
		SDI_Port4.Address = Test_Port + 0x30;
		SDI_Port4.setValue = 6.5153;
		return SDI_Port4.setValue;
		break;
	case 5:
		SDI_Port5.Enabled = true;
		SDI_Port5.Address = Test_Port + 0x30;
		SDI_Port5.setValue = 5.8921;
		return SDI_Port5.setValue;
		break;
	case 6:
		SDI_Port6.Enabled = true;
		SDI_Port6.Address = Test_Port + 0x30;
		SDI_Port6.setValue = 6.9221;
		return SDI_Port6.setValue;
		break;
	}

}
//	===================================================================================	//


//	===================================    MUX    ===================================	//
void MUX_Sel(uint8 Test_Port, uint8 Test) {
	switch (Test_Port) {
	case 1:
		HAL_GPIO_WritePin(MUX_WRodd1_GPIO_Port, MUX_WRodd1_Pin, GPIO_PIN_RESET);
		delay_us(7);
		break;
	case 2:
		HAL_GPIO_WritePin(MUX_WReven1_GPIO_Port, MUX_WReven1_Pin, GPIO_PIN_RESET);
		delay_us(7);
		break;
	case 3:
		HAL_GPIO_WritePin(MUX_WRodd2_GPIO_Port, MUX_WRodd2_Pin, GPIO_PIN_RESET);
		delay_us(5);
		break;
	case 4:
		HAL_GPIO_WritePin(MUX_WReven2_GPIO_Port, MUX_WReven2_Pin, GPIO_PIN_RESET);
		delay_us(5);
		break;
	case 5:
		HAL_GPIO_WritePin(MUX_WRodd3_GPIO_Port, MUX_WRodd3_Pin, GPIO_PIN_RESET);
		delay_us(5);
		break;
	case 6:
		HAL_GPIO_WritePin(MUX_WReven3_GPIO_Port, MUX_WReven3_Pin, GPIO_PIN_RESET);
		delay_us(5);
		break;
	}
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
	switch (Test_Port) {
	case 1:
		delay_us(7);
		HAL_GPIO_WritePin(MUX_WRodd1_GPIO_Port, MUX_WRodd1_Pin, GPIO_PIN_SET);
		break;
	case 2:
		delay_us(7);
		HAL_GPIO_WritePin(MUX_WReven1_GPIO_Port, MUX_WReven1_Pin, GPIO_PIN_SET);
		break;
	case 3:
		delay_us(5);
		HAL_GPIO_WritePin(MUX_WRodd2_GPIO_Port, MUX_WRodd2_Pin, GPIO_PIN_SET);
		break;
	case 4:
		delay_us(5);
		HAL_GPIO_WritePin(MUX_WReven2_GPIO_Port, MUX_WReven2_Pin, GPIO_PIN_SET);
		break;
	case 5:
		delay_us(5);
		HAL_GPIO_WritePin(MUX_WRodd3_GPIO_Port, MUX_WRodd3_Pin, GPIO_PIN_SET);
		break;
	case 6:
		delay_us(5);
		HAL_GPIO_WritePin(MUX_WReven3_GPIO_Port, MUX_WReven3_Pin, GPIO_PIN_SET);
		break;
	}
}
void reset_ALL_MUX() {
	//MUX 1
	HAL_GPIO_WritePin(MUX_WRodd1_GPIO_Port, MUX_WRodd1_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(MUX_A0_GPIO_Port, MUX_A0_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(MUX_A1_GPIO_Port, MUX_A1_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(MUX_WRodd1_GPIO_Port, MUX_WRodd1_Pin, GPIO_PIN_SET);
	//MUX 2
	HAL_GPIO_WritePin(MUX_WReven1_GPIO_Port, MUX_WReven1_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(MUX_A0_GPIO_Port, MUX_A0_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(MUX_A1_GPIO_Port, MUX_A1_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(MUX_WReven1_GPIO_Port, MUX_WReven1_Pin, GPIO_PIN_SET);
	//MUX 3
	HAL_GPIO_WritePin(MUX_WRodd2_GPIO_Port, MUX_WRodd2_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(MUX_A0_GPIO_Port, MUX_A0_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(MUX_A1_GPIO_Port, MUX_A1_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(MUX_WRodd2_GPIO_Port, MUX_WRodd2_Pin, GPIO_PIN_SET);
	//MUX 4
	HAL_GPIO_WritePin(MUX_WReven2_GPIO_Port, MUX_WReven2_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(MUX_A0_GPIO_Port, MUX_A0_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(MUX_A1_GPIO_Port, MUX_A1_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(MUX_WReven2_GPIO_Port, MUX_WReven2_Pin, GPIO_PIN_SET);
	//MUX 5
	HAL_GPIO_WritePin(MUX_WRodd3_GPIO_Port, MUX_WRodd3_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(MUX_A0_GPIO_Port, MUX_A0_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(MUX_A1_GPIO_Port, MUX_A1_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(MUX_WRodd3_GPIO_Port, MUX_WRodd3_Pin, GPIO_PIN_SET);
	//MUX 6
	HAL_GPIO_WritePin(MUX_WReven3_GPIO_Port, MUX_WReven3_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(MUX_A0_GPIO_Port, MUX_A0_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(MUX_A1_GPIO_Port, MUX_A1_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(MUX_WReven3_GPIO_Port, MUX_WReven3_Pin, GPIO_PIN_SET);
}
//	=================================================================================//


//	=================================    DAC    =====================================//
void DAC_set(uint8 Test_Port, int DACvalue) {
	switch (Test_Port) {
	case 1:
	case 2:
		HAL_GPIO_WritePin(DAC_CS1_GPIO_Port, DAC_CS1_Pin, GPIO_PIN_RESET);
		HAL_SPI_Transmit(&DAC_SPI, &DACvalue, 2, 50);
		HAL_GPIO_WritePin(DAC_CS1_GPIO_Port, DAC_CS1_Pin, GPIO_PIN_SET);
		break;
	case 3:
	case 4:
		HAL_GPIO_WritePin(DAC_CS2_GPIO_Port, DAC_CS2_Pin, GPIO_PIN_RESET);
		HAL_SPI_Transmit(&DAC_SPI, &DACvalue, 2, 50);
		HAL_GPIO_WritePin(DAC_CS2_GPIO_Port, DAC_CS2_Pin, GPIO_PIN_SET);
		break;
	case 5:
	case 6:
		HAL_GPIO_WritePin(DAC_CS3_GPIO_Port, DAC_CS3_Pin, GPIO_PIN_RESET);
		HAL_SPI_Transmit(&DAC_SPI, &DACvalue, 2, 50);
		HAL_GPIO_WritePin(DAC_CS3_GPIO_Port, DAC_CS3_Pin, GPIO_PIN_SET);
		break;
	}
}
void reset_ALL_DAC() {
	int DACvalue;
	//DAC 1 Clear
	DACvalue = 0x3000;	//OUTA Clear
	HAL_GPIO_WritePin(DAC_CS1_GPIO_Port, DAC_CS1_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&DAC_SPI, &DACvalue, 2, 100);
	HAL_GPIO_WritePin(DAC_CS1_GPIO_Port, DAC_CS1_Pin, GPIO_PIN_SET);
	DACvalue = 0x9000;	//OUTB Clear
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
//	=================================================================================//


//	=================================    ADC    =====================================//
void ADC_Init(){
	LatchSampling = true;
	stableVoltageCount = 10;

	LatchErrCheck = 0;
	LatchCountTimer = 0;
	LatchOnComplete = false;
	LatchOffComplete = false;
		//Low Voltage Reset
	Vfuse.lowVoltage = 4096;
	Vin.lowVoltage = 4096;
	Vmos1.lowVoltage = 4096;
	Vmos2.lowVoltage = 4096;

		//High Voltage Reset
	Vfuse.highVoltage = 0;
	Vin.highVoltage = 0;
	Vmos1.highVoltage = 0;
	Vmos2.highVoltage = 0;

		//ADC1 Reset
	adc1.HighPulseWidth = 0;
	adc1.LowPulseWidth = 0;
	adc2.HighPulseWidth = 0;
	adc2.LowPulseWidth = 0;

		//ADC2 Reset
	adc1.highVoltage = 0;
	adc1.lowVoltage = 0;
	adc2.highVoltage = 0;
	adc2.lowVoltage = 0;
}
void ADC_MUXsel(uint8 ADCport){
	switch(ADCport){
	case 0:
		HAL_GPIO_WritePin(ADC_MUX_A_GPIO_Port, ADC_MUX_A_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(ADC_MUX_B_GPIO_Port, ADC_MUX_B_Pin, GPIO_PIN_RESET);
		break;
	case 1:
		HAL_GPIO_WritePin(ADC_MUX_A_GPIO_Port, ADC_MUX_A_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ADC_MUX_B_GPIO_Port, ADC_MUX_B_Pin, GPIO_PIN_RESET);
		break;
	case 2:
		HAL_GPIO_WritePin(ADC_MUX_A_GPIO_Port, ADC_MUX_A_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(ADC_MUX_B_GPIO_Port, ADC_MUX_B_Pin, GPIO_PIN_SET);
		break;
	case 3:
		HAL_GPIO_WritePin(ADC_MUX_A_GPIO_Port, ADC_MUX_A_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ADC_MUX_B_GPIO_Port, ADC_MUX_B_Pin, GPIO_PIN_SET);
		break;
	}
}
//	=================================================================================//


//	============================    Print Results    ================================//
void PrintLatchResults(){
	sprintf(Buffer, "\n\n =======================  Latch Test  =======================\n\n");
	CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
	sprintf(Buffer, "\n==============   Port A Latch time:   High: %d Low: %d   ==============\n", adc1.HighPulseWidth, adc1.LowPulseWidth);
	CDC_Transmit_FS(&Buffer[0], strlen(Buffer));


		//Port A Voltage
	adc1.highVoltage = adc1.HighPulseWidth > 0 ? adc1.highVoltage/adc1.HighPulseWidth:0;
	adc1.highVoltage *= (16.17/4096);
	adc1.lowVoltage /= adc1.LowPulseWidth;
	adc1.lowVoltage *= (16.17/4096);
	sprintf(Buffer, "\n==============   Port A Voltage:   High: %f Low: %f   ==============\n\n", adc1.highVoltage, adc1.lowVoltage);
	CDC_Transmit_FS(&Buffer[0], strlen(Buffer));

		//Port B Latch Time
	sprintf(Buffer, "==============   Port B Latch time:   High: %d  Low: %d   ==============\n", adc2.HighPulseWidth, adc2.LowPulseWidth);
	CDC_Transmit_FS(&Buffer[0], strlen(Buffer));

		//Port B Voltage
	adc2.highVoltage = adc2.HighPulseWidth > 0 ? adc2.highVoltage/adc2.HighPulseWidth:0;
	adc2.highVoltage *= (16.17/4096);
	adc2.lowVoltage /= adc2.LowPulseWidth;
	adc2.lowVoltage *= (16.17/4096);
	sprintf(Buffer, "\n==============   Port B Voltage:   High: %f Low: %f   ==============\n\n", adc2.highVoltage, adc2.lowVoltage);
	CDC_Transmit_FS(&Buffer[0], strlen(Buffer));

		//Vin Voltage
	Vin.lowVoltage *= (16.17/4096);
	sprintf(Buffer, "\n==============   Vin Voltage:   AVG: %f Min: %f   ==============\n", Vin.steadyState, Vin.lowVoltage);
	CDC_Transmit_FS(&Buffer[0], strlen(Buffer));

		//Fuse Voltage
	Vfuse.lowVoltage *= (16.17/4096);
	sprintf(Buffer, "\n==============   Fuse Voltage:   AVG: %f Min: %f   ==============\n", Vfuse.steadyState, Vfuse.lowVoltage);
	CDC_Transmit_FS(&Buffer[0], strlen(Buffer));

		//MOSFET Voltage
	Vmos1.lowVoltage *= (16.17/4096);
	Vmos1.highVoltage *= (16.17/4096);
	sprintf(Buffer, "\n==============   MOSFET 1 Voltage:   High: %f Low: %f   ==============\n", Vmos1.highVoltage, Vmos1.lowVoltage);
	CDC_Transmit_FS(&Buffer[0], strlen(Buffer));

	Vmos2.lowVoltage *= (16.17/4096);
	Vmos2.highVoltage *= (16.17/4096);
	sprintf(Buffer, "\n==============   MOSFET 2 Voltage:   High: %f Low: %f   ==============\n", Vmos2.highVoltage, Vmos2.lowVoltage);
	CDC_Transmit_FS(&Buffer[0], strlen(Buffer));


	LatchCurrent.lowVoltage /= 132;
	LatchCurrent.highVoltage /= 132;
	LatchCurrent.lowVoltage *= (16.17/4096);
	LatchCurrent.highVoltage *= (16.17/4096);
	sprintf(Buffer, "\n==============   Lactch Current:   Pulse 1: %f Pulse 2: %f   ==============\n", LatchCurrent.highVoltage, LatchCurrent.lowVoltage);
	CDC_Transmit_FS(&Buffer[0], strlen(Buffer));

	//Store Values
	LatchRes.tOn = (adc1.HighPulseWidth >= adc2.LowPulseWidth) ? adc1.HighPulseWidth:adc2.LowPulseWidth;
	LatchRes.tOff = (adc2.HighPulseWidth >= adc1.LowPulseWidth) ? adc2.HighPulseWidth:adc1.LowPulseWidth;

	LatchRes.PortAhighVoltage = adc1.highVoltage;
	LatchRes.PortAlowVoltage = adc1.lowVoltage;

	LatchRes.PortBhighVoltage = adc2.highVoltage;
	LatchRes.PortBlowVoltage = adc2.lowVoltage;

	LatchRes.InAvgVoltage = Vin.steadyState;
	LatchRes.InLowVoltage = Vin.lowVoltage;
	LatchRes.FuseAvgVoltage = Vfuse.steadyState;
	LatchRes.FuseLowVoltage = Vfuse.lowVoltage;

	LatchRes.MOSonHigh = Vmos1.highVoltage;
	LatchRes.MOSonLow = Vmos2.lowVoltage;

	LatchRes.MOSoffHigh = Vmos2.highVoltage;
	LatchRes.MOSoffLow = Vmos1.lowVoltage;
}

void TransmitResults(){
		sprintf(Buffer, "==============   ADC Average Results   ==============");
		CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
		for(int i = 0;i<LatchCountTimer;i++)
		{
			sprintf(Buffer, "%d,%d,%d,%d\n",i,adc1.avg_Buffer[i],adc2.avg_Buffer[i],Vfuse.avg_Buffer[i]);
			CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
		}
}
//	=================================================================================//
//	=================================================================================//
//	=================================================================================//
