#include <main.h>
#include "calibration.h"
#include "interogate_project.h"
#include "UART_Routine.h"
#include "DAC_Variables.h"

void Calibration(){
	HAL_GPIO_WritePin(MUX_RS_GPIO_Port, MUX_RS_Pin, GPIO_PIN_SET);
	// Set All Ports to 1V
	calTest = V_1;
	calPort = Port_1;
	int8 * calPtr;
	read_correctionFactors();
	calPtr = &CorrectionFactors[0];

	DACval = 0x3000 + DAC_1volt + *calPtr;
	DAC_set((calPort), DACval);
	MUX_Sel((calPort), THREE_VOLT);
	sprintf(Buffer, "==========Calibrating 1V==========\n");
	CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
	sprintf(Buffer, "Port Calibrating: %d\n", calPort+1);
	CDC_Transmit_FS(&Buffer[0], strlen(Buffer));

	while(calTest != Done){
			if (KP_hash.Pressed) {
				KP_hash.Count = KP_hash.Pressed = 0;
				calPtr = &CorrectionFactors[0];
				for(int i = 0; i < 35; i++)
					CorrectionFactors[i] = ~(*calPtr++);
				sprintf(Buffer, "Writing To EPROM\n");
				CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
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
						sprintf(Buffer, "==========Calibrating 1V========== \n\n");
						CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
						  LCD_setCursor(3, 0);
						  sprintf(Buffer, "    1V - Port %d     ", (calPort+1));
						  LCD_printf(&Buffer[0], strlen(Buffer));
						break;
				case V_05:
						sprintf(Buffer, "==========Calibrating 0.5V========== \n\n");
						CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
						DACval = DAC_05volt;
						LCD_setCursor(3, 0);
						sprintf(Buffer, "   0.5V - Port %d    ", (calPort+1));
						LCD_printf(&Buffer[0], strlen(Buffer));
					break;
				case V_24:
						sprintf(Buffer, "==========Calibrating 2.4V========== \n\n");
						CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
						DACval = DAC_24volt;
						  LCD_setCursor(3, 0);
						  sprintf(Buffer, "   2.4V - Port %d    ", (calPort+1));
						  LCD_printf(&Buffer[0], strlen(Buffer));
					break;
				case I_20:
						sprintf(Buffer, "==========Calibrating 20mA========== \n\n");
						CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
						DACval = DAC_20amp;
						  LCD_setCursor(3, 0);
						  sprintf(Buffer, "   20mA - Port %d    ", (calPort+1));
						  LCD_printf(&Buffer[0], strlen(Buffer));
					break;
				case I_4:
						sprintf(Buffer, "==========Calibrating 4mA========== \n\n");
						CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
						DACval = DAC_4amp;
						  LCD_setCursor(3, 0);
						  sprintf(Buffer, "    4mA - Port %d    ", (calPort+1));
						  LCD_printf(&Buffer[0], strlen(Buffer));
					break;
				case I_175:
						sprintf(Buffer, "==========Calibrating 17.5mA========== \n\n");
						CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
						DACval = DAC_175amp;
						LCD_setCursor(3, 0);
						sprintf(Buffer, "   17.5mA - Port %d  ", (calPort+1));
						LCD_printf(&Buffer[0], strlen(Buffer));
					break;
				case Done:
					sprintf(Buffer, "==========	Done ========== \n\n");
					CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
					break;
				}
				sprintf(Buffer, "Port Calibrating: %d \n\n", (calPort+1));
				CDC_Transmit_FS(&Buffer[0], strlen(Buffer));

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
							sprintf(Buffer, "==========Calibrating 1V========== \n\n");
							CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
							  LCD_setCursor(3, 0);
							  sprintf(Buffer, "    1V - Port %d     ", (calPort+1));
							  LCD_printf(&Buffer[0], strlen(Buffer));
							break;
					case V_05:
							sprintf(Buffer, "==========Calibrating 0.5V========== \n\n");
							CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
							DACval = DAC_05volt;
							  LCD_setCursor(3, 0);
							  sprintf(Buffer, "   0.5V - Port %d    ", (calPort+1));
							  LCD_printf(&Buffer[0], strlen(Buffer));
						break;
					case V_24:
							sprintf(Buffer, "==========Calibrating 2.4V========== \n\n");
							CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
							DACval = DAC_24volt;
							  LCD_setCursor(3, 0);
							  sprintf(Buffer, "   2.4V - Port %d    ", (calPort+1));
							  LCD_printf(&Buffer[0], strlen(Buffer));
						break;
					case I_20:
							sprintf(Buffer, "==========Calibrating 20mA========== \n\n");
							CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
							DACval = DAC_20amp;
							  LCD_setCursor(3, 0);
							  sprintf(Buffer, "   20mA - Port %d    ", (calPort+1));
							  LCD_printf(&Buffer[0], strlen(Buffer));
						break;
					case I_4:
							sprintf(Buffer, "==========Calibrating 4mA========== \n\n");
							CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
							DACval = DAC_4amp;
							  LCD_setCursor(3, 0);
							  sprintf(Buffer, "   4mA - Port %d    ", (calPort+1));
							  LCD_printf(&Buffer[0], strlen(Buffer));
						break;
					case I_175:
							sprintf(Buffer, "==========Calibrating 17.5mA========== \n\n");
							CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
							DACval = DAC_175amp;
							LCD_setCursor(3, 0);
							sprintf(Buffer, "   17.5mA - Port %d  ", (calPort+1));
							LCD_printf(&Buffer[0], strlen(Buffer));
						break;
					case Done:
						sprintf(Buffer, "==========	Done ========== \n\n");
						CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
						break;
					}

				DACval += *calPtr;

				sprintf(Buffer, "Port Calibrating: %d\n", calPort+1);
				CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
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
				if ((calTest == V_05) || (calTest == V_1) || (calTest == V_24))
					MUX_Sel((calPort), THREE_VOLT);
				else
					MUX_Sel((calPort), TWENTY_AMP);
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

				sprintf(Buffer, "%d\n", *calPtr);
				CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
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

				sprintf(Buffer, "%d\n", *calPtr);
				CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
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

				sprintf(Buffer, "%d\n", *calPtr);
				CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
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
				sprintf(Buffer, "%d\n", *calPtr);
				CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
			}
			if (KP_star.Pressed) {
				KP_star.Count = KP_star.Pressed = 0;
				break;
			}
	}
}


