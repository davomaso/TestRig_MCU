/*
 * v1.c
 *
 *  Created on: 30 Jun 2021
 *      Author: mason
 */
#include <utility_functions.h>
#include "main.h"
#include "Global_Variables.h"
#include "Communication.h"
#include "LCD.h"
#include "UART_Routine.h"
#include "string.h"
#include "TestVectors.h"

void clearTestStatusLED() {
	HAL_GPIO_WritePin(FAIL_GPIO_Port, FAIL_Pin, GPIO_PIN_RESET);					// Clear all status LEDs, turn LED to off completely
	HAL_GPIO_WritePin(PASS_GPIO_Port, PASS_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);
}

void testInputVoltage() {															// Routine to test the input voltage
	Vin.total = InputVoltageCounter = 0;											// Set the total and input voltage counter to zero
	HAL_GPIO_WritePin(PIN2EN_GPIO_Port, PIN2EN_Pin, GPIO_PIN_SET);					// Enable the power on two pin port
	BoardResetTimer = 250;															// Board reset timer to allow the power to stablise
	InputVoltageStable = false;														// Ensure that stable flag is not set
	LCD_printf((uns_ch*) "Board Power Test", 2, 0);									// Print current state of system to LCD
}

void testSolarCharger() {															// Routine to test the solar charger on 4270 & 4020 boards
	Vin.total = SolarChargerCounter = 0;											//	Set the total and solar charger count to zero
	LCD_printf((uns_ch*) "Solar Charger Test", 2, 1);								// Print the state of the system
	SolarChargerTimer = 1250;														// Time allocated for the system to stablise and measure 13.6+ volts
	SolarChargerSampling = true;													// Enable solar charger sampling
	HAL_GPIO_WritePin(SOLAR_CH_EN_GPIO_Port, SOLAR_CH_EN_Pin, GPIO_PIN_SET);		// Enable solar charging, Change the Relay state
}

void initialiseTargetBoard(TboardConfig *Board) {
	LCD_printf((uns_ch*) "    Initialising    ", 2, 0);								// Initialise target board ready to be placed on self or return to standard 250,1 parameters
	uns_ch Command;
	Command = 0xCC;																	// Initialisation command
	BoardCommsParameters[0] = 0x49;													// Parameter to initialise target board correctly
	BoardCommsParametersLength = 1;
	communication_array(Command, &BoardCommsParameters[0], BoardCommsParametersLength);	// Transmit command
}

void interrogateTargetBoard() {														// Interrogate target board
	if(CurrentState != csIDLE){
		if (CurrentState == csSerialise)											// Print to LCD if in serialising or not in IDLE
			LCD_printf((uns_ch *) "    Serialising    ", 2, 0);
		else
			LCD_printf((uns_ch *) "   Interrogating    ", 2, 0);
	}
	uns_ch Command;
	Command = 0x10;
	communication_arraySerial(Command, 0, 0);										// Transmit the interogation command
}

void configureTargetBoard(TboardConfig *Board) {									// Configure target board
	uns_ch Command;
	Command = 0x56;
	SetPara(Board, Command);
	if (Board->BoardType != b422x)													// No need to reconfigure as board is configured with the initialisation
		communication_array(Command, &BoardCommsParameters[0], BoardCommsParametersLength);
	OutputsSet = false;																// Ensure that the outputs are only set the once
}

void sampleTargetBoard(TboardConfig *Board) {										// Sample Target board
	uns_ch Command;
	LCD_printf((uns_ch*) "      Sampling      ", 2, 0);
	Command = 0x1A;																	// 0x1A command to initialise sampling
	SetPara(Board, Command);														// Set the required parameters
	communication_array(Command, (uns_ch*) &BoardCommsParameters[0], BoardCommsParametersLength);	// Transmit to the board
}

void calibrateTargetBoard(TboardConfig *Board) {									// Calibrate Target Board
	uns_ch Command = 0xC0;
	LCD_printf((uns_ch*) "     Calibrating      ", 2, 0);							// Print to LCD screen
	BoardCommsParameters[0] = 0x50;
	BoardCommsParametersLength = 1;
	communication_array(Command, &BoardCommsParameters[0], BoardCommsParametersLength);	// Transmit to target board
}

void uploadSamplesTargetBoard(TboardConfig *Board) {								// Upload results from target board to testrig
	uns_ch Command;
	Command = 0x18;
	LCD_printf((uns_ch*) "     Uploading      ", 2, 0);								// Print to LCD screen
	SetPara(Board, Command);														// Set parameters
	communication_array(Command, &BoardCommsParameters[0], BoardCommsParametersLength);	// Transmit to target board
}

