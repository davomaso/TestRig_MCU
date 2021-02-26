#include <main.h>
#include "calibration.h"
#include "interogate_project.h"

void Calibration(){
	HAL_GPIO_WritePin(MUX_RS_GPIO_Port, MUX_RS_Pin, GPIO_PIN_SET);
	// Set All Ports to 1V
	calTest = V_1;
	calPort = Port_1;
	int8 * calPtr;
	calPtr = &CorrectionFactors[0];

	DACval = 0x3000 + 0x457 + *calPtr;
	DAC_set((calPort+1), DACval);
	MUX_Sel((calPort+1), THREE_VOLT);
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
						DACval = 0x457;
						sprintf(Buffer, "==========Calibrating 1V========== \n\n");
						CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
						  LCD_setCursor(3, 0);
						  sprintf(Buffer, "    1V - Port %d     ", (calPort+1));
						  LCD_printf(&Buffer[0], strlen(Buffer));
						break;
				case V_05:
						sprintf(Buffer, "==========Calibrating 0.5V========== \n\n");
						CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
						DACval = 0x22C;
						LCD_setCursor(3, 0);
						sprintf(Buffer, "   0.5V - Port %d    ", (calPort+1));
						LCD_printf(&Buffer[0], strlen(Buffer));
					break;
				case V_24:
						sprintf(Buffer, "==========Calibrating 2.4V========== \n\n");
						CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
						DACval = 0xA6B;
						  LCD_setCursor(3, 0);
						  sprintf(Buffer, "   2.4V - Port %d    ", (calPort+1));
						  LCD_printf(&Buffer[0], strlen(Buffer));
					break;
				case I_20:
						sprintf(Buffer, "==========Calibrating 20mA========== \n\n");
						CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
						DACval = 0xB87;
						  LCD_setCursor(3, 0);
						  sprintf(Buffer, "   20mA - Port %d    ", (calPort+1));
						  LCD_printf(&Buffer[0], strlen(Buffer));
					break;
				case I_4:
						sprintf(Buffer, "==========Calibrating 4mA========== \n\n");
						CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
						DACval = 0x24E;
						  LCD_setCursor(3, 0);
						  sprintf(Buffer, "    4mA - Port %d    ", (calPort+1));
						  LCD_printf(&Buffer[0], strlen(Buffer));
					break;
				case I_175:
						sprintf(Buffer, "==========Calibrating 17.5mA========== \n\n");
						CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
						DACval = 0xA16;
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
				DAC_set((calPort+1), DACval);

				if (calTest < 3)
					MUX_Sel((calPort+1), THREE_VOLT);
				else
					MUX_Sel((calPort+1), TWENTY_AMP);

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
							DACval = 0x457;
							sprintf(Buffer, "==========Calibrating 1V========== \n\n");
							CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
							  LCD_setCursor(3, 0);
							  sprintf(Buffer, "    1V - Port %d     ", (calPort+1));
							  LCD_printf(&Buffer[0], strlen(Buffer));
							break;
					case V_05:
							sprintf(Buffer, "==========Calibrating 0.5V========== \n\n");
							CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
							DACval = 0x22C;
							  LCD_setCursor(3, 0);
							  sprintf(Buffer, "   0.5V - Port %d    ", (calPort+1));
							  LCD_printf(&Buffer[0], strlen(Buffer));
						break;
					case V_24:
							sprintf(Buffer, "==========Calibrating 2.4V========== \n\n");
							CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
							DACval = 0xA6B;
							  LCD_setCursor(3, 0);
							  sprintf(Buffer, "   2.4V - Port %d    ", (calPort+1));
							  LCD_printf(&Buffer[0], strlen(Buffer));
						break;
					case I_20:
							sprintf(Buffer, "==========Calibrating 20mA========== \n\n");
							CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
							DACval = 0xB87;
							  LCD_setCursor(3, 0);
							  sprintf(Buffer, "   20mA - Port %d    ", (calPort+1));
							  LCD_printf(&Buffer[0], strlen(Buffer));
						break;
					case I_4:
							sprintf(Buffer, "==========Calibrating 4mA========== \n\n");
							CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
							DACval = 0x24E;
							  LCD_setCursor(3, 0);
							  sprintf(Buffer, "   4mA - Port %d    ", (calPort+1));
							  LCD_printf(&Buffer[0], strlen(Buffer));
						break;
					case I_175:
							sprintf(Buffer, "==========Calibrating 17.5mA========== \n\n");
							CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
							DACval = 0xA16;
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
				if (calPort == Port_1 || calPort == Port_3 || calPort == Port_5)
						DACval |= 0x3000;
				else if (calPort == Port_2 || calPort == Port_4 || calPort == Port_6)
						DACval |= 0xB000;
				DAC_set((calPort+1), DACval);
						//Switch Multiplexer to Test being Run
				if ((calTest == V_05) || (calTest == V_1) || (calTest == V_24))
					MUX_Sel((calPort+1), THREE_VOLT);
				else
					MUX_Sel((calPort+1), TWENTY_AMP);
			}
			if (KP_2.Pressed) {
				//Increment CF by 1
				KP_2.Count = KP_2.Pressed = 0;
				(*calPtr)++;
				DACval++;
					//Write New Val to DAC
				if (calPort == Port_1 || calPort == Port_3 || calPort == Port_5)
						DACval |= 0x3000;
				else if (calPort == Port_2 || calPort == Port_4 || calPort == Port_6)
						DACval |= 0xB000;
				DAC_set((calPort+1), DACval);

				sprintf(Buffer, "%d\n", *calPtr);
				CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
			}
			if (KP_8.Pressed) {
				//Decrement CF by 1
				KP_8.Count = KP_8.Pressed = 0;
				(*calPtr)--;
				DACval--;
					//Write New Val to DAC
				if (calPort == Port_1 || calPort == Port_3 || calPort == Port_5)
						DACval |= 0x3000;
				else if (calPort == Port_2 || calPort == Port_4 || calPort == Port_6)
						DACval |= 0xB000;
				DAC_set((calPort+1), DACval);

				sprintf(Buffer, "%d\n", *calPtr);
				CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
			}
			if (KP_3.Pressed) {
				//Increment CF by 10
				KP_3.Count = KP_3.Pressed = 0;
				(*calPtr) += 10;
				DACval += 10;

					//Write New Val to DAC
				if (calPort == Port_1 || calPort == Port_3 || calPort == Port_5)
					DACval |= 0x3000;
				else if (calPort == Port_2 || calPort == Port_4 || calPort == Port_6)
					DACval |= 0xB000;
				DAC_set((calPort+1), DACval);

				sprintf(Buffer, "%d\n", *calPtr);
				CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
			}
			if (KP_9.Pressed) {
				// Decrement CF by 10
				KP_9.Count = KP_9.Pressed = 0;
				(*calPtr) -= 10;
				DACval -= 10;

					//Write New Val to DAC
				if (calPort == Port_1 || calPort == Port_3 || calPort == Port_5)
					DACval |= 0x3000;
				else if (calPort == Port_2 || calPort == Port_4 || calPort == Port_6)
					DACval |= 0xB000;
				DAC_set((calPort+1), DACval);
				sprintf(Buffer, "%d\n", *calPtr);
				CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
			}
			if (KP_star.Pressed) {
				KP_star.Count = KP_star.Pressed = 0;
				break;
			}
	}
}
