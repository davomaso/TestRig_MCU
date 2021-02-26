#include <main.h>
#include "interogate_project.h"

int8_t LatchErrorCheck(){
	LatchErrCheck = 0x00;
		//ADC1 Check
	if((adc1.HighPulseWidth > 52 || adc1.HighPulseWidth < 48) || (adc1.LowPulseWidth > 52 || adc1.LowPulseWidth < 48) )
		LatchErrCheck |= 1;
	if(adc1.highVoltage <= 10  || adc1.lowVoltage >= 0.7 || adc1.highVoltage == 0)
		LatchErrCheck |= 2;

		//ADC2 Check
	if(adc2.HighPulseWidth > 52 || adc2.HighPulseWidth < 48|| (adc2.LowPulseWidth > 52 || adc2.LowPulseWidth < 48))
		LatchErrCheck |= 4;
	if((adc2.highVoltage < 10) || (adc2.lowVoltage > 0.7) || (adc2.highVoltage == 0))
		LatchErrCheck |= 8;
		//Vin Check
	LatchErrCheck = Vin.steadyState < 11 ? LatchErrCheck |= 16 : LatchErrCheck;
		//Vfuse Check
	if(Vfuse.steadyState < 0.95*Vin.steadyState  || Vfuse.lowVoltage < 0.9*Vin.lowVoltage)
		LatchErrCheck |= 32;
		//Vmos Check
	if(Vmos1.highVoltage < 0.05 || Vmos1.highVoltage > 3 || Vmos1.lowVoltage > 0.7 || Vmos1.lowVoltage < 0.1)
		LatchErrCheck |= 64;
	if(Vmos2.highVoltage < 0.05 || Vmos2.highVoltage > 3 || Vmos2.lowVoltage > 0.7 || Vmos2.lowVoltage < 0.1)
		LatchErrCheck |= 128;

	return LatchErrCheck;
}
void printLatchError(uint8 *ErrorCode){
	if(*ErrorCode & 1)
	{
		sprintf(Buffer, "\n******** ERROR--PortA Pulse Width ********\n");
		HAL_UART_Transmit(&T_UART, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);
	}
	if(*ErrorCode & 2)
	{
		sprintf(Buffer, "\n******** ERROR--PortA Voltage ********\n");
		HAL_UART_Transmit(&T_UART, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);
	}
	if(*ErrorCode & 4)
	{
		sprintf(Buffer, "\n******** ERROR--PortB Pulse Width ********\n");
		HAL_UART_Transmit(&T_UART, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);
	}
	if(*ErrorCode & 8)
	{
		sprintf(Buffer, "\n******** ERROR--PortB Voltage ********\n");
		HAL_UART_Transmit(&T_UART, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);
	}
	if(*ErrorCode & 16)
	{
		sprintf(Buffer, "\n******** ERROR--Input Voltage ********\n");
		HAL_UART_Transmit(&T_UART, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);
	}
	if(*ErrorCode & 32){
		sprintf(Buffer, "\n******** ERROR--Fuse Voltage ********\n");
		HAL_UART_Transmit(&T_UART, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);
	}
	if(*ErrorCode & 64)
	{
		sprintf(Buffer, "\n******** ERROR--MOSFET 1 Voltage ********\n");
		HAL_UART_Transmit(&T_UART, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);
	}
	if(*ErrorCode & 128)
	{
		sprintf(Buffer, "\n******** ERROR--MOSFET 2 Voltage ********\n");
		HAL_UART_Transmit(&T_UART, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);
	}
}