void GetBatteryLevel(TboardConfig *Board) {											// Take average of the battery voltage across all tests
	for (uint8 i = 0; i < Board->GlobalTestNum; i++) {
		Board->BatteryLevel += (float) Board->rawBatteryLevel[i] / 10.0;			// Battery voltage in base 10 so divide by 10 to get floating
	}
	Board->BatteryLevel /= Board->GlobalTestNum;									// Take average across tests completed
	if (Board->BatteryLevel > BATTERY_VOLTAGE_THRESHOLD)// "TestVectors.h"			// Ensure voltage is higher than the voltage threshold
		SET_BIT(Board->BVR, BATT_LVL_STABLE);										// Set bit if battery voltage was stable
	else
		CLEAR_BIT(Board->BVR, BATT_LVL_STABLE);										// Clear bit if battery voltage did not meet threshold
}

void CheckPowerRegisters(TboardConfig *Board) {										//	Check the power registers
	PrintVoltages(Board);															// Print the board voltages to the terminal
	if (VoltageComparison(&Board->VoltageBuffer[V_INPUT], INPUT_VOLTAGE_THRESHOLD)) // Compare Input Voltage for all boards
		SET_BIT(Board->BVR, INPUT_V_STABLE);										// Set input voltage stable

	switch (Board->BoardType) {
	case b422x:																		// Only other voltage on a 4220 is the 12v output voltage
		if (VoltageComparison(&Board->VoltageBuffer[V_12output], OUTPUT_VOLTAGE_VALUE))
			SET_BIT(Board->BVR, V12_OUTPUT_STABLE);									// Set V12_OUTPUT_STABLE bit if solar charger stable
		SET_BIT(Board->BVR, V3_SAMPLE_STABLE);										// Set the rest of the bits as no other voltages are required, Conditional statements can be added if the board is changed
		SET_BIT(Board->BVR, V12_SAMPLE_STABLE);
		SET_BIT(Board->BVR, V10_SAMPLE_STABLE);
		SET_BIT(Board->BVR, TRIM_VOLTAGE_STABLE);
		SET_BIT(Board->BVR, SOLAR_V_STABLE);
	case b427x:
		if (VoltageComparison(&Board->VoltageBuffer[V_SOLAR], SOLAR_CHARGER_THRESHOLD))
			SET_BIT(Board->BVR, SOLAR_V_STABLE);									// Set SOLAR_V_STABLE bit if solar charger stable
		SET_BIT(Board->BVR, V3_SAMPLE_STABLE);
		SET_BIT(Board->BVR, V12_SAMPLE_STABLE);
		if (VoltageComparison(&Board->VoltageBuffer[V_12output], OUTPUT_VOLTAGE_VALUE))
			SET_BIT(Board->BVR, V12_OUTPUT_STABLE);									// Set V12_OUTPUT_STABLE bit if 12V output voltage stable
		SET_BIT(Board->BVR, V10_SAMPLE_STABLE);
		SET_BIT(Board->BVR, TRIM_VOLTAGE_STABLE);
		break;
	case b935x:
	case b937x:
		SET_BIT(Board->BVR, SOLAR_V_STABLE);
		if (VoltageComparison(&Board->VoltageBuffer[V_3], THREE_VOLT_SAMPLE_VALUE))
			SET_BIT(Board->BVR, V3_SAMPLE_STABLE);									// Set V3_SAMPLE_STABLE bit if 3v sample voltage stable
		if (VoltageComparison(&Board->VoltageBuffer[V_12], TWELVE_VOLT_SAMPLE_VALUE))
			SET_BIT(Board->BVR, V12_SAMPLE_STABLE);									// Set V12_SAMPLE_STABLE bit if 12v sample voltage stable
		if (VoltageComparison(&Board->VoltageBuffer[V_12output], OUTPUT_VOLTAGE_VALUE))
			SET_BIT(Board->BVR, V12_OUTPUT_STABLE);									// Set V12_OUTPUT_STABLE bit if output voltage stable
		SET_BIT(Board->BVR, V10_SAMPLE_STABLE);
		SET_BIT(Board->BVR, TRIM_VOLTAGE_STABLE);
		break;
	case b401x:
		SET_BIT(Board->BVR, SOLAR_V_STABLE);
		if (VoltageComparison(&Board->VoltageBuffer[V_3], THREE_VOLT_SAMPLE_VALUE))
			SET_BIT(Board->BVR, V3_SAMPLE_STABLE);									// Set V3_SAMPLE_STABLE bit if 3v sample voltage stable
		if (VoltageComparison(&Board->VoltageBuffer[V_12], TWELVE_VOLT_SAMPLE_VALUE))
			SET_BIT(Board->BVR, V12_SAMPLE_STABLE);									// Set V12_SAMPLE_STABLE bit if 12v sample voltage stable
		if (VoltageComparison(&Board->VoltageBuffer[V_12output], OUTPUT_VOLTAGE_VALUE))
			SET_BIT(Board->BVR, V12_OUTPUT_STABLE);									// Set V12_OUTPUT_STABLE bit if output voltage stable
		if (VoltageComparison(&Board->VoltageBuffer[V_105], TEN_VOLT_SAMPLE_THRESHOLD))
			SET_BIT(Board->BVR, V10_SAMPLE_STABLE);									// Set V10_SAMPLE_STABLE bit if 10.5v voltage stable
		if (VoltageComparison(&Board->VoltageBuffer[V_trim], TEN_VOLT_SAMPLE_THRESHOLD))
			SET_BIT(Board->BVR, TRIM_VOLTAGE_STABLE);								// Set TRIM_VOLTAGE_STABLE bit if trim voltage stable
		break;
	case b402x:
		if (VoltageComparison(&Board->VoltageBuffer[V_SOLAR], SOLAR_CHARGER_THRESHOLD))
			SET_BIT(Board->BVR, SOLAR_V_STABLE);									// Set SOLAR_V_STABLE bit if solar charger stable
		if (VoltageComparison(&Board->VoltageBuffer[V_3], THREE_VOLT_SAMPLE_VALUE))
			SET_BIT(Board->BVR, V3_SAMPLE_STABLE);									// Set V3_SAMPLE_STABLE bit if 3v sample voltage stable
		if (VoltageComparison(&Board->VoltageBuffer[V_12], TWELVE_VOLT_SAMPLE_VALUE))
			SET_BIT(Board->BVR, V12_SAMPLE_STABLE);									// Set V12_SAMPLE_STABLE bit if 12v sample voltage stable
		if (VoltageComparison(&Board->VoltageBuffer[V_12output], OUTPUT_VOLTAGE_VALUE))
			SET_BIT(Board->BVR, V12_OUTPUT_STABLE);									// Set V12_OUTPUT_STABLE bit if output voltage stable
		if (VoltageComparison(&Board->VoltageBuffer[V_105], TEN_VOLT_SAMPLE_THRESHOLD))
			SET_BIT(Board->BVR, V10_SAMPLE_STABLE);									// Set V10_SAMPLE_STABLE bit if 10.5v voltage stable
		if (VoltageComparison(&Board->VoltageBuffer[V_trim], TEN_VOLT_SAMPLE_THRESHOLD))
			SET_BIT(Board->BVR, TRIM_VOLTAGE_STABLE);								// Set TRIM_VOLTAGE_STABLE bit if trim voltage stable
		break;
	case bNone:
		CLEAR_REG(Board->BVR);
		break;

	}
	if ( READ_REG(Board->BVR) != 0xFF)
		CLEAR_BIT(Board->BSR, BOARD_POWER_STABLE);									// Clear Reg if incorrect power registers are set
}

