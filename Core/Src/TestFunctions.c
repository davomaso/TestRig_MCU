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
#include "Global_Variables.h"
#include "TestFunctions.h"
#include "TestVectors.h"
#include "Test.h"
#include "DAC.h"
#include "DAC_Variables.h"
#include "Calibration.h"
#include "Delay.h"
#include "File_Handling.h"
#include "UART_Routine.h"

/*
 * This function will check the test that is being run on each port, then assign the various channel values to be compared to the measured values by the target board
 */
void TestFunction(TboardConfig *Board) {
	//	HAL_GPIO_WritePin(MUX_RS_GPIO_Port, MUX_RS_Pin, GPIO_PIN_SET);
	uint8 totalPortCount = 0;
	uint8 currPort;
	uint8 DataChannel = 0;
	//====================== Analog Test Count ======================//
	for (currPort = 0; currPort < Board->latchPortCount; currPort++) {	// Iterate through each port to assign the channel values
		switch (Board->TestCode[totalPortCount]) {
		case TWO_WIRE_LATCHING:
			CHval[Board->GlobalTestNum][DataChannel] = twoWireLatching(Board, currPort, 1);	// set the CHval to 1000 if latch test is enabled
			break;
		case NOTEST:
			CHval[Board->GlobalTestNum][DataChannel] = twoWireLatching(Board, currPort, 0);
			break;
		}
		DataChannel += 2;																	// increment ch count by 2 if no test or latch test is assigned
		totalPortCount++;
	}
	//====================== Analog Test Count ======================//
	for (currPort = 0; currPort < Board->analogInputCount; currPort++) {					// Assign the values to all current, voltage, SDI-12, async tests
		switch (Board->TestCode[totalPortCount]) {
		case TWENTY_AMP:
			CHval[Board->GlobalTestNum][DataChannel++] = setCurrentTestDAC(currPort);
			break;
		case THREE_VOLT:
			CHval[Board->GlobalTestNum][DataChannel++] = setVoltageTestDAC(currPort, THREE_VOLT);
			break;
		case TWOFIVE_VOLT:
			CHval[Board->GlobalTestNum][DataChannel++] = setVoltageTestDAC(currPort, TWOFIVE_VOLT);
			break;
		case ONE_VOLT:
			CHval[Board->GlobalTestNum][DataChannel++] = setVoltageTestDAC(currPort, ONE_VOLT);
			break;
		case ASYNC_PULSE:
			CHval[Board->GlobalTestNum][DataChannel++] = setAsyncPulseCount(Board, currPort);
			break;
		case SDI_TWELVE:
			CHval[Board->GlobalTestNum][DataChannel++] = setSDItwelveValue(currPort);
			break;
		case AQUASPY:
			setRS485values(&CHval[Board->GlobalTestNum][DataChannel]);
			DataChannel += 9;
			break;
		}
		totalPortCount++;
	}
	//====================== Digital Test Count ======================//
	for (currPort = 6; currPort < Board->digitalInputCout + 6; currPort++) {				// Assign the amount of asynchronous pulses to be generated on the top 3 ports/supplementary inputs of 9350
		switch (Board->TestCode[totalPortCount]) {
		case ASYNC_PULSE:
			CHval[Board->GlobalTestNum][DataChannel++] = setAsyncPulseCount(Board, currPort);
			break;
		}
		totalPortCount++;
	}
	if (Board->BoardType == b402x) {														// Enable Ouput test on 4020, Handling of the results are required by the set/measured TODO: handle the output test in SetVsMeasured()
		if (currPort == 9 && Board->TestCode[totalPortCount] == 0x01) {
			CHval[Board->GlobalTestNum][totalPortCount++] = 1;
		} else
			CHval[Board->GlobalTestNum][totalPortCount++] = 0;
	}
	OutputsSet = true;																		// Following the assignment of outputs, enable a set flag to assure they are not set twice
}
//	==================================================================================	//

