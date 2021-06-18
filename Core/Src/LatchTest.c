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
	PulseCountDown = latchCountTo = 2000;

	HAL_TIM_Base_Start_IT(&htim10);
	ADC_Init();
	while( !READ_BIT(LatchTestStatusRegister, STABLE_INPUT_VOLTAGE) ){
		if(!READ_BIT(LatchTestStatusRegister, LATCH_SAMPLING))
			break;
	}
	if ( READ_BIT(LatchTestStatusRegister, LATCH_SAMPLING) ) {
		communication_array(LatchCommand, &Para[0], Paralen);
		while ( !READ_BIT(LatchTestStatusRegister, LATCH_ON_COMPLETE) ) { //Loop While Sampling
			if(!READ_BIT(LatchTestStatusRegister, LATCH_SAMPLING))
				break;
		}
		stableVoltageCount = 25;
		while( !READ_BIT(LatchTestStatusRegister, STABLE_INPUT_VOLTAGE) ){
			if(!READ_BIT(LatchTestStatusRegister, LATCH_SAMPLING))
				break;
		}
		Para[0] = 0x00 + (Test_Port * 2);
		communication_array(LatchCommand, &Para[0], Paralen);
		while ( !READ_BIT(LatchTestStatusRegister, LATCH_OFF_COMPLETE) ) { //Loop While Sampling
			if(!READ_BIT(LatchTestStatusRegister, LATCH_SAMPLING))
				break;
		}
		stableVoltageCount = 25;
		while( !READ_BIT(LatchTestStatusRegister, STABLE_INPUT_VOLTAGE)){
			if(!READ_BIT(LatchTestStatusRegister, LATCH_SAMPLING))
				break;
		}
	}
	uint8 Response;
	runLatchTimeOut(20000);
	while (LatchTimeOut) { //TODO: Return to main(), requires the addition of callback to this function if 0xXX is received.
		if (UART2_ReceiveComplete) {
			communication_response(&Response, &UART2_RXbuffer, UART2_RecPos);
			if (Response == 0x27)
				break;
		}
	}


	HAL_TIM_Base_Stop(&htim10);

		//Print Results & Error Messages
//	TransmitResults();
	PrintLatchResults();
	LatchErrorCheck(Board);
	if(Board->LTR)
		printLatchError(&Board->LTR);

	if(!Board->LTR){
		printT("\n==============   LATCH TEST PASSED  ==============\n\n\n\n");
		Board->TestResults[Board->GlobalTestNum][Test_Port] = true;
	}else{
		printT("\n==============   LATCH TEST FAILED  ==============\n\n\n\n");
		Board->TestResults[Board->GlobalTestNum][Test_Port] = false;
		}

//	TransmitResults();
}

void LatchingSolenoidDriverTest(TboardConfig * Board, uint8 Port) {
	SetTestParam(Board, Board->GlobalTestNum, &Para, &Paralen);
	TestFunction(Board);
	switch(Port) {
		case Port_1:
				runLatchTest(Board, Port);
			break;
		case Port_2:
				runLatchTest(Board, Port);
			break;
		case Port_3:
				runLatchTest(Board, Port);
			break;
		case Port_4:
				runLatchTest(Board, Port);
			break;
	}
}