void TargetBoardCalibration() {
		uns_ch Command;
			//Set Port 1
		DACval = DAC_1volt + Port1.CalibrationFactor[V_1];
		DACval |= 0x3000;
		DAC_set(Port_1, DACval);
			//Set Port 2
		DACval = DAC_1volt + Port2.CalibrationFactor[V_1];
		DACval |= 0xB000;
		DAC_set(Port_2, DACval);
			//Set Port 3
		DACval = DAC_1volt + Port3.CalibrationFactor[V_1];
		DACval |= 0x3000;
		DAC_set(Port_3, DACval);
			//Set Port 4
		DACval = DAC_1volt + Port4.CalibrationFactor[V_1];
		DACval |= 0xB000;
		DAC_set(Port_4, DACval);
			//Set Port 5
		DACval = DAC_1volt + Port5.CalibrationFactor[V_1];
		DACval |= 0x3000;
		DAC_set(Port_5, DACval);
			//Set Port 6
		DACval = DAC_1volt + Port6.CalibrationFactor[V_1];
		DACval |= 0xB000;
		DAC_set(Port_6, DACval);
	for (int i = 0; i < 6; i++) {
			MUX_Sel(i, THREE_VOLT);
		}
	ADC_MUXsel(0);
	switchToCurrent = false;
	calibrateADCval.total = calibrateADCval.average = 0;
	Calibrating = true;
	Command = 0xC0;
	SetPara(Command);
	communication_array(Command, &Para[0], Paralen);
	CalibratingTimer = 0;
	HAL_TIM_Base_Start(&htim10);
	HAL_TIM_Base_Start_IT(&htim10);
	while(!switchToCurrent && Calibrating) {

	}
	calibrateADCval.total = calibrateADCval.average = 0;
	if (switchToCurrent) {
			HAL_TIM_Base_Stop(&htim10);
				//Set Port 1
			DACval = DAC_20amp + Port1.CalibrationFactor[I_20];
			DACval |= 0x3000;
			DAC_set(Port_1, DACval);
				//Set Port 2
			DACval = DAC_20amp + Port2.CalibrationFactor[I_20];
			DACval |= 0xB000;
			DAC_set(Port_2, DACval);
				//Set Port 3
			DACval = DAC_20amp + Port3.CalibrationFactor[I_20];
			DACval |= 0x3000;
			DAC_set(Port_3, DACval);
				//Set Port 4
			DACval = DAC_20amp + Port4.CalibrationFactor[I_20];
			DACval |= 0xB000;
			DAC_set(Port_4, DACval);
				//Set Port 5
			DACval = DAC_20amp + Port5.CalibrationFactor[I_20];
			DACval |= 0x3000;
			DAC_set(Port_5, DACval);
				//Set Port 6
			DACval = DAC_20amp + Port6.CalibrationFactor[I_20];
			DACval |= 0xB000;
			DAC_set(Port_6, DACval);

		for (int i = 0; i <= 5; i++) {
				MUX_Sel(i, TWENTY_AMP);
			}
		while ( (!UART2_ReceiveComplete) && Calibrating) {

		}
		if (UART2_ReceiveComplete) {
			communication_response(&Command, &UART2_Receive[0], UART2_RecPos);
			sprintf(Buffer, "=====     Target Board Calibrated     =====\n");
			HAL_UART_Transmit(&huart1, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);
			Calibrating = false;
		}
	}

}
