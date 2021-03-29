#include "Test.h"
#include "stdio.h"
#include "Board_Config.h"
#include "main.h"
#include "interogate_project.h"
#include "SetVsMeasured.h"

void SetTestParam(TboardConfig*, uint8);
extern TestFunction(unsigned char*, uint8, uint8, uint8);
extern set_ALL_DAC(int);

// ==================	Global Assignment of Port Configs	================== //
void ConfigInit() {
	//Sample Time Adjustment
	SampleTime.Code = 0x00;	// 12V or 2V no sample voltage required
	SampleTime.Channels = 0x00;	//Set voltage to always off
	SampleTime.GateTime = 0x02; //0.2s Sample Time

	//Output Test Assignment
	outputTest.Code = 0x01;
	outputTest.Channels = 0x00;
	outputTest.GateTime = 0x00;

	//Latch Test Assignment
	latchTest.Code = TWO_WIRE_LATCHING;
	latchTest.Channels = 0x32;	//Pulse Time
	latchTest.Options = 0xFA;	//Delay Between Pulses
	latchTest.GateTime = 0x00;	//Options

	//Current Test Assignment
	currentTest.Code = TWENTY_AMP;
	currentTest.Channels = 1;
	currentTest.Options = 0;
	currentTest.GateTime = 0;

	//Voltage Test Assignment
	OnevoltTest.Code = ONE_VOLT;
	OnevoltTest.Channels = 1;
	OnevoltTest.Options = 0;
	OnevoltTest.GateTime = 0;

	TwovoltTest.Code = TWOFIVE_VOLT;
	TwovoltTest.Channels = 1;
	TwovoltTest.Options = 0;
	TwovoltTest.GateTime = 0;

	//Async Test Assignment
	asyncTest.Code = ASYNC_PULSE;
	asyncTest.Channels = 2;
	asyncTest.Options = 0xA0;
	asyncTest.GateTime = 0;

	//Async Test for Digital Only Ports
	asyncDigitalTest.Code = ASYNC_PULSE;
	asyncDigitalTest.Channels = 1;
	asyncDigitalTest.Options = 0x80;
	asyncDigitalTest.GateTime = 0;

	//SDI12 Test
	sdi12Test.Code = SDI_TWELVE;
	sdi12Test.Channels = 1;
	sdi12Test.Options = 0;
	sdi12Test.GateTime = 0;
	//No Test
	noTest.Code = NOTEST;
	noTest.Channels = NOTEST;
	noTest.Options = NOTEST;
	noTest.GateTime = NOTEST;
}
//==================================================================================================================================//


//=====================================================  SINGLE & DUAL BOARDS  =====================================================//
TboardConfig TestConfig935x() {
	BoardType = b935x;
	TestNum = 6;
	PortNum = 5;
	ParamNum = 4;

	//Set Number of Ports by Type:
	LatchNum = 1;
	AnalogNum = 2;
	DigitalNum = 2;

	TportConfig *TestArray935x[6][5] = {
			{ &latchTest, &currentTest,&OnevoltTest, &asyncDigitalTest, &asyncDigitalTest },
			{ &latchTest, &sdi12Test, &TwovoltTest, &asyncDigitalTest, &asyncDigitalTest },
			{ &latchTest, &asyncDigitalTest, &sdi12Test, &asyncDigitalTest,&asyncDigitalTest },
			{ &latchTest, &OnevoltTest, &currentTest, &asyncDigitalTest, &asyncDigitalTest },
			{ &latchTest, &TwovoltTest, &currentTest, &asyncDigitalTest, &asyncDigitalTest },
			{ &latchTest, &currentTest, &asyncTest, &asyncDigitalTest, &asyncDigitalTest }
	};
	unsigned char portCodeArray935x[5][4] = {
			{ 0x80, 0x81, 0x82, 0x83 },
			{ 0xC0, 0xC1, 0xC2, 0xC3 },
			{ 0xD0, 0xD1, 0xD2, 0xD3 },
			{ 0xE0, 0xE1, 0xE2, 0x00 },
			{ 0xF0, 0xF1, 0xF2, 0x00 },
	};

	TboardConfig single935x_Test = { BoardType, LatchNum, AnalogNum, DigitalNum, TestNum, ParamNum, &TestArray935x, &portCodeArray935x };
	SetTestParam(&single935x_Test, GlobalTestNum);
	return single935x_Test;
}