_Bool VoltageComparison(float *Sample, float ExpectedValue) {						// Routine to calculate tolerance from expected value and compare to the measured voltage
	float tolerance;
	tolerance = GET_SAMPLE_VOLTAGE_TOLERANCE(ExpectedValue);						// Get tolerance routine, 10% accuracy, Adjustable in header file
	if ((*Sample < (ExpectedValue + tolerance)) && (*Sample > (ExpectedValue - tolerance)))
		return true;																// Return true if comparison passed
	else
		return false;																// Return false if comparison failed
}

void TestComplete(TboardConfig *Board) {											// Run when test is complete
	HAL_GPIO_WritePin(PIN2EN_GPIO_Port, PIN2EN_Pin, GPIO_PIN_RESET);				// disable power to the board
	HAL_GPIO_WritePin(PIN5EN_GPIO_Port, PIN5EN_Pin, GPIO_PIN_RESET);
	if (READ_BIT(Board->BSR,BOARD_TEST_PASSED)) {									// if test passed bit is set enable Status LED  & print Test passed to LCD
		HAL_GPIO_WritePin(PASS_GPIO_Port, PASS_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(FAIL_GPIO_Port, FAIL_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);
		sprintf((char*) &previousTestBuffer[0], "Previous Test Passed");
	} else {
		HAL_GPIO_WritePin(FAIL_GPIO_Port, FAIL_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(PASS_GPIO_Port, PASS_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);
		sprintf((char*) &previousTestBuffer[0], "Previous Test Failed");
	}
	timeOutEn = false;																// Disable timeoutEn
	LCD_printf(&previousTestBuffer[0], 1, 0);										// Print Test passed to LCD on top line
}
void PrintHomeScreen(TboardConfig *Board) {											// Print LCD homescreen, print serial number, connect loom and testing options
	LCD_Clear();																	// Clear LCD screen completely
	if (Board->BoardType != bNone) {												// print Test rig and board connected if a loom is detected
		sprintf((char*) &lcdBuffer, "TEST RIG        %x", Board->BoardType);
	} else {
		LCD_printf((uns_ch*) "Connect Loom", 2, 1);									// Print connect loom when no board is detected
		sprintf((char*) &lcdBuffer, "TEST RIG");
	}
	LCD_printf((uns_ch*) &lcdBuffer, 1, 1);
	if (Board->SerialNumber) {
		sprintf((char*) &lcdBuffer[0], "SN:%lu        ", Board->SerialNumber);		// populate buffer with serial no.
		LCD_printf((uns_ch*) &lcdBuffer, 2, 1);										// display serial no. to screen
		sprintf((char*) &lcdBuffer[0], "v%x", Board->Version);						// populate buffer with version no.
		LCD_setCursor(2, 18);														// Move cursor to edge of screen
		LCD_displayString((uns_ch*) &lcdBuffer[0], strlen((char*) &lcdBuffer));		// display version to screen
		LCD_printf((uns_ch*) "1 - Test Only", 3, 0);								// print options to lcd screen
		LCD_printf((uns_ch*) "3 - New SN  Prog - #", 4, 0);
	} else if (TestRigMode != BatchMode) {
		clearTestStatusLED();														// Clear LEDs
		LCD_ClearLine(3);															// clear line 3 and status leds
	}
}

void PrintVoltages(TboardConfig *Board) {											// Print the various sample voltages and output voltages of the board to the termninal
	printT((uns_ch*) "\n==========     Target Board Voltages     ==========\n");
	if ((Board->BoardType == b402x) || (Board->BoardType == b427x)) {				// Print Solar charger voltages
		sprintf((char*) &debugTransmitBuffer[0], "Solar Voltage:         %.3f\n", Board->VoltageBuffer[V_SOLAR]);
		printT((uns_ch*) &debugTransmitBuffer[0]);
	}
	sprintf((char*) &debugTransmitBuffer[0], "Input Voltage:         %.3f\n", Board->VoltageBuffer[V_INPUT]);	// Print Input voltage, everyboard has this voltage, so unconditional
	printT((uns_ch*) &debugTransmitBuffer[0]);
	if (Board->BoardType != b427x && Board->BoardType != b422x) {					// Print 3V sample voltage for everyboard bar 4220 & 4270
		sprintf((char*) &debugTransmitBuffer[0], "3V Sample Voltage:      %.3f\n", Board->VoltageBuffer[V_3]);
		printT((uns_ch*) &debugTransmitBuffer[0]);
	}
	if ((Board->BoardType != b422x) && (Board->BoardType != b401x) && (Board->BoardType != b402x)) {		// Print 12v Sample voltage on all boards bar 4220, 4010 and 4020, print 10.5v & trim on 4010, 4020 boards
		sprintf((char*) &debugTransmitBuffer[0], "12V Sample Voltage:    %.3f\n", Board->VoltageBuffer[V_12]);
		printT((uns_ch*) &debugTransmitBuffer[0]);
	} else if (Board->BoardType == b401x || Board->BoardType == b402x) {
		sprintf((char*) &debugTransmitBuffer[0], "10.5V User Voltage:    %.3f\n", Board->VoltageBuffer[V_105]);
		printT((uns_ch*) &debugTransmitBuffer[0]);
		sprintf((char*) &debugTransmitBuffer[0], "10.5V Adjusted:        %.3f\n", Board->VoltageBuffer[V_trim]);
		printT((uns_ch*) &debugTransmitBuffer[0]);
	}
	sprintf((char*) &debugTransmitBuffer[0], "12V Output:            %.3f\n", Board->VoltageBuffer[V_12output]);	// Print 12v output and battery on all boards
	printT((uns_ch*) &debugTransmitBuffer[0]);
	sprintf((char*) &debugTransmitBuffer[0], "Battery Level:         %.3f\n", Board->BatteryLevel);
	printT((uns_ch*) &debugTransmitBuffer[0]);
}
