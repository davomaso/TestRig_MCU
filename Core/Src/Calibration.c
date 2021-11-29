#include <main.h>
#include "Global_Variables.h"
#include "Calibration.h"

#include "UART_Routine.h"
#include "DAC_Variables.h"
#include "EEPROM.h"
#include "LCD.h"
#include "DAC.h"
#include "TestFunctions.h"


	/*
	 * Routine to calibrate the test rig DAC used for the sensor ports. Keypad used to increment & decrement the DAC step. Use multimeter to read the value,
	 * on exit of routine values are stored to eeprom and read upon startup of device to ensure system stays calibrated.
	 */
void TestRig_Calibration() {
	uint16 DACval;
	uint8 calPort;
	TcalTestConfig calTest;

	LCD_printf((uns_ch*) "Calibrate Test Rig", 2, 0);
	LCD_printf((uns_ch*) "1V - Port 1", 3, 5);
	printT((uns_ch*) "\n\n==========  Calibration Routine  ==========\n");
	calTest = V_1;												// First range is 1V
	calPort = Port_1;											// Start with first port
	read_correctionFactors();									// Read correction factors from eeprom
	DACval = DAC_VOLTAGE_CALC(1);								// Calculate the DAC reference to obtain 1V output
	DACval |= 0x3000 + Port[Port_1].CalibrationFactor[V_1];		// Ensure it is on Port_1, with the correction factor added
	DAC_set((calPort), DACval);									// Set DAC value
	MUX_Sel((calPort), THREE_VOLT);								// Ensure Mux is selected as Voltage
	printT((uns_ch*) "==========Calibrating 1V==========\n");
	sprintf((char*) &debugTransmitBuffer[0], "Port Calibrating: %d\n", calPort + 1);
	printT((uns_ch*) &debugTransmitBuffer[0]);

	while (calTest != Done) {
		if (KP[hash].Pressed || KP[star].Pressed) {				// if hash or star are pressed, either save the values to eeprom and read them back, or break out of calibration
			if (KP[hash].Pressed) {
				KP[hash].Pressed = false;
				printT((uns_ch*) "Writing To EPROM\n");
				write_correctionFactors();
				read_correctionFactors();
			} else
				KP[star].Pressed = false;
			calTest = Done;
			break;
		}
		if (KP[4].Pressed || KP[6].Pressed) {	// buttons 4/6 increment the port being calibrated, if V_1 OR I_175 is being tested roll over to next calibration value and port
			if (KP[4].Pressed) {
				KP[4].Pressed = 0;
				//Decrement Port
				if (calPort != Port_1) {
					calPort--;
				} else {
					if (calTest == V_1) {
						calTest = I_175;
						calPort = Port_6;
					} else {
						calTest--;
						calPort = Port_6;
					}
				}
			} else {
				KP[6].Pressed = 0;
				//Set New DAC values
				if (calPort != Port_6) {
					calPort++;
				} else {
					if (calTest != I_175) {
						calTest++;
						calPort = Port_1;
					} else {
						calTest = V_1;
						calPort = Port_1;
					}
				}
			}
			switch (calTest) {	// depending on the value being calibrated print to LCD screen, find the DAC value and add calibration factor
			case V_1:
				DACval = DAC_VOLTAGE_CALC(1);
				printT((uns_ch*) "==========Calibrating 1V========== \n\n");
				sprintf((char*) &lcdBuffer[0], "1V - Port %d     ", (calPort + 1));
				LCD_printf(&lcdBuffer[0], 3, 5);
				break;
			case V_05:
				printT((uns_ch*) "==========Calibrating 0.5V========== \n\n");
				DACval = DAC_VOLTAGE_CALC(0.5);
				sprintf((char*) &lcdBuffer[0], "0.5V - Port %d    ", (calPort + 1));
				LCD_printf((uns_ch*) &lcdBuffer[0], 3, 3);
				break;
			case V_24:
				printT((uns_ch*) "==========Calibrating 2.4V========== \n\n");
				DACval = DAC_VOLTAGE_CALC(2.4);
				sprintf((char*) &lcdBuffer[0], "2.4V - Port %d    ", (calPort + 1));
				LCD_printf((uns_ch*) &lcdBuffer, 3, 3);
				break;
			case I_20:
				printT((uns_ch*) "==========Calibrating 20mA========== \n\n");
				DACval = DAC_CURRENT_CALC(20.0);
				sprintf((char*) &lcdBuffer[0], "20mA - Port %d    ", (calPort + 1));
				LCD_printf((uns_ch*) &lcdBuffer[0], 3, 3);
				break;
			case I_4:
				printT((uns_ch*) "==========Calibrating 4mA========== \n\n");
				DACval = DAC_CURRENT_CALC(4);
				sprintf((char*) &lcdBuffer[0], "4mA - Port %d    ", (calPort + 1));
				LCD_printf((uns_ch*) &lcdBuffer[0], 3, 4);
				break;
			case I_175:
				printT((uns_ch*) "==========Calibrating 17.5mA========== \n\n");
				DACval = DAC_CURRENT_CALC(17.5);
				sprintf((char*) &lcdBuffer[0], "17.5mA - Port %d  ", (calPort + 1));
				LCD_printf((uns_ch*) &lcdBuffer[0], 3, 1);
				break;
			case Done:
				printT((uns_ch*) "==========	Done ========== \n\n");
				break;
			}
			sprintf((char*) &debugTransmitBuffer[0], "Port Calibrating: %d \n\n", (calPort + 1));
			printT(&debugTransmitBuffer[0]);

			DACval += Port[calPort].CalibrationFactor[calTest];
			//Write New Val To DAC
			if (calPort == Port_1 || calPort == Port_3 || calPort == Port_5)
				DACval |= 0x3000;
			else if (calPort == Port_2 || calPort == Port_4 || calPort == Port_6)
				DACval |= 0xB000;
			DAC_set((calPort), DACval);

			if (calTest < 3)
				MUX_Sel((calPort), THREE_VOLT);
			else
				MUX_Sel((calPort), TWENTY_AMP);
		}
		if (KP[2].Pressed || KP[8].Pressed || KP[3].Pressed || KP[9].Pressed) {	// if 2/3 is pressed either increment by 1 or 10, if 8/9 decrement by 1 or 10
			if (KP[2].Pressed) {
				//Increment CF by 1
				KP[2].Pressed = false;
				(Port[calPort].CalibrationFactor[calTest])++;
				DACval++;
			} else if (KP[8].Pressed) {
				//Decrement CF by 1
				KP[8].Pressed = false;
				(Port[calPort].CalibrationFactor[calTest])--;
				DACval--;
			} else if (KP[3].Pressed) {
				//Increment CF by 10
				KP[3].Pressed = false;
				Port[calPort].CalibrationFactor[calTest] += 10;
				DACval += 10;
			} else if (KP[9].Pressed) {
				// Decrement CF by 10
				KP[9].Pressed = false;
				Port[calPort].CalibrationFactor[calTest] -= 10;
				DACval -= 10;
			}

			if (calPort == Port_1 || calPort == Port_3 || calPort == Port_5) {
				DACval &= 0x0FFF;	// Can probably be removed	//TODO: try removing this
				DACval |= 0x3000;
			} else if (calPort == Port_2 || calPort == Port_4 || calPort == Port_6) {
				DACval &= 0x0FFF;
				DACval |= 0xB000;
			}
			DAC_set((calPort), DACval);
			sprintf((char*) &debugTransmitBuffer[0], "%d\n", Port[calPort].CalibrationFactor[calTest]);
			printT(&debugTransmitBuffer[0]);
		}
	}
}

	// Puts 1V on all sensor ports and prepare the ADC to begin sampling to determine when to switch to current calibration