TboardConfig TestConfig937x() {
	BoardType = b937x;
	TestNum = 6;
	PortNum = 4;
	ParamNum = 4;

	//Set Number of Ports by Type:
	LatchNum = 2;
	AnalogNum = 2;
	DigitalNum = 0;

	TportConfig *TestArray937x[6][4] = {
			{ &latchTest, &noTest, &sdi12Test, &asyncTest },
			{ &noTest, &latchTest, &asyncTest, &sdi12Test },
			{ &noTest, &noTest, &asyncTest, &OnevoltTest },
			{ &noTest, &noTest, &OnevoltTest, &asyncTest },
			{ &noTest, &noTest, &asyncTest, &currentTest },
			{ &noTest, &noTest, &TwovoltTest, &asyncTest },
	};

	unsigned char portCodeArray937x[4][4] = {
			{ 0x80, 0x81, 0x00, 0x00 },
			{ 0x84, 0x85, 0x88, 0x89 },
			{ 0xC0, 0xC1, 0xC2, 0x00 },
			{ 0xD0, 0xD1, 0xD2, 0x00 },
	};

	TboardConfig dual937x_Test = { BoardType, LatchNum, AnalogNum, DigitalNum, TestNum, ParamNum, &TestArray937x, &portCodeArray937x };
	SetTestParam(&dual937x_Test, GlobalTestNum);
	return dual937x_Test;
}
//==================================================================================================================================//


//====================================================  INPUT EXPANSION BOARDS  ====================================================//
TboardConfig TestConfig401x(){
	BoardType = b401x;
	TestNum = 6;
	PortNum = 4;
	ParamNum = 4;

	//Set Number of Ports by Type:
	LatchNum = 0;
	AnalogNum = 4;
	DigitalNum = 0;

	TportConfig *TestArray401x[6][4] = {
			{ &sdi12Test, &OnevoltTest, &currentTest, &asyncTest },
			{ &asyncTest, &sdi12Test, &OnevoltTest, &currentTest },
			{ &currentTest, &asyncTest, &sdi12Test, &OnevoltTest },
			{ &OnevoltTest, &currentTest, &asyncTest, &sdi12Test },
			{ &TwovoltTest, &currentTest, &TwovoltTest, &currentTest },
			{ &currentTest, &TwovoltTest, &currentTest, &TwovoltTest },
	};

	unsigned char portCodeArray401x[4][4] = {
			{ 0xC0, 0xC1, 0xC2, 0xC3 },
			{ 0xD0, 0xD1, 0xD2, 0xD3 },
			{ 0xE0, 0xE1, 0xE2, 0xE3 },
			{ 0xF0, 0xF1, 0xF2, 0xF3 },
	};

	TboardConfig input401x_Test = { BoardType, LatchNum, AnalogNum, DigitalNum, TestNum, ParamNum, &TestArray401x, &portCodeArray401x };
	SetTestParam(&input401x_Test, GlobalTestNum);
	return input401x_Test;
}

TboardConfig TestConfig402x(){
	BoardType = b402x;
	TestNum = 7;
	PortNum = 10;
	ParamNum = 4;

	//Set Number of Ports by Type:
	LatchNum = 1;
	AnalogNum = 6;
	DigitalNum = 3;

	TportConfig *TestArray402x[7][10] = {
			{ &sdi12Test, &OnevoltTest, &OnevoltTest, &OnevoltTest, &currentTest, &currentTest, &asyncTest, &asyncTest, &asyncDigitalTest, &outputTest },
			{ &currentTest, &sdi12Test, &TwovoltTest, &TwovoltTest, &currentTest, &OnevoltTest, &asyncDigitalTest, &asyncTest, &asyncTest, &outputTest },
			{ &currentTest, &TwovoltTest, &sdi12Test, &currentTest, &OnevoltTest, &currentTest, &asyncTest, &asyncDigitalTest, &asyncTest, &noTest },
			{ &OnevoltTest, &currentTest, &currentTest, &sdi12Test, &TwovoltTest, &TwovoltTest, &asyncTest, &asyncTest, &asyncDigitalTest, &noTest },
			{ &TwovoltTest, &currentTest, &currentTest, &currentTest, &sdi12Test, &asyncDigitalTest, &asyncDigitalTest, &asyncTest, &asyncTest, &noTest },
			{ &asyncTest, &asyncDigitalTest, &asyncTest, &asyncDigitalTest, &asyncTest, &sdi12Test, &asyncTest, &asyncDigitalTest, &asyncTest, &noTest },
			{ &asyncDigitalTest, &asyncTest, &asyncDigitalTest, &asyncTest, &asyncDigitalTest, &asyncTest, &asyncDigitalTest, &asyncTest, &asyncDigitalTest, &noTest },
	};
						//Watch PARAMcount on 4021 boards as config on iConfigure had 5 parameters but 0x00 in the 5th config param, may not be required but check results
	unsigned char portCodeArray402x[10][4] = {
			{ 0xA0, 0xA1, 0xA2, 0xA3},
			{ 0xA8, 0xA9, 0xAA, 0xAB},
			{ 0xB0, 0xB1, 0xB2, 0xB3},
			{ 0xB8, 0xB9, 0xBA, 0xBB},
			{ 0xC0, 0xC1, 0xC2, 0xC3},
			{ 0xC8, 0xC9, 0xCA, 0xCB},
			{ 0xD0, 0xD1, 0xD2, 0xD3},
			{ 0xD8, 0xD9, 0xDA, 0xDB},
			{ 0xE0, 0xE1, 0xE2, 0xE3},
			{ 0xE8, 0x00, 0x00, 0x00},
	};

	TboardConfig input402x_Test = { BoardType, LatchNum, AnalogNum, DigitalNum, TestNum, ParamNum, &TestArray402x, &portCodeArray402x };
	SetTestParam(&input402x_Test, GlobalTestNum);
	return input402x_Test;
}
//==================================================================================================================================//


