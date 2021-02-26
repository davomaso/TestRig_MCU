#include "main.h"
#include "interogate_project.h"


void runLatchTest(uint8 Test_Port){
//Latch On
	unsigned char LatchCommand = 0x26;
	ADC_MUXsel(Test_Port);
	Para[0] = 0x80 + (Test_Port * 2);
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
		communication_array(&LatchCommand, &Para[0], Paralen);
		while (!LatchOnComplete) { //Loop While Sampling
			if(!LatchSampling)
				break;
		}
		stableVoltageCount = 10;
		while(stableVoltageCount){
			if(!LatchSampling)
				break;
		}
		Para[0] = 0x00 + (Test_Port * 2);
		communication_array(&LatchCommand, &Para[0], Paralen);
		while (!LatchOffComplete) { //Loop While Sampling
			if(!LatchSampling)
				break;
		}
		stableVoltageCount = 10;
		while(stableVoltageCount){
			if(!LatchSampling)
				break;
		}
	}

	HAL_TIM_Base_Stop(&htim10);
	LatchResult = true;

		//Print Results & Error Messages
	PrintLatchResults();
	LatchState[Test_Port] = (0xFF & LatchErrorCheck());
	if(LatchState[Test_Port])
		printLatchError(&LatchState[Test_Port]);

	if(!LatchState[Test_Port]){
		sprintf(Buffer, "\n==============   LATCH TEST PASSED  ==============\n\n\n\n");
		CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
	}else{
		sprintf(Buffer, "\n==============   LATCH TEST FAILED  ==============\n\n\n\n");
		CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
		}
}

void StoreLatchResults(TlatchResults * res, uint8 currResult) {

}
