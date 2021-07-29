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
		if((adc1.highVoltage < 9.2)  || (adc1.lowVoltage > 1.2) || (adc1.highVoltage == 0))
			SET_BIT(Board->LTR, PORT_A_VOLTAGE_ERROR);
		if((adc2.highVoltage < 9.2) || (adc2.lowVoltage > 1.2) || (adc2.highVoltage == 0))
			SET_BIT(Board->LTR, PORT_B_VOLTAGE_ERROR);
	} else {
		if((adc1.highVoltage <= 9.2)  || (adc1.lowVoltage >= 1.2) || (adc1.highVoltage == 0))
			SET_BIT(Board->LTR, PORT_A_VOLTAGE_ERROR);
		if((adc2.highVoltage <= 9.2 ) || (adc2.lowVoltage >= 1.2) || (adc2.highVoltage == 0))
			SET_BIT(Board->LTR, PORT_B_VOLTAGE_ERROR);
	}
		//ADC2 Check
	if( ( (adc2.HighPulseWidth > 52) || (adc2.HighPulseWidth < 48) ) || ( (adc2.LowPulseWidth > 52) || (adc2.LowPulseWidth < 48) ) )
		SET_BIT(Board->LTR, PORT_B_PULSEWIDTH_ERROR);
		//Vin Check
	if( Vin.average < 10.8 )
		SET_BIT(Board->LTR, INPUT_VOLTAGE_ERROR);
		//Vfuse Check
	if(Vfuse.average < 0.95*Vin.average  || Vfuse.lowVoltage < 0.9*Vin.lowVoltage)
		SET_BIT(Board->LTR, FUSE_VOLTAGE_ERROR);
		//Vmos Check
	if(Vmos1.highVoltage < 0.01 || Vmos1.highVoltage > 3.5 || Vmos1.lowVoltage > 0.7 || Vmos1.lowVoltage < 0.01)
		SET_BIT(Board->LTR, PORT_A_MOSFET_ERROR);
	if(Vmos2.highVoltage < 0.01 || Vmos2.highVoltage > 3.5 || Vmos2.lowVoltage > 0.7 || Vmos2.lowVoltage < 0.01)
		SET_BIT(Board->LTR, PORT_B_MOSFET_ERROR);
}
void printLatchError(TboardConfig * Board){
	/*
	 * Prints the errors that are determined above. Dependant on the code that is set by the routines above will
	 * determine the errors that are printed to the terminal.
	 */
	if(READ_BIT(Board->LTR, PORT_A_PULSEWIDTH_ERROR)) {
		printT("\n******** ERROR--PortA Pulse Width ********\n");
		}
	if(READ_BIT(Board->LTR, PORT_A_VOLTAGE_ERROR) ) {
		printT("\n******** ERROR--PortA Voltage ********\n");
		}
	if(READ_BIT(Board->LTR, PORT_B_PULSEWIDTH_ERROR) ) {
		printT("\n******** ERROR--PortB Pulse Width ********\n");
		}
	if(READ_BIT(Board->LTR, PORT_B_VOLTAGE_ERROR) ) {
		printT( "\n******** ERROR--PortB Voltage ********\n");
		}
	if(READ_BIT(Board->LTR, INPUT_VOLTAGE_ERROR) ) {
		printT( "\n******** ERROR--Input Voltage ********\n");
		}
	if(READ_BIT(Board->LTR, FUSE_VOLTAGE_ERROR) ) {
		printT( "\n******** ERROR--Fuse Voltage ********\n");
		}
	if(READ_BIT(Board->LTR, PORT_A_MOSFET_ERROR) ) {
		printT( "\n******** ERROR--MOSFET 1 Voltage ********\n");
		}
	if(READ_BIT(Board->LTR, PORT_B_MOSFET_ERROR) ) {
		printT( "\n******** ERROR--MOSFET 2 Voltage ********\n");
		}
}
