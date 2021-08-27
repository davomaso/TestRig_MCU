#include <main.h>
#include "calibration.h"
#include "interogate_project.h"
#include "UART_Routine.h"
#include "DAC_Variables.h"

void Calibration(){
	uint16 DACval;
	uint8 calPort;
	TcalTestConfig calTest;

//	LCD_Clear();
//	LCD_printf("Test Rig", 1, 6);
	LCD_printf("Calibrate Test Rig", 2, 0);
	LCD_printf("1V - Port 1", 3, 5);
	printT("\n\n==========  Calibration Routine  ==========\n");
	calTest = V_1;
	calPort = Port_1;
	read_correctionFactors();

	DACval = 0x3000 + DAC_1volt + Port[Port_1].CalibrationFactor[V_1];
	DAC_set((calPort), DACval);
	MUX_Sel((calPort), THREE_VOLT);
	printT("==========Calibrating 1V==========\n");
	sprintf(&debugTransmitBuffer, "Port Calibrating: %d\n", calPort+1);
	printT(&debugTransmitBuffer[0]);

	while (calTest != Done) {
		if (KP[hash].Pressed  || KP[star].Pressed) {
			if (KP[hash].Pressed) {
			KP[hash].Pressed = false;
				printT("Writing To EPROM\n");
				write_correctionFactors();
				read_correctionFactors();
			} else
				KP[star].Pressed = false;
			calTest = Done;
			break;
		}
		if (KP[4].Pressed || KP[6].Pressed) {
			if (KP[4].Pressed) {
				KP[4].Pressed = 0;
						//Decrement Port
				if (calPort != Port_1) {
					calPort--;
				} else {
					if (calTest == V_1){
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
				switch (calTest) {
					case V_1:
						DACval = DAC_1volt;
						printT("==========Calibrating 1V========== \n\n");
						sprintf(&lcdBuffer[0], "1V - Port %d     ", (calPort+1));
						LCD_printf(&lcdBuffer[0], 3, 5);
						break;
				case V_05:
						printT("==========Calibrating 0.5V========== \n\n");
						DACval = DAC_05volt;
						sprintf(&lcdBuffer[0], "0.5V - Port %d    ", (calPort+1));
						LCD_printf(&lcdBuffer[0], 3, 3);
					break;
				case V_24:
						printT("==========Calibrating 2.4V========== \n\n");
						DACval = DAC_24volt;
						sprintf(&lcdBuffer[0], "2.4V - Port %d    ", (calPort+1));
						LCD_printf(&lcdBuffer, 3, 3);
					break;
				case I_20:
						printT("==========Calibrating 20mA========== \n\n");
						DACval = DAC_20amp;
						sprintf(&lcdBuffer[0], "20mA - Port %d    ", (calPort+1));
						LCD_printf(&lcdBuffer[0], 3, 3);
					break;
				case I_4:
						printT("==========Calibrating 4mA========== \n\n");
						DACval = DAC_4amp;
						sprintf(&lcdBuffer[0], "4mA - Port %d    ", (calPort+1));
						LCD_printf(&lcdBuffer[0], 3, 4);
					break;
				case I_175:
						printT("==========Calibrating 17.5mA========== \n\n");
						DACval = DAC_175amp;
						sprintf(&lcdBuffer[0], "17.5mA - Port %d  ", (calPort+1));
						LCD_printf(&lcdBuffer[0], 3, 1);
					break;
				case Done:
					printT("==========	Done ========== \n\n");
					break;
				}
				sprintf(debugTransmitBuffer, "Port Calibrating: %d \n\n", (calPort+1));
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
		if (KP[2].Pressed || KP[8].Pressed  || KP[3].Pressed || KP[9].Pressed) {
			if (KP[2].Pressed) {
				//Increment CF by 1
				KP[2].Pressed = false;
				(Port[calPort].CalibrationFactor[calTest])++;
				DACval++;
			} else if (KP[8].Pressed) {
				KP[8].Pressed = 0;
				(Port[calPort].CalibrationFactor[calTest])--;
				DACval--;
			} else if (KP[3].Pressed) {
				//Increment CF by 10
				KP[3].Pressed = 0;
				Port[calPort].CalibrationFactor[calTest] += 10;
				DACval += 10;
			} else if (KP[9].Pressed) {
				// Decrement CF by 10
				KP[9].Pressed = 0;
				Port[calPort].CalibrationFactor[calTest] -= 10;
				DACval -= 10;
			}

			if (calPort == Port_1 || calPort == Port_3 || calPort == Port_5) {
				DACval &= 0x0FFF;
				DACval |= 0x3000;
			} else if (calPort == Port_2 || calPort == Port_4 || calPort == Port_6) {
				DACval &= 0x0FFF;
				DACval |= 0xB000;
			}
			DAC_set((calPort), DACval);
			sprintf(debugTransmitBuffer, "%d\n", Port[calPort].CalibrationFactor[calTest]);
			printT(&debugTransmitBuffer[0]);
		}
	}
}

void TargetBoardCalibration_Voltage(TboardConfig * Board) {
	uint16 DACval;
	uns_ch Command;
	_Bool MuxState = HAL_GPIO_ReadPin(MUX_A0_GPIO_Port, MUX_A0_Pin);
	for (uint8 i = Port_1; i < Port_6; i++) {
		DACval = DAC_1volt + Port[i].CalibrationFactor[V_1];
		DACval += (i & 0x01) ? 0xB000:0x3000;
		DAC_set(i, DACval);
		MUX_Sel(i, THREE_VOLT);
	}
	if (Board->BoardType == b427x)
		ADC_MUXsel(Port_4);	//Depending on board connected switch what port is being watched by ADC
	else
		ADC_MUXsel(Port_1);
	calibrateADCval.total = calibrateADCval.average = 0;
	SET_BIT(CalibrationStatusRegister, CALIBRATE_VOLTAGE_SET);
	Command = 0xC0;
	BoardCommsParameters[0] = 0x50;
	BoardCommsParametersLength = 1;
	communication_array(Command, &BoardCommsParameters[0], BoardCommsParametersLength);
	CalibratingTimer = 0;
	if (MuxState)
		HAL_GPIO_WritePin(MUX_A0_GPIO_Port, MUX_A0_Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(MUX_A0_GPIO_Port, MUX_A0_Pin, GPIO_PIN_RESET);
}

void TargetBoardCalibration_Current(TboardConfig * Board) {
	uint16 DACval;
	calibrateADCval.total = calibrateADCval.average = 0;
	for (uint8 i = Port_1;i <= Port_6;i++) {
		DACval = DAC_20amp + Port[i].CalibrationFactor[I_20];
		DACval += (i & 0x01) ? 0xB000:0x3000;
		DAC_set(i, DACval);
		MUX_Sel(i, TWENTY_AMP);
	}
	SET_BIT(CalibrationStatusRegister, CALIBRATE_CURRENT_SET);
}