void TargetBoardCalibration_Voltage(TboardConfig *Board) {
	uint16 DACval;
//	uns_ch Command;
	_Bool MuxState = HAL_GPIO_ReadPin(MUX_A0_GPIO_Port, MUX_A0_Pin);
	for (uint8 i = Port_1; i <= Port_6; i++) {
		DACval = DAC_VOLTAGE_CALC(1) + Port[i].CalibrationFactor[V_1];
		DACval += (i & 0x01) ? 0xB000 : 0x3000;
		DAC_set(i, DACval);
		MUX_Sel(i, THREE_VOLT);
	}
	if (Board->BoardType == b427x)
		ADC_MUXsel(Port_4);	//Depending on board connected switch what port is being watched by ADC
	else
		ADC_MUXsel(Port_1);
	calibrateADCval.total = calibrateADCval.average = 0;
	SET_BIT(CalibrationStatusRegister, CALIBRATE_VOLTAGE_SET);
//	Command = 0xC0;	//TODO: See if pulling this out into its own routine was okay
//	BoardCommsParameters[0] = 0x50;
//	BoardCommsParametersLength = 1;
//	communication_array(Command, &BoardCommsParameters[0], BoardCommsParametersLength);
	if (MuxState)
		HAL_GPIO_WritePin(MUX_A0_GPIO_Port, MUX_A0_Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(MUX_A0_GPIO_Port, MUX_A0_Pin, GPIO_PIN_RESET);
}


	// Puts 20mA on all sensor ports
void TargetBoardCalibration_Current(TboardConfig *Board) {
	uint16 DACval;
//	calibrateADCval.total = calibrateADCval.average = 0;
	for (uint8 i = Port_1; i <= Port_6; i++) {
		DACval = DAC_CURRENT_CALC(20) + Port[i].CalibrationFactor[I_20];
		DACval += (i & 0x01) ? 0xB000 : 0x3000;
		DAC_set(i, DACval);
		MUX_Sel(i, TWENTY_AMP);
	}
	SET_BIT(CalibrationStatusRegister, CALIBRATE_CURRENT_SET);
}
