#include "main.h"
#include "Global_Variables.h"
#include "strings.h"
#include "UART_Routine.h"
#include "LatchTest.h"

void HandleLatchSample() {
	//Vin
	Vin.avg_Buffer[LatchCountTimer] = Vin.currentValue;
	//Vfuse
	Vfuse.avg_Buffer[LatchCountTimer] = Vfuse.currentValue;
	//Latch A
	LatchPortA.avg_Buffer[LatchCountTimer] = LatchPortA.currentValue;
	//Latch B
	LatchPortB.avg_Buffer[LatchCountTimer] = LatchPortB.currentValue;

	if (LatchCountTimer > 100) {
		Vfuse.lowVoltage = Vfuse.currentValue < Vfuse.lowVoltage ? Vfuse.currentValue : Vfuse.lowVoltage;
		Vin.lowVoltage = Vin.currentValue < Vin.lowVoltage ? Vin.currentValue : Vin.lowVoltage;
		/*
		 *  Once voltage is stable find the lowest fuse and input voltage. If the calculated average voltage is less then the current minimum
		 *  set the minimum to the calculated average
		 *  */
	}

	if (stableVoltageCount) {
		/*
		 * Determine whether the input and fuse voltages are stable, if stableVoltageCount increments too high
		 * set LatchSampling to false so that the process is halted and the test fails
		 */
		if ((Vin.currentValue > 3000) && (Vfuse.currentValue > 0.75 * Vin.currentValue))
			stableVoltageCount--;
		else
			stableVoltageCount++;
		if (stableVoltageCount > 100)
			CLEAR_BIT(LatchTestStatusRegister, LATCH_SAMPLING);
		else if (stableVoltageCount == 0)
			SET_BIT(LatchTestStatusRegister, STABLE_INPUT_VOLTAGE);

	}

	if (LatchCountTimer == 100 && READ_BIT(LatchTestStatusRegister, STABLE_INPUT_VOLTAGE)) {
		/*
		 * Calculate the steady state/ average voltage of the fuse and input across the range determined
		 * if the fuse and input are determined to be stable
		 */
		Vfuse.total = Vin.total = 0;
		for (int i = 0; i <= LatchCountTimer; i++) {
			Vfuse.total += Vfuse.avg_Buffer[i];
			Vin.total += Vin.avg_Buffer[i];
		}
		Vfuse.average = Vfuse.total / LatchCountTimer;
		Vin.average = Vin.total / LatchCountTimer;
	} else if (LatchCountTimer > 100 && READ_BIT(LatchTestStatusRegister, STABLE_INPUT_VOLTAGE)) {
		/*
		 * If statement to determine the voltages of the latch and pulsewidths.
		 * If the pulse width is above 47ms the latchoff rountine can be run.
		 */
		//Latch On Test
		if (READ_BIT(LatchTestStatusRegister, LATCH_ON_SAMPLING) && !READ_BIT(LatchTestStatusRegister, LATCH_ON_COMPLETE)) {
			if (LatchPortA.currentValue > LATCH_HIGH_ADC_THRESHOLD) {
				LatchPortA.HighPulseWidth++;
				LatchPortA.highVoltage += LatchPortA.currentValue;
				if (LatchPortA.HighPulseWidth >= 20 && LatchPortA.HighPulseWidth <= 80) {
					if (Vfuse.currentValue > LatchPortA.currentValue)
						MOSFETvoltageA.total += Vfuse.currentValue - LatchPortA.currentValue;
					MOSFETvoltageA.HighPulseWidth++;
				}
				PulseCountDown =
						(LatchPortA.HighPulseWidth > 90 || LatchPortB.LowPulseWidth > 90) ? 10 : PulseCountDown;
			}
			if (LatchPortB.currentValue < LATCH_LOW_ADC_THRESHOLD) {
				LatchPortB.LowPulseWidth++;
				LatchPortB.lowVoltage += LatchPortB.currentValue;
				if (LatchPortB.LowPulseWidth > 20 && LatchPortB.LowPulseWidth < 80) {
					MOSFETvoltageB.total += LatchPortB.currentValue;
					MOSFETvoltageB.LowPulseWidth++;
				}
			}
		}
		if (READ_BIT(LatchTestStatusRegister,
				LATCH_OFF_SAMPLING) && !READ_BIT(LatchTestStatusRegister, LATCH_OFF_COMPLETE)) {
			//Latch Off Test
			if (LatchPortA.currentValue < LATCH_LOW_ADC_THRESHOLD) {
				LatchPortA.LowPulseWidth++;
				LatchPortA.lowVoltage += LatchPortA.currentValue;
				if (LatchPortA.LowPulseWidth >= 20 && LatchPortA.LowPulseWidth <= 80) {
					MOSFETvoltageA.total += LatchPortA.currentValue;
					MOSFETvoltageA.LowPulseWidth++;
				}
			}
			if (LatchPortB.currentValue > LATCH_HIGH_ADC_THRESHOLD) {
				LatchPortB.HighPulseWidth++;
				LatchPortB.highVoltage += LatchPortB.currentValue;
				if ((LatchPortB.HighPulseWidth > 20) && (LatchPortB.HighPulseWidth < 80)) {
					if (Vfuse.currentValue > LatchPortB.currentValue)
						MOSFETvoltageB.total += Vfuse.currentValue - LatchPortB.currentValue;
					MOSFETvoltageB.HighPulseWidth++;
				}
				PulseCountDown =
						(LatchPortB.HighPulseWidth > 90 || LatchPortA.LowPulseWidth > 90) ? 10 : PulseCountDown;
			}
		}
		PulseCountDown--;

		if (!PulseCountDown) {
			if (((LatchPortA.HighPulseWidth > 94) && (LatchPortB.LowPulseWidth > 94))) { //|| (PulseCountDown == 0)
				SET_BIT(LatchTestStatusRegister, LATCH_ON_COMPLETE);
				MOSFETvoltageA.highVoltage = MOSFETvoltageA.total / MOSFETvoltageA.HighPulseWidth;
				MOSFETvoltageB.lowVoltage = MOSFETvoltageB.total / MOSFETvoltageB.LowPulseWidth;
				LatchPortA.highVoltage /= 2;
				LatchPortB.lowVoltage /= 2;
				LatchPortA.HighPulseWidth /= 2;
				LatchPortB.LowPulseWidth /= 2;
				MOSFETvoltageA.total = MOSFETvoltageB.total = MOSFETvoltageA.HighPulseWidth =
						MOSFETvoltageB.LowPulseWidth = 0;
			}
			if (((LatchPortB.HighPulseWidth > 94) || (LatchPortA.LowPulseWidth > 94))) {
				SET_BIT(LatchTestStatusRegister, LATCH_OFF_COMPLETE);
				MOSFETvoltageB.highVoltage = MOSFETvoltageB.total / MOSFETvoltageB.HighPulseWidth;
				MOSFETvoltageA.lowVoltage = MOSFETvoltageA.total / MOSFETvoltageA.LowPulseWidth;
				LatchPortA.LowPulseWidth /= 2;
				LatchPortB.HighPulseWidth /= 2;
				LatchPortB.highVoltage /= 2;
				LatchPortA.lowVoltage /= 2;
				MOSFETvoltageA.total = MOSFETvoltageB.total = MOSFETvoltageA.LowPulseWidth =
						MOSFETvoltageB.HighPulseWidth = 0;
			}
		}
	}
	LatchCountTimer++;
}

uint8 runLatchTest(TboardConfig *Board, uint8 Test_Port) {
	ADC_MUXsel(Test_Port);
	PulseCountDown = LATCH_TIME_OUT;
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
	LatchCountTimer = 0;
	LatchTestPort = 0;
	stableVoltageCount = 25;
	Vfuse.lowVoltage = 4096.0;
	Vin.lowVoltage = 4096.0;
	MOSFETvoltageA.lowVoltage = 4096.0;
	MOSFETvoltageB.lowVoltage = 4096.0;
	MOSFETvoltageA.total = MOSFETvoltageB.total = 0;

//ADC 1
	LatchPortA.lowVoltage = 4096.0;

//ADC 2
	LatchPortB.lowVoltage = 4096.0;
}
