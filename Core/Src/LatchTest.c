#include "main.h"
#include "Global_Variables.h"
#include "strings.h"
#include "UART_Routine.h"
#include "File_Handling.h"
#include "LatchTest.h"

enum LatchStates {
	EInitial_Stability_Check, ELatch_On, E_Middle_Stability_Check, ELatch_Off, EFinal_Stability_Check, ELatch_Complete
};
enum LatchStates Latch_states;

// Converts the latch sample results from 500us samples to 1ms
void ConvertResultsToBase_ms(TADCconfig *high_p, TADCconfig *low_p, TADCconfig *high_mos, TADCconfig *low_mos) {	// Pass the high/low latch/MOS values to convert to ms, as sampling occurs at 500us
	high_mos->highVoltage = high_mos->total / high_mos->HighPulseWidth;
	low_mos->lowVoltage = low_mos->total / low_mos->LowPulseWidth;
	high_p->highVoltage /= 2;
	low_p->lowVoltage /= 2;
	high_p->HighPulseWidth /= 2;
	low_p->LowPulseWidth /= 2;
	high_mos->total = low_mos->total = high_mos->HighPulseWidth = low_mos->LowPulseWidth = 0;
}

// routine to establish weather sample voltage has returned to a stable state
uint8 GetStableVoltageCnt(uint32 vin, uint32 vfuse, uint8 *v_count) { // initialise v_count (stableVoltageCount) to 75-100 to test voltage stability, then pass to this function repetively to establish stability
	uint8 result = 0;
	if (*v_count) {		// v_count > 0
		if ((vin > VIN_ADC_THRESHOLD) && (vfuse > (0.85 * vin) ))	// When vin, surpass the threshold & vfuse, surpass 85% of vin decrement count, else increase count
			(*v_count)--;
		else
			(*v_count)++;
		if (*v_count > 200)
			CLEAR_BIT(LatchTestStatusRegister, LATCH_SAMPLING);	// DISABLE latch if the count surpasses 200
		else if (*v_count == 0) {
			result = 1;	// return 1 if successful
		}
	}
	return result;
}


int8 SetLatchValue(TADCconfig *high_p, TADCconfig *low_p, TADCconfig *high_mos, TADCconfig *low_mos) {
	int8 result = 0;
	if (high_p->currentValue > LATCH_HIGH_ADC_THRESHOLD) {
		high_p->HighPulseWidth++;
		high_p->highVoltage += high_p->currentValue;
		if (high_p->HighPulseWidth >= 20 && high_p->HighPulseWidth <= 80) {
			if (Vfuse.currentValue > high_p->currentValue)
				high_mos->total += Vfuse.currentValue - high_p->currentValue;
			high_mos->HighPulseWidth++;
		}
	}
	if (low_p->currentValue < LATCH_LOW_ADC_THRESHOLD) {
		low_p->LowPulseWidth++;
		low_p->lowVoltage += low_p->currentValue;
		if (low_p->LowPulseWidth > 20 && low_p->LowPulseWidth < 80) {
			low_mos->total += low_p->currentValue;
			low_mos->LowPulseWidth++;
		}
	}

	if ((high_p->HighPulseWidth > 60) && (low_p->LowPulseWidth > 60)) {
		if ((high_p->currentValue >= 1450) && (high_p->currentValue < 1770) && (low_p->currentValue >= 1450)
				&& (low_p->currentValue < 1770)) { //between 5.4 and 6.6 volts.
			result = 1;
		}
	}
	return result;
}

/*
 * Latch testing state machine to handle latch testing, performs stability check before and after each latch to ensure a perfect latch operation
 * stores values into avg_buffer to be written to file.
 */