//====================================================  OUTPUT EXPANSION BOARDS  ====================================================//
TboardConfig TestConfig422x(){
	BoardType = b422x;
	TestNum = 4;
	PortNum = 4;
	ParamNum = 2;

	//Set Number of Ports by Type:
	LatchNum = 4;
	AnalogNum = 0;
	DigitalNum = 0;

	TportConfig *TestArray422x[4][4] = {
			{ &latchTest, &noTest, &noTest, &noTest },
			{ &noTest, &latchTest, &noTest, &noTest },
			{ &noTest, &noTest, &latchTest, &noTest },
			{ &noTest, &noTest, &noTest, &latchTest },
	};

	unsigned char portCodeArray422x[4][2] = {
			{ 0xC0, 0xC1 },
			{ 0xC4, 0xC5 },
			{ 0xC8, 0xC9 },
			{ 0xCC, 0xCD },
	};

	TboardConfig input422x_Test = { BoardType, LatchNum, AnalogNum, DigitalNum, TestNum, ParamNum, &TestArray422x, &portCodeArray422x,  };
	SetTestParam(&input422x_Test, GlobalTestNum);
	return input422x_Test;
}

TboardConfig TestConfig427x(){
	BoardType = b427x;
	TestNum = 4;
	PortNum = 5;
	ParamNum = 4;

	//Set Number of Ports by Type:
	LatchNum = 4;
	AnalogNum = 1;
	DigitalNum = 0;

	TportConfig *TestArray427x[4][5] = {
			{ &latchTest, &noTest, &noTest, &noTest, &OnevoltTest },
			{ &noTest, &latchTest, &noTest, &noTest, &currentTest },
			{ &noTest, &noTest, &latchTest, &noTest, &asyncTest },
			{ &noTest, &noTest, &noTest, &latchTest, &sdi12Test },
	};

	unsigned char portCodeArray427x[5][4] = {
			{ 0xC0, 0xC1, 0x00, 0x00 },
			{ 0xC4, 0xC5, 0x00, 0x00 },
			{ 0xC8, 0xC9, 0x00, 0x00 },
			{ 0xCC, 0xCD, 0x00, 0x00 },
			{ 0x90, 0x91, 0x92, 0x93 },
	};

	TboardConfig input427x_Test = { BoardType, LatchNum, AnalogNum, DigitalNum, TestNum, ParamNum, &TestArray427x, &portCodeArray427x };
	SetTestParam(&input427x_Test, GlobalTestNum);
	return input427x_Test;
}
//==================================================================================================================================//


 //=====================================================  SET TEST PARAMETERS  =====================================================//
