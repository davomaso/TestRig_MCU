#include "main.h"
#include "interogate_project.h"
#include "UART_Routine.h"

void runLatchTest(uint8 Test_Port){
//Latch On
	uns_ch LatchCommand = 0x26;	//Turn Digital Output on or off
	ADC_MUXsel(Test_Port);
	Para[0] = 0x80 + (Test_Port * 2); // 0x80 turns digital output on
	Paralen = 1;
	latchCountTo = 2000;
	PulseCountDown = latchCountTo;

	HAL_TIM_Base_Start(&htim10);
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
	while (!UART2_ReceiveComplete) { //Ned to include Timeout

	}
	if (UART2_ReceiveComplete)
		communication_response(&Response, &UART2_Receive, UART2_RecPos);

	HAL_TIM_Base_Stop(&htim10);
	LatchResult = true;

		//Print Results & Error Messages
//	TransmitResults();
	PrintLatchResults();
	LatchState[Test_Port] = (0xFF & LatchErrorCheck());
	if(LatchState[Test_Port])
		printLatchError(&LatchState[Test_Port]);

	if(!LatchState[Test_Port]){
		sprintf(Buffer, "\n==============   LATCH TEST PASSED  ==============\n\n\n\n");
		CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
		  HAL_UART_Transmit(&huart1, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);
	}else{
		sprintf(Buffer, "\n==============   LATCH TEST FAILED  ==============\n\n\n\n");
		CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
		  HAL_UART_Transmit(&huart1, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);
		}

//	TransmitResults();
}

void StoreLatchResults(TlatchResults * res, uint8 currResult) {

}