void HandleLatchSample() {
		//Vin
	Vin.avg_Buffer[LatchCountTimer] = Vin.currentValue; //TODO: Find a way to write this to a file as the results are read to reduce memory usage
		//Vfuse
	Vfuse.avg_Buffer[LatchCountTimer] = Vfuse.currentValue;
		//Latch A
	LatchPortA.avg_Buffer[LatchCountTimer] = LatchPortA.currentValue;
		//Latch B
	LatchPortB.avg_Buffer[LatchCountTimer] = LatchPortB.currentValue;

	switch (Latch_states) {
	case EInitial_Stability_Check:
		if (GetStableVoltageCnt(Vin.currentValue, Vfuse.currentValue, &stableVoltageCount)) {		// , STABLE_INPUT_VOLTAGE
			BoardCommsParameters[0] = LatchTestParam(LatchTestPort, 1);
			communication_array(0x26, &BoardCommsParameters[0], 1);
			Latch_states = ELatch_On;
		}
		break;
	case ELatch_On:
		if (SetLatchValue(&LatchPortA, &LatchPortB, &MOSFETvoltageA, &MOSFETvoltageB)) {
			if (BoardCommsReceiveState == RxGOOD) {
				if (Data_Buffer[0] == 0x27) {
					Latch_states = E_Middle_Stability_Check;
					stableVoltageCount = 50;
					ConvertResultsToBase_ms(&LatchPortA, &LatchPortB, &MOSFETvoltageA, &MOSFETvoltageB);
				}
			}
		}
		break;
	case E_Middle_Stability_Check:
		if (GetStableVoltageCnt(Vin.currentValue, Vfuse.currentValue, &stableVoltageCount)) {		// , STABLE_INPUT_VOLTAGE
			BoardCommsParameters[0] = LatchTestParam(LatchTestPort, 0);
			communication_array(0x26, &BoardCommsParameters[0], 1);
			Latch_states = ELatch_Off;
		}
		break;
	case ELatch_Off:
		if (SetLatchValue(&LatchPortB, &LatchPortA, &MOSFETvoltageB, &MOSFETvoltageA)) {
			if (BoardCommsReceiveState == RxGOOD) {
				if (Data_Buffer[0] == 0x27) {
					Latch_states = EFinal_Stability_Check;
					stableVoltageCount = 50;
					ConvertResultsToBase_ms(&LatchPortB, &LatchPortA, &MOSFETvoltageB, &MOSFETvoltageA);
				}
			}
		}
		break;
	case EFinal_Stability_Check:
		if (GetStableVoltageCnt(Vin.currentValue, Vfuse.currentValue, &stableVoltageCount)) {	//  , STABLE_INPUT_VOLTAGE
			Latch_states = ELatch_Complete;
		}
		break;
	case ELatch_Complete:
		SET_BIT(LatchTestStatusRegister, LATCH_TEST_COMPLETE);
		break;
	default:
		break;
	}

	if (LatchCountTimer > 100) {	// After first 50ms begin calculating/comparing average and lowest value
		Vfuse.lowVoltage = Vfuse.currentValue < Vfuse.lowVoltage ? Vfuse.currentValue : Vfuse.lowVoltage;
		Vin.lowVoltage = Vin.currentValue < Vin.lowVoltage ? Vin.currentValue : Vin.lowVoltage;

		Vin.average = Vin.average + (Vin.currentValue - Vin.average) / (LatchCountTimer-100);
		Vfuse.average = Vfuse.average + (Vfuse.currentValue - Vfuse.average) / (LatchCountTimer-100);
	}
	LatchCountTimer++;
}

uint8 runLatchTest(TboardConfig *Board, uint8 Test_Port) {
	ADC_MUXsel(Test_Port);
	SET_BIT(LatchTestStatusRegister, LATCH_SAMPLING);
	HAL_TIM_Base_Start_IT(&htim10);
	return Test_Port;
}

	// Check which port the latch test is to be run on then return true if the port is found, setting the latchtestport in the process.
_Bool LatchingSolenoidDriverTest(TboardConfig *Board) {
	if (Board->TestCode[Port_1] == 0x10) {
		LatchTestPort = runLatchTest(Board, Port_1);
		return true;
	}
	if (Board->TestCode[Port_2] == 0x10) {
		LatchTestPort = runLatchTest(Board, Port_2);
		return true;
	}
	if (Board->TestCode[Port_3] == 0x10) {
		LatchTestPort = runLatchTest(Board, Port_3);
		return true;
	}
	if (Board->TestCode[Port_4] == 0x10) {
		LatchTestPort = runLatchTest(Board, Port_4);
		return true;
	}
	return false;
}

uint8 LatchTestParam(uint8 Test_Port, bool OnOff) {
	return (0x80 * OnOff) + (Test_Port * 2);
}

	// Initialisation for the latch testing, setting all the various struct to zero, and returning all variables to initial states