void SetTestParam(TboardConfig *testParam, uint8 TestCount) {
		//Pointers to the two arrays in the board struct
		TportConfig **testPtr;
		unsigned char *portCodesPtr;
		TportConfig *thisTest;
		// ====================  Set Pointers to TestArray and PortCodes  ==================== //
		testPtr = testParam->TestArray;
		portCodesPtr = testParam->PortCodes;

		uint8 TotalPort = testParam->outputPortCount + testParam->analogInputCount + testParam->digitalInputCout;
		if ((TestCount) < testParam->testNum) {	//Error check to ensure no overflow of test count
			testPtr += TestCount * TotalPort;//Increment to the test that the system is up to

			// ======= Variables for the SetTestParam Function  ======= //
			unsigned char para[256];

			uint8 PortCount;
			//Sum of Input and Output Ports
			uint8 Count = 0;
			//Test_Port represents the counter for the first element of each port
			Test_Port = 0;
			// ======= Load Both Arrays into Para to be sent to Target Board  ======= //

			for (PortCount = 0; PortCount < TotalPort; PortCount++) {
				thisTest = *testPtr++;

				if(thisTest->Code){
					para[Count++] = *portCodesPtr++;
					para[Count++] = thisTest->Code;

					if(thisTest->Channels){
					para[Count++] = *portCodesPtr++;
					para[Count++] = thisTest->Channels;}
					else portCodesPtr++;

					if(thisTest->Options && *portCodesPtr){
					para[Count++] = *portCodesPtr++;
					para[Count++] = thisTest->Options;}
					else portCodesPtr++;

					if (thisTest->GateTime && *portCodesPtr) {
						para[Count++] = *portCodesPtr++;
						para[Count++] = thisTest->GateTime;
					} else portCodesPtr++;
					TestCode[Test_Port++] = thisTest->Code;
				} else{
					TestCode[Test_Port++] = thisTest->Code;
					portCodesPtr += 4; }

				if(thisTest->Code == SDI_TWELVE)
					SDIenabled = true;
				if(thisTest->Code == 0x09){
					switch((Test_Port-testParam->outputPortCount)){
					case 1:
						Async_Port1.FilterEnabled = (thisTest->Options & 0x20);
						break;
					case 2:
						Async_Port2.FilterEnabled = (thisTest->Options & 0x20);
 						break;
					case 3:
						Async_Port3.FilterEnabled = thisTest->Options & 0x20;
						break;
					case 4:
						Async_Port4.FilterEnabled = thisTest->Options & 0x20;
						break;
					case 5:
						Async_Port5.FilterEnabled = thisTest->Options & 0x20;
						break;
					case 6:
						Async_Port6.FilterEnabled = thisTest->Options & 0x20;
						break;
					case 7:
						Async_Port7.FilterEnabled = thisTest->Options & 0x20;
						break;
					case 8:
						Async_Port8.FilterEnabled = thisTest->Options & 0x20;
						break;
					case 9:
						Async_Port9.FilterEnabled = thisTest->Options & 0x20;
						break;
					}
				}
			}
			if((testParam->BoardType == b935x) || (testParam->BoardType == b937x) ){
				para[Count++] = 0xA0;
				para[Count++] = 0x00;	//TODO: Test User Voltage
				para[Count++] = 0xA1;
				para[Count++] = 0x00;
				para[Count++] = 0xA2;
				para[Count++] = 0x01;
			}
			GlobalTestNum++;
			unsigned char ComRep;
			ComRep = 0x56;
			communication_array(ComRep, &para[0], Count);
	}
		else {
		TestingComplete = true;
		Comm_Ready = false;
		sampleUploadComplete = false;
		sprintf(Buffer,"\n ========== Maximum Test Number Reached: %d ==========",GlobalTestNum);
		CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
		HAL_UART_Transmit(&huart1, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);
		reset_ALL_MUX();
		reset_ALL_DAC();

		//Unmount SD card
		Unmount_SD("/");

		LCD_ClearLine(2);
		LCD_ClearLine(3);
		LCD_ClearLine(4);
		LCD_setCursor(2, 0);
		sprintf(Buffer, "    Test Results    ");
		LCD_printf(&Buffer[0], strlen(Buffer));
		uint8 spacing = (20 - (GlobalTestNum) - (GlobalTestNum-1));
		spacing = (spacing & 1) ? spacing+1 : spacing;
		spacing /= 2;
		LCD_setCursor(3, spacing);
		_Bool Passed = true;
		for (int i = 1; i <= GlobalTestNum; i++) {
			if (TestResults[i] == false) {
				sprintf(Buffer, "X ");
				LCD_printf(&Buffer[0], strlen(Buffer));
				Passed = false;
			} else {
				sprintf(Buffer, ". ");
				LCD_printf(&Buffer[0], strlen(Buffer));
			}
			TestResults[i] = true;
		}
		HAL_Delay(500);
		if (Passed) {
			LCD_setCursor(2, 0);
			sprintf(Buffer, "    Test Passed    ");
			LCD_printf(&Buffer[0], strlen(Buffer));
			HAL_GPIO_WritePin(PASS_FAIL_GPIO_Port, PASS_FAIL_Pin, GPIO_PIN_SET);
			TestPassed = true;
		}
	}
}
//	=================================================================================//
//	=================================================================================//
//	=================================================================================//
