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

int twoWireLatching(uint8,bool);
float twentyAmp(uint8);
float threeVolt(uint8, uint8);
float asyncPulse(uint8);
float sdiTwelve(uint8);
//void DAC_set(uint8, int);

void MUX_Sel(uint8, uint8);


extern void delay_us(int);
extern double round(double);

void TestFunction(TboardConfig *Board) {
	HAL_GPIO_WritePin(MUX_RS_GPIO_Port, MUX_RS_Pin, GPIO_PIN_SET);
	uint8 totalPortCount = 0;
	uint8 currPort;
	//====================== Analog Test Count ======================//
	for (currPort = 0; currPort < Board->latchPortCount; currPort++) {
		switch (Board->TestCode[totalPortCount]) {
				case TWO_WIRE_LATCHING:
					CHval[Board->GlobalTestNum][totalPortCount++] = twoWireLatching(currPort,1);
					break;
				case NOTEST:
					CHval[Board->GlobalTestNum][totalPortCount++] = twoWireLatching(currPort,0);
					break;
		}
	}
	//====================== Analog Test Count ======================//
	for (currPort = 0; currPort < Board->analogInputCount; currPort++) {
		switch (Board->TestCode[totalPortCount]) {
		case TWENTY_AMP:
			CHval[Board->GlobalTestNum][totalPortCount++] = twentyAmp(currPort);
//			MUX_Sel(currPort, Board->TestCode[totalPortCount]);
			break;
		case THREE_VOLT:
			CHval[Board->GlobalTestNum][totalPortCount++] = threeVolt(currPort, THREE_VOLT);
			break;
		case TWOFIVE_VOLT:
			CHval[Board->GlobalTestNum][totalPortCount++] = threeVolt(currPort, TWOFIVE_VOLT);
			break;
		case ONE_VOLT:
			CHval[Board->GlobalTestNum][totalPortCount++] = threeVolt(currPort, ONE_VOLT);
			break;
		case ASYNC_PULSE:
			CHval[Board->GlobalTestNum][totalPortCount++] = asyncPulse(currPort);
			break;
		case SDI_TWELVE:
			CHval[Board->GlobalTestNum][totalPortCount++] = sdiTwelve(currPort);
			break;
		}
	}
	//====================== Digital Test Count ======================//
	for (currPort = 7; currPort < Board->digitalInputCout+7; currPort++) {
		switch (Board->TestCode[totalPortCount]) {
		case ASYNC_PULSE:
			CHval[Board->GlobalTestNum][totalPortCount++] = asyncPulse(currPort);
			MUX_Sel(currPort, Board->TestCode[totalPortCount]);
			break;
		}
	}
}
//	==================================================================================	//


//	=================================   Two Wire   =================================	//
int twoWireLatching(uint8 Test_Port,_Bool state) {
		switch(Test_Port){
		case Port_1:
			LatchPort1 = (Test_Port == Port_1) && state ? true : false;
			break;
		case Port_2:
			LatchPort2 = (Test_Port == Port_2) && state ? true : false;
			break;
		case Port_3:
			LatchPort3 = (Test_Port == Port_3) && state ? true : false;
			break;
		case Port_4:
			LatchPort4 = (Test_Port == Port_4) && state ? true : false;
			break;
		}
	return state;
}
//	==================================================================================	//


