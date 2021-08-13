#include <main.h>
#include "calibration.h"
#include "interogate_project.h"
#include "UART_Routine.h"
#include "DAC_Variables.h"

void Calibration(){
	int8 * calPtr;
	uint16 DACval;
	uint8 calPort;
	TcalTestConfig calTest;

	HAL_GPIO_WritePin(MUX_RS_GPIO_Port, MUX_RS_Pin, GPIO_PIN_SET);
	// Set All Ports to 1V
	calTest = V_1;
	calPort = Port_1;

	read_correctionFactors();
	calPtr = &CorrectionFactors[0];

	DACval = 0x3000 + DAC_1volt + *calPtr;
	DAC_set((calPort), DACval);
	MUX_Sel((calPort), THREE_VOLT);
	sprintf(&debugTransmitBuffer, "==========Calibrating 1V==========\n");
	printT(&debugTransmitBuffer[0]);
	sprintf(&debugTransmitBuffer, "Port Calibrating: %d\n", calPort+1);
	printT(&debugTransmitBuffer[0]);

	while(calTest != Done){
			if (KP_hash.Pressed) {
				KP_hash.Count = KP_hash.Pressed = 0;
				calPtr = &CorrectionFactors[0];
				for(int i = 0; i < 35; i++)
					CorrectionFactors[i] = ~(*calPtr++);
				printT("Writing To EPROM\n");
				write_correctionFactors();
				read_correctionFactors();
				break;
			}
			if (KP_4.Pressed) {
				KP_4.Count = KP_4.Pressed = 0;

				//Decrement Port
				if (calPort != Port_1) {
					calPort--;
					calPtr--;
				} else {
					if (calTest == V_1){
						calTest = I_175;
						calPort = Port_6;
						calPtr += 35;
					} else {
						calTest--;
						calPort = Port_6;
						calPtr--;
					}
				}
				switch (calTest) {
					case V_1:
						DACval = DAC_1volt;
						printT("==========Calibrating 1V========== \n\n");
						LCD_setCursor(3, 0);
						sprintf(debugTransmitBuffer, "    1V - Port %d     ", (calPort+1));
						LCD_displayString(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
						break;
				case V_05:
						printT("==========Calibrating 0.5V========== \n\n");
						DACval = DAC_05volt;
						LCD_setCursor(3, 0);
						sprintf(debugTransmitBuffer, "   0.5V - Port %d    ", (calPort+1));
						LCD_displayString(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
					break;
				case V_24:
						printT("==========Calibrating 2.4V========== \n\n");
						DACval = DAC_24volt;
						LCD_setCursor(3, 0);
						sprintf(debugTransmitBuffer, "   2.4V - Port %d    ", (calPort+1));
						LCD_displayString(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
					break;
				case I_20:
						printT("==========Calibrating 20mA========== \n\n");
						DACval = DAC_20amp;
						LCD_setCursor(3, 0);
						sprintf(debugTransmitBuffer, "   20mA - Port %d    ", (calPort+1));
						LCD_displayString(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
					break;
				case I_4:
						printT("==========Calibrating 4mA========== \n\n");
						DACval = DAC_4amp;
						LCD_setCursor(3, 0);
						sprintf(debugTransmitBuffer, "    4mA - Port %d    ", (calPort+1));
						LCD_displayString(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
					break;
				case I_175:
						printT("==========Calibrating 17.5mA========== \n\n");
						DACval = DAC_175amp;
						LCD_setCursor(3, 0);
						sprintf(debugTransmitBuffer, "   17.5mA - Port %d  ", (calPort+1));
						LCD_displayString(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
					break;
				case Done:
					printT("==========	Done ========== \n\n");
					break;
				}
				sprintf(debugTransmitBuffer, "Port Calibrating: %d \n\n", (calPort+1));
				printT(&debugTransmitBuffer[0]);

				DACval += *calPtr;
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
			if (KP_6.Pressed) {
				KP_6.Count = KP_6.Pressed = 0;
					//Set New DAC values
					if (calPort != Port_6) {
							calPort++;
							calPtr++;
					} else {
						if (calTest != I_175) {
							calTest++;
							calPort = Port_1;
							calPtr++;
						} else {
							calTest = V_1;
							calPort = Port_1;
							calPtr -= 35;
						}
					}
					switch (calTest) {
						case V_1:
								DACval = DAC_1volt;
								sprintf(debugTransmitBuffer, "==========Calibrating 1V========== \n\n");
								printT(&debugTransmitBuffer[0]);
								DACval = DAC_1volt;
								LCD_setCursor(3, 0);
								sprintf(debugTransmitBuffer, "    1V - Port %d     ", (calPort+1));
								LCD_displayString(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
							break;
					case V_05:
								sprintf(debugTransmitBuffer, "==========Calibrating 0.5V========== \n\n");
								printT(&debugTransmitBuffer[0]);
								DACval = DAC_05volt;
								LCD_setCursor(3, 0);
								sprintf(debugTransmitBuffer, "   0.5V - Port %d    ", (calPort+1));
								LCD_displayString(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
						break;
					case V_24:
								sprintf(debugTransmitBuffer, "==========Calibrating 2.4V========== \n\n");
								printT(&debugTransmitBuffer[0]);
								DACval = DAC_24volt;
								LCD_setCursor(3, 0);
								sprintf(debugTransmitBuffer, "   2.4V - Port %d    ", (calPort+1));
								LCD_displayString(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
						break;
					case I_20:
								sprintf(debugTransmitBuffer, "==========Calibrating 20mA========== \n\n");
								printT(&debugTransmitBuffer[0]);
								DACval = DAC_20amp;
								LCD_setCursor(3, 0);
								sprintf(debugTransmitBuffer, "   20mA - Port %d    ", (calPort+1));
								LCD_displayString(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
						break;
					case I_4:
								sprintf(debugTransmitBuffer, "==========Calibrating 4mA========== \n\n");
								printT(&debugTransmitBuffer[0]);
								DACval = DAC_4amp;
								LCD_setCursor(3, 0);
								sprintf(debugTransmitBuffer, "   4mA - Port %d    ", (calPort+1));
								LCD_displayString(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
						break;
					case I_175:
								sprintf(debugTransmitBuffer, "==========Calibrating 17.5mA========== \n\n");
								printT(&debugTransmitBuffer[0]);
								DACval = DAC_175amp;
								LCD_setCursor(3, 0);
								sprintf(debugTransmitBuffer, "   17.5mA - Port %d  ", (calPort+1));
								LCD_displayString(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
						break;
					case Done:
								sprintf(debugTransmitBuffer, "==========	Done ========== \n\n");
								printT(&debugTransmitBuffer[0]);
						break;
					}

				DACval += *calPtr;

				sprintf(debugTransmitBuffer, "Port Calibrating: %d\n", calPort+1);
				printT(&debugTransmitBuffer[0]);
					//Write New Val To DAC
				if (calPort == Port_1 || calPort == Port_3 || calPort == Port_5) {
					DACval &= 0x0FFF;
					DACval |= 0x3000;
				}
				else if (calPort == Port_2 || calPort == Port_4 || calPort == Port_6) {
					DACval &= 0x0FFF;
					DACval |= 0xB000;
				}
				DAC_set((calPort), DACval);
						//Switch Multiplexer to Test being Run
				if ( calTest <3 )
					MUX_Sel((calPort), THREE_VOLT);
				else
					MUX_Sel(calPort, TWENTY_AMP);
			}
			if (KP_2.Pressed) {
				//Increment CF by 1
				KP_2.Count = KP_2.Pressed = 0;
				(*calPtr)++;
				DACval++;
					//Write New Val to DAC
				if (calPort == Port_1 || calPort == Port_3 || calPort == Port_5) {
					DACval &= 0x0FFF;
					DACval |= 0x3000;
				}
				else if (calPort == Port_2 || calPort == Port_4 || calPort == Port_6) {
					DACval &= 0x0FFF;
					DACval |= 0xB000;
				}
				DAC_set((calPort), DACval);

				sprintf(debugTransmitBuffer, "%d\n", *calPtr);
				CDC_Transmit_FS(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
			}
			if (KP_8.Pressed) {
				//Decrement CF by 1
				KP_8.Count = KP_8.Pressed = 0;
				(*calPtr)--;
				DACval--;
					//Write New Val to DAC
				if (calPort == Port_1 || calPort == Port_3 || calPort == Port_5) {
					DACval &= 0x0FFF;
					DACval |= 0x3000;
				}
				else if (calPort == Port_2 || calPort == Port_4 || calPort == Port_6) {
					DACval &= 0x0FFF;
					DACval |= 0xB000;
				}
				DAC_set((calPort), DACval);

				sprintf(debugTransmitBuffer, "%d\n", *calPtr);
				printT(&debugTransmitBuffer[0]);
			}
			if (KP_3.Pressed) {
				//Increment CF by 10
				KP_3.Count = KP_3.Pressed = 0;
				(*calPtr) += 10;
				DACval += 10;

					//Write New Val to DAC
				if (calPort == Port_1 || calPort == Port_3 || calPort == Port_5) {
					DACval &= 0x0FFF;
					DACval |= 0x3000;
				}
				else if (calPort == Port_2 || calPort == Port_4 || calPort == Port_6) {
					DACval &= 0x0FFF;
					DACval |= 0xB000;
				}
				DAC_set((calPort), DACval);

				sprintf(debugTransmitBuffer, "%d\n", *calPtr);
				CDC_Transmit_FS(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
			}
			if (KP_9.Pressed) {
				// Decrement CF by 10
				KP_9.Count = KP_9.Pressed = 0;
				(*calPtr) -= 10;
				DACval -= 10;

					//Write New Val to DAC
				if (calPort == Port_1 || calPort == Port_3 || calPort == Port_5) {
					DACval &= 0x0FFF;
					DACval |= 0x3000;
				}
				else if (calPort == Port_2 || calPort == Port_4 || calPort == Port_6) {
					DACval &= 0x0FFF;
					DACval |= 0xB000;
				}
				DAC_set((calPort), DACval);
				sprintf(debugTransmitBuffer, "%d\n", *calPtr);
				CDC_Transmit_FS(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
			}
			if (KP_star.Pressed) {
				KP_star.Count = KP_star.Pressed = 0;
				break;
			}
	}
}

void TargetBoardCalibration_Voltage(TboardConfig * Board) {
	uint16 DACval;
	uns_ch Command;
	_Bool MuxState = HAL_GPIO_ReadPin(MUX_A0_GPIO_Port, MUX_A0_Pin);
		Command = 0xC0;
		Para[0] = 0x50;
		Paralen = 1;
			//Set Port 1
		DACval = DAC_1volt + Port1.CalibrationFactor[V_1];
		DACval += 0x3000;
		DAC_set(Port_1, DACval);
		MUX_Sel(Port_1, THREE_VOLT);
			//Set Port 2
		DACval = DAC_1volt + Port2.CalibrationFactor[V_1];
		DACval += 0xB000;
		DAC_set(Port_2, DACval);
		MUX_Sel(Port_2, THREE_VOLT);
			//Set Port 3
		DACval = DAC_1volt + Port3.CalibrationFactor[V_1];
		DACval += 0x3000;
		DAC_set(Port_3, DACval);
		MUX_Sel(Port_3, THREE_VOLT);
			//Set Port 4
		DACval = DAC_1volt + Port4.CalibrationFactor[V_1];
		DACval += 0xB000;
		DAC_set(Port_4, DACval);
		MUX_Sel(Port_4, THREE_VOLT);
			//Set Port 5
		DACval = DAC_1volt + Port5.CalibrationFactor[V_1];
		DACval += 0x3000;
		DAC_set(Port_5, DACval);
		MUX_Sel(Port_5, THREE_VOLT);
			//Set Port 6
		DACval = DAC_1volt + Port6.CalibrationFactor[V_1];
		DACval += 0xB000;
		DAC_set(Port_6, DACval);
		MUX_Sel(Port_6, THREE_VOLT);

		if (Board->BoardType == b427x)
			ADC_MUXsel(Port_4);	//Depending on board connected switch what port is being watched by ADC
		else
			ADC_MUXsel(Port_1);
		calibrateADCval.total = calibrateADCval.average = 0;
		SET_BIT(CalibrationStatusRegister, CALIBRATE_VOLTAGE_SET);
//		HAL_Delay(125);
		communication_array(Command, &Para[0], Paralen);
		CalibratingTimer = 0;
		if (MuxState)
			HAL_GPIO_WritePin(MUX_A0_GPIO_Port, MUX_A0_Pin, GPIO_PIN_SET);
		else
			HAL_GPIO_WritePin(MUX_A0_GPIO_Port, MUX_A0_Pin, GPIO_PIN_RESET);
}

void TargetBoardCalibration_Current(TboardConfig * Board) {
	uint16 DACval;
	_Bool MuxState = HAL_GPIO_ReadPin(MUX_A0_GPIO_Port, MUX_A0_Pin);
	calibrateADCval.total = calibrateADCval.average = 0;
			//Set Port 1
	DACval = DAC_20amp + Port1.CalibrationFactor[I_20];
	DACval += 0x3000;
	DAC_set(Port_1, DACval);
	MUX_Sel(Port_1, TWENTY_AMP);
		//Set Port 2
	DACval = DAC_20amp + Port2.CalibrationFactor[I_20];
	DACval += 0xB000;
	DAC_set(Port_2, DACval);
	MUX_Sel(Port_2, TWENTY_AMP);
		//Set Port 3
	DACval = DAC_20amp + Port3.CalibrationFactor[I_20];
	DACval += 0x3000;
	DAC_set(Port_3, DACval);
	MUX_Sel(Port_3, TWENTY_AMP);
		//Set Port 4
	DACval = DAC_20amp + Port4.CalibrationFactor[I_20];
	DACval += 0xB000;
	DAC_set(Port_4, DACval);
	MUX_Sel(Port_4, TWENTY_AMP);
		//Set Port 5
	DACval = DAC_20amp + Port5.CalibrationFactor[I_20];
	DACval += 0x3000;
	DAC_set(Port_5, DACval);
	MUX_Sel(Port_5, TWENTY_AMP);
		//Set Port 6
	DACval = DAC_20amp + Port6.CalibrationFactor[I_20];
	DACval +=0xB000;
	DAC_set(Port_6, DACval);
	MUX_Sel(Port_6, TWENTY_AMP);

	if (MuxState)
		HAL_GPIO_WritePin(MUX_A0_GPIO_Port, MUX_A0_Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(MUX_A0_GPIO_Port, MUX_A0_Pin, GPIO_PIN_RESET);
	SET_BIT(CalibrationStatusRegister, CALIBRATE_CURRENT_SET);
}

