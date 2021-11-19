#include "main.h"
#include "Global_Variables.h"
#include "strings.h"
#include "UART_Routine.h"
#include "LatchTest.h"

uint8 runLatchTest(TboardConfig *Board, uint8 Test_Port) {
	ADC_MUXsel(Test_Port);
	PulseCountDown = latchCountTo = 2000;
	SET_BIT(LatchTestStatusRegister, LATCH_SAMPLING);
	HAL_TIM_Base_Start_IT(&htim10);
	return Test_Port;
}

_Bool LatchingSolenoidDriverTest(TboardConfig * Board) {
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