void LatchTestInit() {
	bzero(&LatchPortA, sizeof(TADCconfig));
	bzero(&LatchPortB, sizeof(TADCconfig));
	bzero(&Vin, sizeof(TADCconfig));
	bzero(&Vfuse, sizeof(TADCconfig));
	bzero(&MOSFETvoltageA, sizeof(TADCconfig));
	bzero(&MOSFETvoltageB, sizeof(TADCconfig));
	CLEAR_REG(LatchTestStatusRegister);
	stableVoltageCount = 50; //Must stay stable for 25 cycles
	Vfuse.lowVoltage = 4096.0;
	Vin.lowVoltage = 4096.0;
	MOSFETvoltageA.lowVoltage = 4096.0;
	MOSFETvoltageB.lowVoltage = 4096.0;
	MOSFETvoltageA.total = MOSFETvoltageB.total = 0;

//ADC 1
	LatchPortA.lowVoltage = 4096.0;

//ADC 2
	LatchPortB.lowVoltage = 4096.0;
	LatchCountTimer = 0;
	Latch_states = EInitial_Stability_Check;
//	SET_BIT(LatchTestStatusRegister, LATCH_SAMPLING);
}


//	============================    Print Results    ================================//
void PrintLatchResults() {		// Print the results of the latch test to the terminal
	float LatchCurrent1;		// local variables to calculate the current of each pulse
	float LatchCurrent2;
	printT((uns_ch*) "\n\n=======================             Latch Test             =======================\n\n");
		// Port A pulse voltages
	sprintf((char*) &debugTransmitBuffer,
			"\n==============   Port A Latch time:   High: %d        Low: %d       ==============\n",
			LatchPortA.HighPulseWidth, LatchPortA.LowPulseWidth);
	printT((uns_ch*) &debugTransmitBuffer);
		// Port A Pulse width
	sprintf((char*) &debugTransmitBuffer,
			"\n==============   Port A Voltage:      High: %.3f    Low: %.3f    ==============\n",
			LatchPortA.highVoltage, LatchPortA.lowVoltage);
	printT((uns_ch*) &debugTransmitBuffer);
		// Port B Pulse width
	sprintf((char*) &debugTransmitBuffer,
			"\n==============   Port B Latch time:   High: %d        Low: %d       ==============\n",
			LatchPortB.HighPulseWidth, LatchPortB.LowPulseWidth);
	printT((uns_ch*) &debugTransmitBuffer);
		// Port B Pulse voltages
	sprintf((char*) &debugTransmitBuffer,
			"\n==============   Port B Voltage:      High: %.3f    Low: %.3f    ==============\n\n",
			LatchPortB.highVoltage, LatchPortB.lowVoltage);
	printT((uns_ch*) &debugTransmitBuffer);
		// Vin Voltages
	sprintf((char*) &debugTransmitBuffer,
			"\n==============   Vin Voltage:         AVG: %.3f     Min: %.3f   ==============\n", Vin.average,
			Vin.lowVoltage);
	printT((uns_ch*) &debugTransmitBuffer);
		// Vfuse Voltages
	sprintf((char*) &debugTransmitBuffer,
			"\n==============   Fuse Voltage:        AVG: %.3f     Min: %.3f   ==============\n", Vfuse.average,
			Vfuse.lowVoltage);
	printT((uns_ch*) &debugTransmitBuffer);
		// MOSFET voltage A
	sprintf((char*) &debugTransmitBuffer,
			"\n==============   MOSFET 1 Voltage:    High: %.3f     Low: %.3f    ==============\n",
			MOSFETvoltageA.highVoltage, MOSFETvoltageA.lowVoltage);
	printT((uns_ch*) &debugTransmitBuffer);
		// MOSFET voltage B
	sprintf((char*) &debugTransmitBuffer,
			"\n==============   MOSFET 2 Voltage:    High: %.3f     Low: %.3f    ==============\n",
			MOSFETvoltageB.highVoltage, MOSFETvoltageB.lowVoltage);
	printT((uns_ch*) &debugTransmitBuffer);

		// Calculate the current through the latches
	LatchCurrent2 = LatchPortB.highVoltage - LatchPortA.lowVoltage;
	LatchCurrent1 = LatchPortA.highVoltage - LatchPortB.lowVoltage;
	LatchCurrent1 /= ADC_Rcurrent;
	LatchCurrent2 /= ADC_Rcurrent;
		// Latch Current for both pulses
	sprintf((char*) &debugTransmitBuffer[0],
			"\n==============   Lactch Current:      P1: %.3f       P2: %.3f     ==============\n", LatchCurrent1,
			LatchCurrent2);
	printT((uns_ch*) &debugTransmitBuffer);
}

