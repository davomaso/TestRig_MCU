#include <main.h>
#include "interogate_project.h"

void LatchErrorCheck(TboardConfig * Board){
	/*
	 * Latch error routine
	 * This routine determines whether the latch test passes, if the values calculated by the ADC fall outside of the
	 * range specified here, then the bits in the LatchErrCheck register are truggered, flagging the errors for when the
	 * failures are presented.
	 */
	CLEAR_REG(LatchTestErrorRegister);
		//ADC1 Check
	if( ( (adc1.HighPulseWidth > 52) || (adc1.HighPulseWidth < 48) ) || ( (adc1.LowPulseWidth > 52) || (adc1.LowPulseWidth < 48) ) )
		SET_BIT(Board->LTR, PORT_A_PULSEWIDTH_ERROR);
	if (Board->BoardType == b935x || Board->BoardType == b937x) {
		if((adc1.highVoltage <= 9.8)  || (adc1.lowVoltage >= 0.7) || (adc1.highVoltage == 0))
			SET_BIT(Board->LTR, PORT_A_VOLTAGE_ERROR);
		if((adc2.highVoltage <= 9.8) || (adc2.lowVoltage >= 0.7) || (adc2.highVoltage == 0))
			SET_BIT(Board->LTR, PORT_B_VOLTAGE_ERROR);
	} else {
		if((adc1.highVoltage <= 9.2)  || (adc1.lowVoltage >= 0.7) || (adc1.highVoltage == 0))
			SET_BIT(Board->LTR, PORT_A_VOLTAGE_ERROR);
		if((adc2.highVoltage <= 9.2 ) || (adc2.lowVoltage >= 0.7) || (adc2.highVoltage == 0))
			SET_BIT(Board->LTR, PORT_B_VOLTAGE_ERROR);
	}
		//ADC2 Check
	if( ( (adc2.HighPulseWidth > 52) || (adc2.HighPulseWidth < 48) ) || ( (adc2.LowPulseWidth > 52) || (adc2.LowPulseWidth < 48) ) )
		SET_BIT(Board->LTR, PORT_B_PULSEWIDTH_ERROR);
		//Vin Check
	if( Vin.steadyState < 10.8 )
		SET_BIT(Board->LTR, INPUT_VOLTAGE_ERROR);
		//Vfuse Check
	if(Vfuse.steadyState < 0.95*Vin.steadyState  || Vfuse.lowVoltage < 0.9*Vin.lowVoltage)
		SET_BIT(Board->LTR, FUSE_VOLTAGE_ERROR);
		//Vmos Check
	if(Vmos1.highVoltage < 0.05 || Vmos1.highVoltage > 3 || Vmos1.lowVoltage > 0.7 || Vmos1.lowVoltage < 0.1)
		SET_BIT(Board->LTR, PORT_A_MOSFET_ERROR);
	if(Vmos2.highVoltage < 0.05 || Vmos2.highVoltage > 3 || Vmos2.lowVoltage > 0.7 || Vmos2.lowVoltage < 0.1)
		SET_BIT(Board->LTR, PORT_B_MOSFET_ERROR);
}
void printLatchError(uint8 *ErrorCode){
	/*
	 * Prints the errors that are determined above. Dependant on the code that is set by the routines above will
	 * determine the errors that are printed to the terminal.
	 */
	if(*ErrorCode & PORT_A_PULSEWIDTH_ERROR) {
		sprintf(debugTransmitBuffer, "\n******** ERROR--PortA Pulse Width ********\n");
		CDC_Transmit_FS(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
		  HAL_UART_Transmit(&huart1, &debugTransmitBuffer[0], strlen(debugTransmitBuffer), HAL_MAX_DELAY);
	}
	if(*ErrorCode & PORT_A_VOLTAGE_ERROR) {
		sprintf(debugTransmitBuffer, "\n******** ERROR--PortA Voltage ********\n");
		CDC_Transmit_FS(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
		  HAL_UART_Transmit(&huart1, &debugTransmitBuffer[0], strlen(debugTransmitBuffer), HAL_MAX_DELAY);
	}
	if(*ErrorCode & PORT_B_PULSEWIDTH_ERROR) {
		sprintf(debugTransmitBuffer, "\n******** ERROR--PortB Pulse Width ********\n");
		CDC_Transmit_FS(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
		HAL_UART_Transmit(&huart1, &debugTransmitBuffer[0], strlen(debugTransmitBuffer), HAL_MAX_DELAY);
	}
	if(*ErrorCode & PORT_B_VOLTAGE_ERROR) {
		sprintf(debugTransmitBuffer, "\n******** ERROR--PortB Voltage ********\n");
		CDC_Transmit_FS(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
		  HAL_UART_Transmit(&huart1, &debugTransmitBuffer[0], strlen(debugTransmitBuffer), HAL_MAX_DELAY);
	}
	if(*ErrorCode & INPUT_VOLTAGE_ERROR) {
		sprintf(debugTransmitBuffer, "\n******** ERROR--Input Voltage ********\n");
		CDC_Transmit_FS(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
		  HAL_UART_Transmit(&huart1, &debugTransmitBuffer[0], strlen(debugTransmitBuffer), HAL_MAX_DELAY);
	}
	if(*ErrorCode & FUSE_VOLTAGE_ERROR) {
		sprintf(debugTransmitBuffer, "\n******** ERROR--Fuse Voltage ********\n");
		CDC_Transmit_FS(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
		  HAL_UART_Transmit(&huart1, &debugTransmitBuffer[0], strlen(debugTransmitBuffer), HAL_MAX_DELAY);
	}
	if(*ErrorCode & PORT_A_MOSFET_ERROR) {
		sprintf(debugTransmitBuffer, "\n******** ERROR--MOSFET 1 Voltage ********\n");
		CDC_Transmit_FS(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
		  HAL_UART_Transmit(&huart1, &debugTransmitBuffer[0], strlen(debugTransmitBuffer), HAL_MAX_DELAY);
	}
	if(*ErrorCode & PORT_B_MOSFET_ERROR) {
		sprintf(debugTransmitBuffer, "\n******** ERROR--MOSFET 2 Voltage ********\n");
		CDC_Transmit_FS(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
		  HAL_UART_Transmit(&huart1, &debugTransmitBuffer[0], strlen(debugTransmitBuffer), HAL_MAX_DELAY);
	}
}