//	=================================   Two Wire   =================================	//
_Bool twoWireLatching(TboardConfig *Board, uint8 Test_Port, _Bool state) {				// Using the test port passed to the routine enable the latchtest and return the state
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
float setCurrentTestDAC(uint8 Test_Port) {												// Pass the port to be configured, if low current test has been complete test the board at a higher current, return the set value
	float current;
	uint16 Corrected_DACvalue;
	uint16 DAC_Value;
	//Correction Factor
	if (!Port[Test_Port].lowItestComplete) {
		current = LOW_CURRENT_TEST;														// Set the current to the low value
		Port[Test_Port].lowItestComplete = true;										// flip the flag to true
		DAC_Value = DAC_CURRENT_CALC(current);											// Calculate the DAC value
		Corrected_DACvalue = DAC_Value + Port[Test_Port].CalibrationFactor[I_4];		// Add the correction factor
	} else {
		current = HIGH_CURRENT_TEST;													// Set the current to the high value
		Port[Test_Port].lowItestComplete = true;										// flip the flag to true
		DAC_Value = DAC_CURRENT_CALC(current);											// Calculate the DAC value
		Corrected_DACvalue = DAC_Value + Port[Test_Port].CalibrationFactor[I_175];		// Add the correction factor
	}
	if (Test_Port == Port_1 || Test_Port == Port_3 || Test_Port == Port_5)				// OR the correct port factor to the DAC to enable which ch to place the voltage on
		Corrected_DACvalue += 0x3000;
	else if (Test_Port == Port_2 || Test_Port == Port_4 || Test_Port == Port_6)
		Corrected_DACvalue += 0xB000;
	DAC_set(Test_Port, Corrected_DACvalue);												// Set the DAC with the value
	MUX_Sel(Test_Port, TWENTY_AMP);														// Enable the multiplexer with the type of test and port the Value is being placed on
	return current;																		// return the value being tested
}
//	==================================================================================	//

//	=================================   Three Volt   =================================	//
float setVoltageTestDAC(uint8 Test_Port, uint8 TestCode) {								// Pass the test port and test code to establish which port and what test is being run
	float voltage;
	uint16 Corrected_DACvalue;
	Corrected_DACvalue = 0;
	uint16 DAC_Value;
	switch (TestCode) {
	case ONE_VOLT:																		// 1v Range test
		voltage = LOW_VOLTAGE_TEST;		//"TestVectors.h"								// Low voltage vector loaded into voltage
		DAC_Value = DAC_VOLTAGE_CALC(voltage);											// Calculate the DAC value
		Corrected_DACvalue = DAC_Value + Port[Test_Port].CalibrationFactor[V_05];		// Add the correction factor
		break;
	case TWOFIVE_VOLT:																	//2.5v range test
		voltage = HIGH_VOLTAGE_TEST;													// set the voltage to the high range
		DAC_Value = DAC_VOLTAGE_CALC(voltage);											// Calculate the DAC value
		Corrected_DACvalue = DAC_Value + Port[Test_Port].CalibrationFactor[V_24];		// Add the correction factor
		break;
	}
	if (Test_Port == Port_1 || Test_Port == Port_3 || Test_Port == Port_5)				// OR the correct port factor to the DAC to enable which ch to place the voltage on
		Corrected_DACvalue += 0x3000;
	else if (Test_Port == Port_2 || Test_Port == Port_4 || Test_Port == Port_6)
		Corrected_DACvalue += 0xB000;

	DAC_set(Test_Port, Corrected_DACvalue);												// Set the DAC with the value
	MUX_Sel(Test_Port, THREE_VOLT);														// Enable the multiplexer with the type of test and port the Value is being placed on
	return voltage;																		// return the value being tested
}
//	==================================================================================	//

//	===================================   ASYNC   ===================================	//
float setAsyncPulseCount(TboardConfig *Board, uint8 Test_Port) {
	HAL_GPIO_WritePin(Port[Test_Port].Async.Port, Port[Test_Port].Async.Pin, GPIO_PIN_RESET);
	MUX_Sel(Test_Port, ASYNC_PULSE);
	switch (Board->GlobalTestNum) {
	case 0:
	case 1:
		Port[Test_Port].Async.PulseCount = ASYNC_LOW_COUNT;								// Set async count to low count, vectors found in "TestVectors.h"
		break;
	case 2:
	case 3:
		Port[Test_Port].Async.PulseCount = ASYNC_MEDIUM_COUNT;							// Set async count to medium count
		break;
	case 4:
	case 5:
		Port[Test_Port].Async.PulseCount = ASYNC_HIGH_COUNT;							// Set async count to high count
		break;
	}
	Port[Test_Port].Async.PulseState = true;											// Enable pulsestate to begin pulsing when statemachine reaches csAsync
	if (Port[Test_Port].Async.FilterEnabled) {											// If filter is enabled increased apply the noise buffer, should filter out faster high freq noise
		Port[Test_Port].Async.fcount = AsyncDebounceBuffer[(Port[Test_Port].Async.PulseCount + Test_Port) % 5][0];
		return Port[Test_Port].Async.PulseCount;										// Return the pulsecount to CHval
	} else {
		Port[Test_Port].Async.fcount = 5;												// return the short pulse count
		return Port[Test_Port].Async.PulseCount * 11;									// multiply the pulse count by 11 if filter isnt enabled,	counts: 6 on pulse on 5 pulse off, 5fcount 1 pulse down 5fcount as pulses back to steady state
	}
}
//	===================================================================================	//

//	===================================   SDI-12    ===================================	//
float setSDItwelveValue(uint8 Test_Port) {
	MUX_Sel(Test_Port, SDI_TWELVE);														// Set the multiplexer to SDI-12
	SDSstate = SDSundef;																// Set the SDI-12 initial state to undefined
	Port[Test_Port].Sdi.Enabled = true;													// Enable SDI on the specific port
	Port[Test_Port].Sdi.Address = Test_Port;											// Set the address of the sensor to the port of the test
	Port[Test_Port].Sdi.setValue = HAL_RNG_GetRandomNumber(&hrng) % 10000;				// Set the SDI-12 to random 4 sig figure
	Port[Test_Port].Sdi.setValue /= 1000;												// 3 decimal places result
	return Port[Test_Port].Sdi.setValue;												// return the results to CHval
}
//	===================================================================================	//

void setRS485values(float *RS485buffer) {
	for (uint i = 0; i < 9; i++) {
		RS485sensorBuffer[i] = HAL_RNG_GetRandomNumber(&hrng) % 10000;					// Get true random number with 4 sig figures of data
		RS485sensorBuffer[i] /= 1000;													// Get the data in floating point form with 3 decimal places
	}
	memcpy(RS485buffer, &RS485sensorBuffer[0], sizeof(float) * 9);						// Copy the array into ChVal
}

//	===================================    MUX    ===================================	//
void MUX_Sel(uint8 Test_Port, uint8 Test) {												// Multiplexer select routine, requires the port and test being run, Sets the multiplexer to the required signal being switched through
	_Bool MuxState = HAL_GPIO_ReadPin(MUX_A0_GPIO_Port, MUX_A0_Pin);					// Maintain the state of the MUX_A0 pin for comms
	if (Test_Port <= Port_6) {
		switch (Test_Port) {															// Switch the required ports Write pin low
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
		delay_us(5);																	// 5us delay for the multiplexer to switch
		switch (Test) {																	// Switch the required test to the right port
		case SDI_TWELVE:
			HAL_GPIO_WritePin(MUX_A0_GPIO_Port, MUX_A0_Pin, GPIO_PIN_RESET); 			// MUX Address = 1
			HAL_GPIO_WritePin(MUX_A1_GPIO_Port, MUX_A1_Pin, GPIO_PIN_RESET);
			break;
		case THREE_VOLT:
			HAL_GPIO_WritePin(MUX_A0_GPIO_Port, MUX_A0_Pin, GPIO_PIN_SET); 				// MUX Address = 2
			HAL_GPIO_WritePin(MUX_A1_GPIO_Port, MUX_A1_Pin, GPIO_PIN_RESET);
			break;
		case TWENTY_AMP:
			HAL_GPIO_WritePin(MUX_A0_GPIO_Port, MUX_A0_Pin, GPIO_PIN_RESET); 			// MUX Address = 3
			HAL_GPIO_WritePin(MUX_A1_GPIO_Port, MUX_A1_Pin, GPIO_PIN_SET);
			break;
		case ASYNC_PULSE:
			HAL_GPIO_WritePin(MUX_A0_GPIO_Port, MUX_A0_Pin, GPIO_PIN_SET); 				// MUX Address = 4
			HAL_GPIO_WritePin(MUX_A1_GPIO_Port, MUX_A1_Pin, GPIO_PIN_SET);
			break;
		}
		delay_us(5);																	// 5us delay for the multiplexer to switch
		switch (Test_Port) {															// Return the WR to its default state
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
		if (MuxState)																	// Reenable/Disable MUX_A0 depending on the state that it was previously in
			HAL_GPIO_WritePin(MUX_A0_GPIO_Port, MUX_A0_Pin, GPIO_PIN_SET);
		else
			HAL_GPIO_WritePin(MUX_A0_GPIO_Port, MUX_A0_Pin, GPIO_PIN_RESET);
	}

}

void reset_ALL_MUX() {																	// Changes all multiplexers to the Async channel, in the reset state, ensure no voltage is applied to the outputs once testing is complete
	for (uint8 i = Port_1; i <= Port_9; i++) {
		HAL_GPIO_WritePin(Port[i].Async.Port, Port[i].Async.Pin, GPIO_PIN_RESET);
		MUX_Sel(i, ASYNC_PULSE);
	}
}
//	====================================================================================//

//	====================================================================================//
void ADC_MUXsel(uint8 ADCport) {													   	// Multiplexer to switch the ADC signal to the mcu, requires only 2 pins to test the latching of all boards with various latch tests
	switch (ADCport) {																	//	Latch ADC is transparent so no
	case Port_1:
		HAL_GPIO_WritePin(ADC_MUX_A_GPIO_Port, ADC_MUX_A_Pin, GPIO_PIN_RESET);			// MUX Address 1
		HAL_GPIO_WritePin(ADC_MUX_B_GPIO_Port, ADC_MUX_B_Pin, GPIO_PIN_RESET);
		break;
	case Port_2:
		HAL_GPIO_WritePin(ADC_MUX_A_GPIO_Port, ADC_MUX_A_Pin, GPIO_PIN_SET);			// MUX Address 2
		HAL_GPIO_WritePin(ADC_MUX_B_GPIO_Port, ADC_MUX_B_Pin, GPIO_PIN_RESET);
		break;
	case Port_3:
		HAL_GPIO_WritePin(ADC_MUX_A_GPIO_Port, ADC_MUX_A_Pin, GPIO_PIN_RESET);			// MUX Address 3
		HAL_GPIO_WritePin(ADC_MUX_B_GPIO_Port, ADC_MUX_B_Pin, GPIO_PIN_SET);
		break;
	case Port_4:
		HAL_GPIO_WritePin(ADC_MUX_A_GPIO_Port, ADC_MUX_A_Pin, GPIO_PIN_SET);			// MUX Address 4
		HAL_GPIO_WritePin(ADC_MUX_B_GPIO_Port, ADC_MUX_B_Pin, GPIO_PIN_SET);
		break;
	}
}
//	=================================================================================//
//	=================================================================================//
//	=================================================================================//