//	================================   20mA Current   ================================	//
float twentyAmp(uint8 Test_Port) {
	float current;
	float corrected_current;
	uint16 DAC_Value = 0;
	//Correction Factor
	switch (Test_Port) {
		case Port_1:
			if(!Port1.lowItestComplete){
				current = 4;
				Port1.lowItestComplete = true;
				DAC_Value = DAC_4amp;
				corrected_current = DAC_Value + Port1.CalibrationFactor[I_4];
			} else {
				current = 17.5;
				DAC_Value = DAC_175amp;
				corrected_current = DAC_Value + Port1.CalibrationFactor[I_175];
			}
			break;
		case Port_2:
			if(!Port2.lowItestComplete){
				current = 4;
				Port2.lowItestComplete = true;
				DAC_Value = DAC_4amp;
				corrected_current = DAC_Value + Port2.CalibrationFactor[I_4];
			} else {
				current = 17.5;
				DAC_Value = DAC_175amp;
				corrected_current = DAC_Value + Port2.CalibrationFactor[I_175];
			}
			break;
		case Port_3:
			if(!Port3.lowItestComplete){
				current = 4;
				Port3.lowItestComplete = true;
				DAC_Value = DAC_4amp;
				corrected_current = DAC_Value + Port3.CalibrationFactor[I_4];
			} else {
				current = 17.5;
				DAC_Value = DAC_175amp;
				corrected_current = DAC_Value + Port3.CalibrationFactor[I_175];
			}
			break;
		case Port_4:
			if(!Port4.lowItestComplete){
				current = 4;
				Port4.lowItestComplete = true;
				DAC_Value = DAC_4amp;
				corrected_current = DAC_Value + Port4.CalibrationFactor[I_4];
			} else {
				current = 17.5;
				DAC_Value = DAC_175amp;
				corrected_current = DAC_Value + Port4.CalibrationFactor[I_175];
			}
			break;
		case Port_5:
			if(!Port5.lowItestComplete){
				current = 4;
				Port5.lowItestComplete = true;
				DAC_Value = DAC_4amp;
				corrected_current = DAC_Value + Port5.CalibrationFactor[I_4];
			} else {
				current = 17.5;
				DAC_Value = DAC_175amp;
				corrected_current = DAC_Value + Port5.CalibrationFactor[I_175];
			}
			break;
		case Port_6:
			if(!Port6.lowItestComplete){
				current = 4;
				Port6.lowItestComplete = true;
				DAC_Value = DAC_4amp;
				corrected_current = DAC_Value + Port6.CalibrationFactor[I_4];
			} else {
				current = 17.5;
				DAC_Value = DAC_175amp;
				corrected_current = DAC_Value + Port6.CalibrationFactor[I_175];
			}
			break;
		}

	if (Test_Port == Port_1 || Test_Port == Port_3 || Test_Port == Port_5)
		DAC_Value += 0x3000;
	else if (Test_Port == Port_2 || Test_Port == Port_4 || Test_Port == Port_6)
		DAC_Value += 0xB000;
	DAC_set(Test_Port, DAC_Value);
	MUX_Sel(Test_Port, TWENTY_AMP);
	return current;
}
//	==================================================================================	//