void normaliseLatchResults() {
		// Port A Voltage
	LatchPortA.highVoltage = LatchPortA.HighPulseWidth > 0 ? (LatchPortA.highVoltage / LatchPortA.HighPulseWidth) : 0;
	LatchPortA.highVoltage *= (ADC_MAX_INPUT_VOLTAGE / ADC_RESOLUTION);
	LatchPortA.lowVoltage /= LatchPortA.LowPulseWidth;
	LatchPortA.lowVoltage *= (ADC_MAX_INPUT_VOLTAGE / ADC_RESOLUTION);

		// Port B Voltage
	LatchPortB.highVoltage = LatchPortB.HighPulseWidth > 0 ? LatchPortB.highVoltage / LatchPortB.HighPulseWidth : 0;
	LatchPortB.highVoltage *= (ADC_MAX_INPUT_VOLTAGE / ADC_RESOLUTION);
	LatchPortB.lowVoltage /= LatchPortB.LowPulseWidth;
	LatchPortB.lowVoltage *= (ADC_MAX_INPUT_VOLTAGE / ADC_RESOLUTION);

		// Vin Voltage
	Vin.lowVoltage *= (MAX_SOURCE_VALUE / ADC_RESOLUTION);
	Vin.average *= (MAX_SOURCE_VALUE / ADC_RESOLUTION);
		// Fuse Voltage
	Vfuse.lowVoltage *= (MAX_SOURCE_VALUE / ADC_RESOLUTION);
	Vfuse.average *= (MAX_SOURCE_VALUE / ADC_RESOLUTION);
		// MOSFET Voltage
	MOSFETvoltageA.lowVoltage *= (ADC_MAX_INPUT_VOLTAGE / ADC_RESOLUTION);
	MOSFETvoltageA.highVoltage *= (ADC_MAX_INPUT_VOLTAGE / ADC_RESOLUTION);
	MOSFETvoltageB.lowVoltage *= (ADC_MAX_INPUT_VOLTAGE / ADC_RESOLUTION);
	MOSFETvoltageB.highVoltage *= (ADC_MAX_INPUT_VOLTAGE / ADC_RESOLUTION);

		// Store Values
	LatchRes.tOn = (LatchPortA.HighPulseWidth >= LatchPortB.LowPulseWidth) ?
					LatchPortA.HighPulseWidth : LatchPortB.LowPulseWidth;
	LatchRes.tOff =
			(LatchPortB.HighPulseWidth >= LatchPortA.LowPulseWidth) ?
					LatchPortB.HighPulseWidth : LatchPortA.LowPulseWidth;

}
	// Routine to transmit the latch results to the SD card storing the results as "serialNumber_LATCH_testNumber.csv"
void TransmitResults(TboardConfig *Board) {
	sprintf(SDcard.FILEname, "TEST_RESULTS/%lu_%x/%lu_LATCH_%d.CSV", Board->SerialNumber, Board->BoardType,
			Board->SerialNumber, Board->GlobalTestNum + 1);	// Create the latch results file string
	Create_File(&SDcard);	// Create the file on the SDcard
	if (TestRigMode == VerboseMode)
		printT((uns_ch*) "==============   ADC Average Results   ==============");
	sprintf((char*) &debugTransmitBuffer, "t(ms),PortA,PortB,Vin\n");
	Write_File(&SDcard, (TCHAR*) &SDcard.FILEname, (char*) &debugTransmitBuffer[0]);	// Write the header to the file
	for (int i = 0; i < LatchCountTimer; i++) {	//TODO: remove this to occur during the polling of the adc to remove the large overhead here, check how long writing to the SDcard takes then move where it occurs
		sprintf((char*) &debugTransmitBuffer[0], "%.1f,%.3f,%.3f,%.3f\n", i * 0.5,
				(LatchPortA.avg_Buffer[i] * 15.25 / 4096), (LatchPortB.avg_Buffer[i] * 15.25 / 4096),
				(Vfuse.avg_Buffer[i] * 15.25 / 4096));	// covert the units to voltage and store in string
//		printT((uns_ch*) &debugTransmitBuffer);
		Write_File(&SDcard, (TCHAR*) &SDcard.FILEname, (char*) &debugTransmitBuffer[0]);	// append the results to the file
	}
	Close_File(&SDcard);	// close file after the results have been written
}
