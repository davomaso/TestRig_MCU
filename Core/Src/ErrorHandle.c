#include <main.h>
#include "ADC_Variables.h"
#include "ErrorHandle.h"
#include "Global_Variables.h"
#include "UART_Routine.h"

/*
 * Latch error routine
 * This routine determines whether the latch test passes, if the values calculated by the ADC fall outside of the
 * range specified here, then the bits in the LatchErrCheck register are truggered, flagging the errors for when the
 * failures are presented.
 */
void LatchErrorCheck(TboardConfig *Board) {
	CLEAR_REG(Board->LTR);
		//PortA Pulsewidth Check
	if (((LatchPortA.HighPulseWidth > LATCH_PULSE_WIDTH_MAX) || (LatchPortA.HighPulseWidth < LATCH_PULSE_WIDTH_MIN))
			|| ((LatchPortA.LowPulseWidth > LATCH_PULSE_WIDTH_MAX) || (LatchPortA.LowPulseWidth < LATCH_PULSE_WIDTH_MIN)))
		SET_BIT(Board->LTR, PORT_A_PULSEWIDTH_ERROR);
		//PortB Pulsewidth Check
	if (((LatchPortB.HighPulseWidth > LATCH_PULSE_WIDTH_MAX) || (LatchPortB.HighPulseWidth < LATCH_PULSE_WIDTH_MIN))
			|| ((LatchPortB.LowPulseWidth > LATCH_PULSE_WIDTH_MAX) || (LatchPortB.LowPulseWidth < LATCH_PULSE_WIDTH_MIN)))
		SET_BIT(Board->LTR, PORT_B_PULSEWIDTH_ERROR);

		//PortA Voltage Check
	if ((LatchPortA.highVoltage < LATCH_PULSE_HIGH_THRESHOLD) || (LatchPortA.highVoltage > Vin.average) || (LatchPortA.lowVoltage > LATCH_PULSE_LOW_THRESHOLD))
		SET_BIT(Board->LTR, PORT_A_VOLTAGE_ERROR);
		//PortB Voltage Check
	if ((LatchPortB.highVoltage < LATCH_PULSE_HIGH_THRESHOLD) || (LatchPortB.highVoltage > Vin.average) || (LatchPortB.lowVoltage > LATCH_PULSE_LOW_THRESHOLD))
		SET_BIT(Board->LTR, PORT_B_VOLTAGE_ERROR);
//	if ((LatchPortA.highVoltage <= LATCH_PULSE_HIGH_THRESHOLD) || (LatchPortA.lowVoltage >= LATCH_PULSE_LOW_THRESHOLD))
//		SET_BIT(Board->LTR, PORT_A_VOLTAGE_ERROR);
//	if ((LatchPortB.highVoltage <= LATCH_PULSE_HIGH_THRESHOLD) || (LatchPortB.lowVoltage >= LATCH_PULSE_LOW_THRESHOLD))
//		SET_BIT(Board->LTR, PORT_B_VOLTAGE_ERROR);
		//ADC2 Check

		//Vin Check
	if (Vin.average < VIN_VOLTAGE_THRESHOLD)
		SET_BIT(Board->LTR, INPUT_VOLTAGE_ERROR);
		//Vfuse Check
	if ((Vfuse.average > Vin.average) && (Vfuse.average < 0.85 * Vin.average) && (Vfuse.lowVoltage > Vin.lowVoltage) && (Vfuse.lowVoltage < 0.85 * Vin.lowVoltage) )
		SET_BIT(Board->LTR, FUSE_VOLTAGE_ERROR);
		//Vmos Check
	if (MOSFETvoltageA.highVoltage < 0.1 || MOSFETvoltageA.highVoltage > 1.0 || MOSFETvoltageA.lowVoltage > 1.0 || MOSFETvoltageA.lowVoltage < 0.1)
		SET_BIT(Board->LTR, PORT_A_MOSFET_ERROR);
	if (MOSFETvoltageB.highVoltage < 0.1 || MOSFETvoltageB.highVoltage > 1.0 || MOSFETvoltageB.lowVoltage > 1.0 || MOSFETvoltageB.lowVoltage < 0.1)
		SET_BIT(Board->LTR, PORT_B_MOSFET_ERROR);
}

/*
 * Prints the errors that are determined above. Dependant on the code that is set by the routines above will
 * determine the errors that are printed to the terminal.
 */
void printLatchError(TboardConfig *Board) {
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