//	=================================   Three Volt   =================================	//
float threeVolt(uint8 Test_Port, uint8 TestCode) {
	float voltage;
	uint16 Corrected_DACvalue;
	uint16 DAC_Value;
	switch (TestCode) {
		case ONE_VOLT:
				voltage = 0.5;
				DAC_Value = DAC_05volt;
			break;
		case TWOFIVE_VOLT:
				voltage = 2.4;
				DAC_Value = DAC_24volt;
			break;
	}
	switch (Test_Port) {
	case Port_1:
		if (TestCode == ONE_VOLT) {
			Corrected_DACvalue = DAC_Value + Port1.CalibrationFactor[V_05];
		} else if (TestCode == TWOFIVE_VOLT) {
			Corrected_DACvalue = DAC_Value + Port1.CalibrationFactor[V_24];
		}
		break;
	case Port_2:
		if (TestCode == ONE_VOLT) {
			Corrected_DACvalue = DAC_Value + Port2.CalibrationFactor[V_05];
		} else if (TestCode == TWOFIVE_VOLT) {
			Corrected_DACvalue = DAC_Value + Port2.CalibrationFactor[V_24];
		}
		break;
	case Port_3:
		if (TestCode == ONE_VOLT) {
			Corrected_DACvalue = DAC_Value + Port3.CalibrationFactor[V_05];
		} else if (TestCode == TWOFIVE_VOLT) {
			Corrected_DACvalue = DAC_Value + Port3.CalibrationFactor[V_24];
		}
		break;
	case Port_4:
		if (TestCode == ONE_VOLT) {
			Corrected_DACvalue = DAC_Value + Port4.CalibrationFactor[V_05];
		} else if (TestCode == TWOFIVE_VOLT) {
			Corrected_DACvalue = DAC_Value + Port4.CalibrationFactor[V_24];
		}
		break;
	case Port_5:
		if (TestCode == ONE_VOLT) {
			Corrected_DACvalue = DAC_Value + Port5.CalibrationFactor[V_05];
		} else if (TestCode == TWOFIVE_VOLT) {
			Corrected_DACvalue = DAC_Value + Port5.CalibrationFactor[V_24];
		}
		break;
	case Port_6:
		if (TestCode == ONE_VOLT) {
			Corrected_DACvalue = DAC_Value + Port6.CalibrationFactor[V_05];
		} else if (TestCode == TWOFIVE_VOLT) {
			Corrected_DACvalue = DAC_Value + Port6.CalibrationFactor[V_24];
		}
		break;
	}

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
float asyncPulse(uint8 Test_Port) {
	switch (Test_Port) {
	//Port 2
	case Port_1:
		switch (BoardConnected.GlobalTestNum) {
		case 0:
		case 1:
			Async_Port1.PulseCount = 12;
			break;
		case 2:
		case 3:
			Async_Port1.PulseCount = 12;
			break;
		case 4:
		case 5:
			Async_Port1.PulseCount = 6;
			break;
		}
		MUX_Sel(Test_Port, ASYNC_PULSE);
		Async_Port1.fcount = 5;
		Async_Port1.PulseState = true;
		return (float) Async_Port1.FilterEnabled ?  Async_Port1.PulseCount : (11*Async_Port1.PulseCount);
		break;
		//Port 3
	case Port_2:
		switch (BoardConnected.GlobalTestNum) {
		case 0:
		case 1:
			Async_Port2.PulseCount = 5;
			break;
		case 2:
		case 3:
			Async_Port2.PulseCount = 5;
			break;
		case 4:
		case 5:
			Async_Port2.PulseCount = 10;
			break;
		}
		MUX_Sel(Test_Port, ASYNC_PULSE);
		Async_Port2.fcount = 5;
		Async_Port2.PulseState = true;
		return (float) Async_Port2.FilterEnabled ?  Async_Port2.PulseCount : (11*Async_Port2.PulseCount);
		break;
		//Port 4
	case Port_3:
		switch (BoardConnected.GlobalTestNum) {
		case 0:
		case 1:
			Async_Port3.PulseCount = 8;
			break;
		case 2:
		case 3:
			Async_Port3.PulseCount = 10;
			break;
		case 4:
		case 5:
			Async_Port3.PulseCount = 7;
			break;
		}
		MUX_Sel(Test_Port, ASYNC_PULSE);
		Async_Port3.fcount = 5;
		Async_Port3.PulseState = true;
		return (float) Async_Port3.FilterEnabled ?  Async_Port3.PulseCount : (11*Async_Port3.PulseCount);
		break;
		//Port 5
	case Port_4:
		switch (BoardConnected.GlobalTestNum) {
		case 0:
		case 1:
			Async_Port4.PulseCount = 10;
			break;
		case 2:
		case 3:
			Async_Port4.PulseCount = 8;
			break;
		case 4:
		case 5:
			Async_Port4.PulseCount = 12;
			break;
		}
		MUX_Sel(Test_Port, ASYNC_PULSE);
		Async_Port4.fcount = 5;
		Async_Port4.PulseState = true;
		return (float) Async_Port4.FilterEnabled ?  Async_Port4.PulseCount : (11*Async_Port4.PulseCount);
		break;
	case Port_5:
		switch (BoardConnected.GlobalTestNum) {
		case 0:
		case 1:
			Async_Port5.PulseCount = 10;
			break;
		case 2:
		case 3:
			Async_Port5.PulseCount = 8;
			break;
		case 4:
		case 5:
			Async_Port5.PulseCount = 12;
			break;
		}
		MUX_Sel(Test_Port, ASYNC_PULSE);
		Async_Port5.fcount = 5;
		Async_Port5.PulseState = true;
		return (float) Async_Port5.FilterEnabled ?  Async_Port5.PulseCount : (11*Async_Port5.PulseCount);
			break;

	case Port_6:
		switch (BoardConnected.GlobalTestNum) {
		case 0:
		case 1:
			Async_Port6.PulseCount = 10;
			break;
		case 2:
		case 3:
			Async_Port6.PulseCount = 8;
			break;
		case 4:
		case 5:
			Async_Port6.PulseCount = 12;
			break;
		}
		MUX_Sel(Test_Port, ASYNC_PULSE);
		Async_Port6.fcount = 5;
		Async_Port6.PulseState = true;
		return (float) Async_Port6.FilterEnabled ?  Async_Port6.PulseCount : (11*Async_Port6.PulseCount);
		break;

	case 6:
		switch (BoardConnected.GlobalTestNum) {
		case 0:
		case 1:
			Async_Port7.PulseCount = 5;
			break;
		case 2:
		case 3:
			Async_Port7.PulseCount = 5;
			break;
		case 4:
		case 5:
			Async_Port7.PulseCount = 5;
			break;
		}
		MUX_Sel(Test_Port, ASYNC_PULSE);
		Async_Port7.fcount = 5;
		Async_Port7.PulseState = true;
		return (float) Async_Port7.PulseCount;
		break;

	case 7:
		switch (BoardConnected.GlobalTestNum) {
		case 0:
		case 1:
			Async_Port8.PulseCount = 7;
			break;
		case 2:
		case 3:
			Async_Port8.PulseCount = 7;
			break;
		case 4:
		case 5:
			Async_Port8.PulseCount = 7;
			break;
		}
		MUX_Sel(Test_Port, ASYNC_PULSE);
		Async_Port8.fcount = 5;
		Async_Port8.PulseState = true;
		return (float) Async_Port8.PulseCount;
		break;
	case 8:
		switch (BoardConnected.GlobalTestNum) {
		case 0:
		case 1:
			Async_Port9.PulseCount = 7;
			break;
		case 2:
		case 3:
			Async_Port9.PulseCount = 7;
			break;
		case 4:
		case 5:
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
	case Port_1:
		SDI_Port1.Enabled = true;
		SDI_Port1.Address = Test_Port;
		SDI_Port1.setValue = 7.064;
		return SDI_Port1.setValue;
		break;
	case Port_2:
		SDI_Port2.Enabled = true;
		SDI_Port2.Address = Test_Port;
		SDI_Port2.setValue = 9.544;
		return SDI_Port2.setValue;
		break;
	case Port_3:
		SDI_Port3.Enabled = true;
		SDI_Port3.Address = Test_Port;
		SDI_Port3.setValue = 4.408;
		return SDI_Port3.setValue;
		break;
	case Port_4:
		SDI_Port4.Enabled = true;
		SDI_Port4.Address = Test_Port;
		SDI_Port4.setValue = 6.515;
		return SDI_Port4.setValue;
		break;
	case Port_5:
		SDI_Port5.Enabled = true;
		SDI_Port5.Address = Test_Port;
		SDI_Port5.setValue = 5.892;
		return SDI_Port5.setValue;
		break;
	case Port_6:
		SDI_Port6.Enabled = true;
		SDI_Port6.Address = Test_Port;
		SDI_Port6.setValue = 6.922;
		return SDI_Port6.setValue;
		break;
	}

}
//	===================================================================================	//


//	===================================    MUX    ===================================	//
void MUX_Sel(uint8 Test_Port, uint8 Test) {
	switch (Test_Port) {
	case Port_1:
		HAL_GPIO_WritePin(MUX_WRodd1_GPIO_Port, MUX_WRodd1_Pin, GPIO_PIN_RESET);
		delay_us(7);
		break;
	case Port_2:
		HAL_GPIO_WritePin(MUX_WReven1_GPIO_Port, MUX_WReven1_Pin, GPIO_PIN_RESET);
		delay_us(7);
		break;
	case Port_3:
		HAL_GPIO_WritePin(MUX_WRodd2_GPIO_Port, MUX_WRodd2_Pin, GPIO_PIN_RESET);
		delay_us(5);
		break;
	case Port_4:
		HAL_GPIO_WritePin(MUX_WReven2_GPIO_Port, MUX_WReven2_Pin, GPIO_PIN_RESET);
		delay_us(5);
		break;
	case Port_5:
		HAL_GPIO_WritePin(MUX_WRodd3_GPIO_Port, MUX_WRodd3_Pin, GPIO_PIN_RESET);
		delay_us(5);
		break;
	case Port_6:
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
	case Port_1:
		delay_us(7);
		HAL_GPIO_WritePin(MUX_WRodd1_GPIO_Port, MUX_WRodd1_Pin, GPIO_PIN_SET);
		break;
	case Port_2:
		delay_us(7);
		HAL_GPIO_WritePin(MUX_WReven1_GPIO_Port, MUX_WReven1_Pin, GPIO_PIN_SET);
		break;
	case Port_3:
		delay_us(5);
		HAL_GPIO_WritePin(MUX_WRodd2_GPIO_Port, MUX_WRodd2_Pin, GPIO_PIN_SET);
		break;
	case Port_4:
		delay_us(5);
		HAL_GPIO_WritePin(MUX_WReven2_GPIO_Port, MUX_WReven2_Pin, GPIO_PIN_SET);
		break;
	case Port_5:
		delay_us(5);
		HAL_GPIO_WritePin(MUX_WRodd3_GPIO_Port, MUX_WRodd3_Pin, GPIO_PIN_SET);
		break;
	case Port_6:
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



//	=================================================================================//


//	=================================    ADC    =====================================//
void ADC_Init(){
	CLEAR_REG(LatchTestStatusRegister);
	stableVoltageCount = 10;
	LatchCountTimer = 0;
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

	Vin.average = 0;
	Vin.steadyState = 0;
	Vin.total = 0;
	Vfuse.average = 0;
	Vfuse.steadyState = 0;
	Vfuse.total = 0;
	SET_BIT(LatchTestStatusRegister, LATCH_SAMPLING);
}
void ADC_MUXsel(uint8 ADCport){
	switch(ADCport){
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
void PrintLatchResults(){
	float LatchCurrent1;
	float LatchCurrent2;
	sprintf(debugTransmitBuffer, "\n\n =======================  Latch Test  =======================\n\n");
	CDC_Transmit_FS(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
	  HAL_UART_Transmit(&huart1, &debugTransmitBuffer[0], strlen(debugTransmitBuffer), HAL_MAX_DELAY);
	sprintf(debugTransmitBuffer, "\n==============   Port A Latch time:   High: %d Low: %d   ==============\n", adc1.HighPulseWidth, adc1.LowPulseWidth);
	CDC_Transmit_FS(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
	  HAL_UART_Transmit(&huart1, &debugTransmitBuffer[0], strlen(debugTransmitBuffer), HAL_MAX_DELAY);


		//Port A Voltage
	adc1.highVoltage = adc1.HighPulseWidth > 0 ? adc1.highVoltage/adc1.HighPulseWidth:0;
	adc1.highVoltage *= (ADC_MAX_INPUT_VOLTAGE/ADC_RESOLUTION); //Previous value was 16.17 when resistor is set to 150
	adc1.lowVoltage /= adc1.LowPulseWidth;
	adc1.lowVoltage *= (ADC_MAX_INPUT_VOLTAGE/ADC_RESOLUTION);
	sprintf(debugTransmitBuffer, "\n==============   Port A Voltage:   High: %f Low: %f   ==============\n\n", adc1.highVoltage, adc1.lowVoltage);
	CDC_Transmit_FS(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
	  HAL_UART_Transmit(&huart1, &debugTransmitBuffer[0], strlen(debugTransmitBuffer), HAL_MAX_DELAY);

		//Port B Latch Time
	sprintf(debugTransmitBuffer, "==============   Port B Latch time:   High: %d  Low: %d   ==============\n", adc2.HighPulseWidth, adc2.LowPulseWidth);
	CDC_Transmit_FS(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
	  HAL_UART_Transmit(&huart1, &debugTransmitBuffer[0], strlen(debugTransmitBuffer), HAL_MAX_DELAY);

		//Port B Voltage
	adc2.highVoltage = adc2.HighPulseWidth > 0 ? adc2.highVoltage/adc2.HighPulseWidth:0;
	adc2.highVoltage *= (ADC_MAX_INPUT_VOLTAGE/ADC_RESOLUTION);
	adc2.lowVoltage /= adc2.LowPulseWidth;
	adc2.lowVoltage *= (ADC_MAX_INPUT_VOLTAGE/ADC_RESOLUTION);
	sprintf(debugTransmitBuffer, "\n==============   Port B Voltage:   High: %f Low: %f   ==============\n\n", adc2.highVoltage, adc2.lowVoltage);
	CDC_Transmit_FS(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
	  HAL_UART_Transmit(&huart1, &debugTransmitBuffer[0], strlen(debugTransmitBuffer), HAL_MAX_DELAY);

		//Vin Voltage
	Vin.lowVoltage *= (ADC_MAX_INPUT_VOLTAGE/ADC_RESOLUTION);
	Vin.steadyState *= (ADC_MAX_INPUT_VOLTAGE/ADC_RESOLUTION);
	sprintf(debugTransmitBuffer, "\n==============   Vin Voltage:   AVG: %f Min: %f   ==============\n", Vin.steadyState, Vin.lowVoltage);
	CDC_Transmit_FS(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
	  HAL_UART_Transmit(&huart1, &debugTransmitBuffer[0], strlen(debugTransmitBuffer), HAL_MAX_DELAY);

		//Fuse Voltage
	Vfuse.lowVoltage *= (ADC_MAX_INPUT_VOLTAGE/ADC_RESOLUTION);
	Vfuse.steadyState *= (ADC_MAX_INPUT_VOLTAGE/ADC_RESOLUTION);
	sprintf(debugTransmitBuffer, "\n==============   Fuse Voltage:   AVG: %f Min: %f   ==============\n", Vfuse.steadyState, Vfuse.lowVoltage);
	CDC_Transmit_FS(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
	  HAL_UART_Transmit(&huart1, &debugTransmitBuffer[0], strlen(debugTransmitBuffer), HAL_MAX_DELAY);

		//MOSFET Voltage
	Vmos1.lowVoltage *= (ADC_MAX_INPUT_VOLTAGE/ADC_RESOLUTION);
	Vmos1.highVoltage *= (ADC_MAX_INPUT_VOLTAGE/ADC_RESOLUTION);
	sprintf(debugTransmitBuffer, "\n==============   MOSFET 1 Voltage:   High: %f Low: %f   ==============\n", Vmos1.highVoltage, Vmos1.lowVoltage);
	CDC_Transmit_FS(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
	  HAL_UART_Transmit(&huart1, &debugTransmitBuffer[0], strlen(debugTransmitBuffer), HAL_MAX_DELAY);

	Vmos2.lowVoltage *= (ADC_MAX_INPUT_VOLTAGE/ADC_RESOLUTION);
	Vmos2.highVoltage *= (ADC_MAX_INPUT_VOLTAGE/ADC_RESOLUTION);
	sprintf(debugTransmitBuffer, "\n==============   MOSFET 2 Voltage:   High: %f Low: %f   ==============\n", Vmos2.highVoltage, Vmos2.lowVoltage);
	CDC_Transmit_FS(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
	  HAL_UART_Transmit(&huart1, &debugTransmitBuffer[0], strlen(debugTransmitBuffer), HAL_MAX_DELAY);


	  LatchCurrent2 = adc2.highVoltage - adc1.lowVoltage;
	  LatchCurrent1 = adc1.highVoltage - adc2.lowVoltage;
	  LatchCurrent1 /= ADC_Rcurrent;
	  LatchCurrent2 /= ADC_Rcurrent;


	sprintf(debugTransmitBuffer, "\n==============   Lactch Current:   Pulse 1: %f Pulse 2: %f   ==============\n", LatchCurrent1, LatchCurrent2);
	CDC_Transmit_FS(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
	  HAL_UART_Transmit(&huart1, &debugTransmitBuffer[0], strlen(debugTransmitBuffer), HAL_MAX_DELAY);

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
		sprintf(debugTransmitBuffer, "==============   ADC Average Results   ==============");
		CDC_Transmit_FS(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
		  HAL_UART_Transmit(&huart1, &debugTransmitBuffer[0], strlen(debugTransmitBuffer), HAL_MAX_DELAY);
		for(int i = 0;i<LatchCountTimer;i++)
		{
			sprintf(debugTransmitBuffer, "%d,%d,%d,%d\n",i,adc1.avg_Buffer[i],adc2.avg_Buffer[i],Vfuse.avg_Buffer[i]);
			CDC_Transmit_FS(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
			  HAL_UART_Transmit(&huart1, &debugTransmitBuffer[0], strlen(debugTransmitBuffer), HAL_MAX_DELAY);
		}
}
//	=================================================================================//
//	=================================================================================//
//	=================================================================================//
