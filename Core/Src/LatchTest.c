#include "main.h"
#include "interogate_project.h"
#include "UART_Routine.h"

uint8 runLatchTest(TboardConfig *Board, uint8 Test_Port){
	ADC_MUXsel(Test_Port);
	PulseCountDown = latchCountTo = 2000;
	SET_BIT(LatchTestStatusRegister, LATCH_SAMPLING);
	HAL_TIM_Base_Start_IT(&htim10);
	return Test_Port;
}

void LatchingSolenoidDriverTest(TboardConfig * Board) {
	if (Board->TestCode[Port_1] == 0x10) {
		LatchTestPort = runLatchTest(Board, Port_1);
		return;
	}
	if (Board->TestCode[Port_2] == 0x10) {
		LatchTestPort = runLatchTest(Board, Port_2);
		return;
	}
	if (Board->TestCode[Port_3] == 0x10) {
		LatchTestPort = runLatchTest(Board, Port_3);
		return;
	}
	if (Board->TestCode[Port_4] == 0x10) {
		LatchTestPort = runLatchTest(Board, Port_4);
		return;
	}
	LatchTestPort = 0xFF;
}

uint8 LatchTestParam(uint8 Test_Port, bool OnOff) {
	return (0x80 * OnOff) + (Test_Port * 2);
}

void LatchTestInit() {
	CLEAR_REG(LatchTestStatusRegister);
	LatchCountTimer = 0;
	LatchTestPort = 0;
	stableVoltageCount = 25;
	Vfuse.lowVoltage = 4096.0;
	Vin.lowVoltage = 4096.0;
	Vin.total = 0.0;
	Vfuse.total = 0.0;
	Vin.average = 0.0;
	Vfuse.average = 0.0;
	MOSFETvoltageA.highVoltage = 0.0;
	MOSFETvoltageB.highVoltage = 0.0;
	MOSFETvoltageA.lowVoltage = 4096.0;
	MOSFETvoltageB.lowVoltage = 4096.0;

		//ADC 1
	LatchPortA.highVoltage = 0.0;
	LatchPortA.HighPulseWidth = 0;
	LatchPortA.lowVoltage = 4096.0;
	LatchPortA.LowPulseWidth = 0;

		//ADC 2
	LatchPortB.highVoltage = 0.0;
	LatchPortB.HighPulseWidth = 0;
	LatchPortB.lowVoltage = 4096.0;
	LatchPortB.LowPulseWidth = 0;

}
