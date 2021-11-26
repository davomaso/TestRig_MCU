#include "main.h"
#include "Global_Variables.h"
#include "strings.h"
#include "UART_Routine.h"
#include "LatchTest.h"

enum LatchStates {
	EInitial_Stability_Check, ELatch_On, E_Middle_Stability_Check, ELatch_Off, EFinal_Stability_Check, ELatch_Complete
};
enum LatchStates Latch_states;

void ConvertResultsToBase_ms(TADCconfig *high_p, TADCconfig *low_p, TADCconfig *high_mos, TADCconfig *low_mos) {
	high_mos->highVoltage = high_mos->total / high_mos->HighPulseWidth;
	low_mos->lowVoltage = low_mos->total / low_mos->LowPulseWidth;
	high_p->highVoltage /= 2;
	low_p->lowVoltage /= 2;
	high_p->HighPulseWidth /= 2;
	low_p->LowPulseWidth /= 2;
	high_mos->total = low_mos->total = high_mos->HighPulseWidth = low_mos->LowPulseWidth = 0;
}

uint8 GetStableVoltageCnt(uint32 vin, uint32 vfuse, uint8 *v_count) { // , uint8 Mask
	uint8 result = 0;
	if (*v_count) {
		/*
		 * Determine whether the input and fuse voltages are stable, if stableVoltageCount increments too high
		 * set LatchSampling to false so that the process is halted and the test fails
		 */
		if ((vin > VIN_ADC_THRESHOLD) && (vfuse > (0.85 * vin) ))
			(*v_count)--;
		else
			(*v_count)++;
		if (*v_count > 200)
			CLEAR_BIT(LatchTestStatusRegister, LATCH_SAMPLING);
		else if (*v_count == 0) {
//			SET_BIT(LatchTestStatusRegister, Mask);
			result = 1;
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
 *
 *
 Initial_Stability: Check Input voltage Value, Check Fuse Voltage Value
 */
void HandleLatchSample() {
	//Vin
	Vin.avg_Buffer[LatchCountTimer] = Vin.currentValue;
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

	if (LatchCountTimer > 100) {
		Vfuse.lowVoltage = Vfuse.currentValue < Vfuse.lowVoltage ? Vfuse.currentValue : Vfuse.lowVoltage;
		Vin.lowVoltage = Vin.currentValue < Vin.lowVoltage ? Vin.currentValue : Vin.lowVoltage;

		Vin.average = Vin.average + (Vin.currentValue - Vin.average) / (LatchCountTimer-100);
		Vfuse.average = Vfuse.average + (Vfuse.currentValue - Vfuse.average) / (LatchCountTimer-100);
		/*
		 *  Once voltage is stable find the lowest fuse and input voltage. If the calculated average voltage is less then the current minimum
		 *  set the minimum to the calculated average
		 *  */
	}
	LatchCountTimer++;
}

uint8 runLatchTest(TboardConfig *Board, uint8 Test_Port) {
	ADC_MUXsel(Test_Port);
	SET_BIT(LatchTestStatusRegister, LATCH_SAMPLING);
	HAL_TIM_Base_Start_IT(&htim10);
	return Test_Port;
}

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
	SET_BIT(LatchTestStatusRegister, LATCH_SAMPLING);
}
