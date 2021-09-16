#include <main.h>
#include "ErrorHandle.h"
#include "Global_Variables.h"
#include "UART_Routine.h"

void LatchErrorCheck(TboardConfig *Board) {
	/*
	 * Latch error routine
	 * This routine determines whether the latch test passes, if the values calculated by the ADC fall outside of the
	 * range specified here, then the bits in the LatchErrCheck register are truggered, flagging the errors for when the
	 * failures are presented.
	 */
	CLEAR_REG(Board->LTR);
	//ADC1 Check
	if (((LatchPortA.HighPulseWidth > 52) || (LatchPortA.HighPulseWidth < 48))
			|| ((LatchPortA.LowPulseWidth > 52) || (LatchPortA.LowPulseWidth < 48)))
		SET_BIT(Board->LTR, PORT_A_PULSEWIDTH_ERROR);
	if (Board->BoardType == b935x || Board->BoardType == b937x) {
		if ((LatchPortA.highVoltage < 9.2) || (LatchPortA.lowVoltage > 1.2) || (LatchPortA.highVoltage == 0))
			SET_BIT(Board->LTR, PORT_A_VOLTAGE_ERROR);
		if ((LatchPortB.highVoltage < 9.2) || (LatchPortB.lowVoltage > 1.2) || (LatchPortB.highVoltage == 0))
			SET_BIT(Board->LTR, PORT_B_VOLTAGE_ERROR);
	} else {
		if ((LatchPortA.highVoltage <= 9.2) || (LatchPortA.lowVoltage >= 1.2) || (LatchPortA.highVoltage == 0))
			SET_BIT(Board->LTR, PORT_A_VOLTAGE_ERROR);
		if ((LatchPortB.highVoltage <= 9.2) || (LatchPortB.lowVoltage >= 1.2) || (LatchPortB.highVoltage == 0))
			SET_BIT(Board->LTR, PORT_B_VOLTAGE_ERROR);
	}
	//ADC2 Check
	if (((LatchPortB.HighPulseWidth > 52) || (LatchPortB.HighPulseWidth < 48))
			|| ((LatchPortB.LowPulseWidth > 52) || (LatchPortB.LowPulseWidth < 48)))
		SET_BIT(Board->LTR, PORT_B_PULSEWIDTH_ERROR);
	//Vin Check
	if (Vin.average < 10.8)
		SET_BIT(Board->LTR, INPUT_VOLTAGE_ERROR);
	//Vfuse Check
	if (Vfuse.average < 0.95 * Vin.average || Vfuse.lowVoltage < 0.9 * Vin.lowVoltage)
		SET_BIT(Board->LTR, FUSE_VOLTAGE_ERROR);
	//Vmos Check
	if (MOSFETvoltageA.highVoltage < 0.001 || MOSFETvoltageA.highVoltage > 1.8 || MOSFETvoltageA.lowVoltage > 1.8
			|| MOSFETvoltageA.lowVoltage < 0.001)
		SET_BIT(Board->LTR, PORT_A_MOSFET_ERROR);
	if (MOSFETvoltageB.highVoltage < 0.001 || MOSFETvoltageB.highVoltage > 1.8 || MOSFETvoltageB.lowVoltage > 1.8
			|| MOSFETvoltageB.lowVoltage < 0.001)
		SET_BIT(Board->LTR, PORT_B_MOSFET_ERROR);
}
void printLatchError(TboardConfig *Board) {
	/*
	 * Prints the errors that are determined above. Dependant on the code that is set by the routines above will
	 * determine the errors that are printed to the terminal.
	 */
	if (READ_BIT(Board->LTR, PORT_A_PULSEWIDTH_ERROR)) {
		printT((uns_ch*) "\n******** ERROR--PortA Pulse Width ********\n");
	}
	if (READ_BIT(Board->LTR, PORT_A_VOLTAGE_ERROR)) {
		printT((uns_ch*) "\n******** ERROR--PortA Voltage ********\n");
	}
	if (READ_BIT(Board->LTR, PORT_B_PULSEWIDTH_ERROR)) {
		printT((uns_ch*) "\n******** ERROR--PortB Pulse Width ********\n");
	}
	if (READ_BIT(Board->LTR, PORT_B_VOLTAGE_ERROR)) {
		printT((uns_ch*) "\n******** ERROR--PortB Voltage ********\n");
	}
	if (READ_BIT(Board->LTR, INPUT_VOLTAGE_ERROR)) {
		printT((uns_ch*) "\n******** ERROR--Input Voltage ********\n");
	}
	if (READ_BIT(Board->LTR, FUSE_VOLTAGE_ERROR)) {
		printT((uns_ch*) "\n******** ERROR--Fuse Voltage ********\n");
	}
	if (READ_BIT(Board->LTR, PORT_A_MOSFET_ERROR)) {
		printT((uns_ch*) "\n******** ERROR--MOSFET 1 Voltage ********\n");
	}
	if (READ_BIT(Board->LTR, PORT_B_MOSFET_ERROR)) {
		printT((uns_ch*) "\n******** ERROR--MOSFET 2 Voltage ********\n");
	}
}
