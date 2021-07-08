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
	if(READ_BIT(Board->LatchTestPort, LATCH_PORT_ONE)) {
		LatchTestPort = runLatchTest(Board, Port_1);
		return;
	}
	if(READ_BIT(Board->LatchTestPort, LATCH_PORT_TWO)) {
		LatchTestPort = runLatchTest(Board, Port_2);
		return;
	}
	if(READ_BIT(Board->LatchTestPort, LATCH_PORT_THREE)) {
		LatchTestPort = runLatchTest(Board, Port_3);
		return;
	}
	if(READ_BIT(Board->LatchTestPort, LATCH_PORT_FOUR)) {
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
	Vfuse.lowVoltage = 99.0;
	Vin.lowVoltage = 99.0;
	Vin.average = 0.0;
	Vfuse.average = 0.0;
	Vmos1.lowVoltage = 99.0;
	Vmos2.lowVoltage = 99.0;

		//ADC 1
	adc1.highVoltage = 0.0;
	adc1.HighPulseWidth = 0;
	adc1.lowVoltage = 99.0;
	adc1.LowPulseWidth = 0;

		//ADC 2
	adc2.highVoltage = 0.0;
	adc2.HighPulseWidth = 0;
	adc2.lowVoltage = 99.0;
	adc2.LowPulseWidth = 0;

}
