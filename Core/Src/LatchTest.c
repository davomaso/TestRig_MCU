#include "main.h"
#include "interogate_project.h"
#include "UART_Routine.h"

void runLatchTest(TboardConfig *Board, uint8 Test_Port){
//Latch On
	uns_ch LatchCommand = 0x26;	//Turn Digital Output on or off
	uint8 LatchState;
	ADC_MUXsel(Test_Port);
	Para[0] = 0x80 + (Test_Port * 2); // 0x80 turns digital output on
	Paralen = 1;
	latchCountTo = 2000;
	PulseCountDown = latchCountTo;

	HAL_TIM_Base_Start_IT(&htim10);
	ADC_Init();
	while(stableVoltageCount){
		if(!LatchSampling)
			break;
	}
	if (LatchSampling) {
		communication_array(LatchCommand, &Para[0], Paralen);
		while (!LatchOnComplete) { //Loop While Sampling
			if(!LatchSampling)
				break;
		}
		stableVoltageCount = 25;
		while(stableVoltageCount){
			if(!LatchSampling)
				break;
		}
		Para[0] = 0x00 + (Test_Port * 2);
		communication_array(LatchCommand, &Para[0], Paralen);
		while (!LatchOffComplete) { //Loop While Sampling
			if(!LatchSampling)
				break;
		}
		stableVoltageCount = 25;
		while(stableVoltageCount){
			if(!LatchSampling)
				break;
		}
	}
	uint8 Response;
	runLatchTimeOut();
	while (!UART2_ReceiveComplete && LatchTimeOut) { //TODO: Return to main(), requires the addition of callback to this function if 0xXX is received.

	}
	if (UART2_ReceiveComplete)
		communication_response(&Response, &UART2_RXbuffer, UART2_RecPos);

	HAL_TIM_Base_Stop(&htim10);

		//Print Results & Error Messages
	TransmitResults();
	PrintLatchResults();
	LatchState = (0xFF & LatchErrorCheck());
	if(LatchState)
		printLatchError(&LatchState);

	if(!LatchState){
		printT("\n==============   LATCH TEST PASSED  ==============\n\n\n\n");
		Board->TestResults[Board->GlobalTestNum][Test_Port] = true;
	}else{
		printT("\n==============   LATCH TEST FAILED  ==============\n\n\n\n");
		Board->TestResults[Board->GlobalTestNum][Test_Port] = false;
		}

//	TransmitResults();
}

void StoreLatchResults(TlatchResults * res, uint8 currResult) {

}
