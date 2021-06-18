#include "Test.h"
#include "stdio.h"
#include "Board_Config.h"
#include "main.h"
#include "interogate_project.h"
#include "SetVsMeasured.h"

void SetTestParam(TboardConfig*, uint8, uns_ch *, uint8 *);
extern TestFunction(TboardConfig *);

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
	asyncDigitalTest.Channels = 2;
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
void TestConfig935x(TboardConfig * Board) {
		// Port Test Array
	uint32 *tempTestARR[30] = {
			&latchTest, &OnevoltTest,&OnevoltTest, &asyncTest, &asyncDigitalTest, 				//
			&latchTest, &OnevoltTest, &TwovoltTest, &asyncDigitalTest, &asyncTest,				//
			&noTest, &asyncDigitalTest, &sdi12Test, &asyncDigitalTest,&asyncDigitalTest, 		//
			&noTest, &OnevoltTest, &currentTest, &asyncTest, &asyncDigitalTest, 				//
			&noTest, &TwovoltTest, &currentTest, &asyncDigitalTest, &asyncTest, 				//
			&noTest, &currentTest, &asyncTest, &asyncDigitalTest, &asyncDigitalTest		 		//
	};
	memcpy(&Board->TestArray, tempTestARR, sizeof(tempTestARR) );
	Board->ArrayPtr = 0;
		// Port Code Array
	uint8 tempPcARR[5][4] = {
			{ 0x80, 0x81, 0x82, 0x83 },
			{ 0xC0, 0xC1, 0xC2, 0xC3 },
			{ 0xD0, 0xD1, 0xD2, 0xD3 },
			{ 0xE0, 0xE1, 0xE2, 0x00 },
			{ 0xF0, 0xF1, 0xF2, 0x00 },
	};
	memcpy(&Board->PortCodes, &tempPcARR, sizeof(tempPcARR));

	Board->BoardType = b935x;
	Board->Subclass = 'C';
		// Quantity of each Port type & tests
	Board->latchPortCount = 1;
	Board->analogInputCount = 2;
	Board->digitalInputCout = 2;
	Board->testNum = 6;
}

void TestConfig937x(TboardConfig * Board) {
		// Port Test Array
	TportConfig *tempTestARR[24] = {	// Array size must not exceed size of MAX_TEST_ARRAY_SIZE
					&latchTest, &noTest, &OnevoltTest, &asyncDigitalTest,	//
					&noTest, &latchTest, &TwovoltTest, &asyncDigitalTest,	//
					&noTest, &noTest, &sdi12Test, &currentTest,	//
					&noTest, &noTest, &asyncDigitalTest, &sdi12Test,	//
					&noTest, &noTest, &currentTest, &TwovoltTest,	//
					&noTest, &noTest, &currentTest, &OnevoltTest	//
			};
	memcpy(&Board->TestArray, tempTestARR, sizeof(tempTestARR));
	Board->ArrayPtr = 0;
		// Port Code Array
	 uint8 tempPcARR[4][4] = {	//
			 0x80, 0x81, 0x00, 0x00 ,	//
			 0x84, 0x85, 0x88, 0x89 ,	//
			 0xC0, 0xC1, 0xC2, 0x00 ,	//
			 0xD0, 0xD1, 0xD2, 0x00 ,	//
	};
	memcpy(&Board->PortCodes, &tempPcARR, sizeof(tempPcARR));
		//Board Type
	Board->BoardType = b937x;
	Board->Subclass = 'D';
		//Quantity of each Port type & tests
	Board->latchPortCount = 2;
	Board->analogInputCount = 2;
	Board->digitalInputCout = 0;
	Board->testNum = 6;
}
//==================================================================================================================================//


//====================================================  INPUT EXPANSION BOARDS  ====================================================//
void TestConfig401x(TboardConfig * Board){
		// Port Test Array
	uint32 *tempTestARR[24] = {
			&asyncDigitalTest, &OnevoltTest, &currentTest, &asyncTest,		//
			&asyncTest, &sdi12Test, &OnevoltTest, &currentTest,				//
			&currentTest, &asyncTest, &sdi12Test, &OnevoltTest,				//
			&OnevoltTest, &currentTest, &asyncTest, &sdi12Test,				//
			&TwovoltTest, &currentTest, &TwovoltTest, &currentTest,			//
			&currentTest, &TwovoltTest, &currentTest, &TwovoltTest,			//
	};
	memcpy(&Board->TestArray, tempTestARR, sizeof(tempTestARR));
	Board->ArrayPtr = 0;
		//Port Code Array
	uint8 tempPcARR[4][4] = {
			{ 0xC0, 0xC2, 0xC1, 0xC3 },	//Transmit Control bits first, then channel number
			{ 0xD0, 0xD2, 0xD1, 0xD3 },
			{ 0xE0, 0xE2, 0xE1, 0xE3 },
			{ 0xF0, 0xF2, 0xF1, 0xF3 },
	};
	memcpy(&Board->PortCodes, &tempPcARR, sizeof(tempPcARR));
		//Board Type
	Board->BoardType = b401x;
	Board->Subclass = 0;
		// Quantity of each port type and tests
	Board->latchPortCount = 0;
	Board->analogInputCount = 4;
	Board->digitalInputCout = 0;
	Board->testNum = 6;
}

void TestConfig402x(TboardConfig * Board){
		//Port Test Array
	uint32 *tempTestARR[70] = {
			&sdi12Test, &OnevoltTest, &OnevoltTest, &OnevoltTest, &currentTest, &currentTest, &asyncTest, &asyncTest, &asyncDigitalTest, &outputTest,				//
			&currentTest, &sdi12Test, &TwovoltTest, &TwovoltTest, &currentTest, &OnevoltTest, &asyncDigitalTest, &asyncTest, &asyncTest, &outputTest,				//
			&currentTest, &TwovoltTest, &sdi12Test, &currentTest, &OnevoltTest, &currentTest, &asyncTest, &asyncDigitalTest, &asyncTest, &noTest,					//
			&OnevoltTest, &currentTest, &currentTest, &sdi12Test, &TwovoltTest, &TwovoltTest, &asyncTest, &asyncTest, &asyncDigitalTest, &noTest,					//
			&TwovoltTest, &currentTest, &currentTest, &currentTest, &sdi12Test, &asyncDigitalTest, &asyncDigitalTest, &asyncTest, &asyncTest, &noTest,				//
			&asyncTest, &asyncDigitalTest, &asyncTest, &asyncDigitalTest, &asyncTest, &sdi12Test, &asyncTest, &asyncDigitalTest, &asyncTest, &noTest,				//
			&asyncDigitalTest, &asyncTest, &asyncDigitalTest, &asyncTest, &asyncDigitalTest, &asyncTest, &asyncDigitalTest, &asyncTest, &asyncDigitalTest, &noTest,	//
	};
	memcpy(&Board->TestArray, tempTestARR, sizeof(tempTestARR));
	Board->ArrayPtr = 0;
		//Port Code Array	//Watch PARAMcount on 4021 boards as config on iConfigure had 5 parameters but 0x00 in the 5th config param, may not be required but check results
	uint8 tempPcARR[10][4] = {
			{ 0xA0, 0xA2, 0xA1, 0xA3},
			{ 0xA8, 0xAA, 0xA9, 0xAB},
			{ 0xB0, 0xB2, 0xB1, 0xB3},
			{ 0xB8, 0xBA, 0xB9, 0xBB},
			{ 0xC0, 0xC2, 0xC1, 0xC3},
			{ 0xC8, 0xCA, 0xC9, 0xCB},
			{ 0xD0, 0xDA, 0xD1, 0xD3},
			{ 0xD8, 0xDA, 0xD9, 0xDB},
			{ 0xE0, 0xE2, 0xE1, 0xE3},
			{ 0xE8, 0x00, 0x00, 0x00},
	};
	memcpy(&Board->PortCodes, &tempPcARR, sizeof(tempPcARR));
		//Board Type
	Board->BoardType = b402x;
	Board->Subclass = 0;
		//Quantity of each port type and tests
	Board->latchPortCount = 1;
	Board->analogInputCount = 6;
	Board->digitalInputCout = 3;
	Board->testNum = 7;
}
//==================================================================================================================================//


//====================================================  OUTPUT EXPANSION BOARDS  ====================================================//
void TestConfig422x(TboardConfig * Board){
	uint32 *tempTestARR[16] = {
			&latchTest, &noTest, &noTest, &noTest,		//
			&noTest, &latchTest, &noTest, &noTest,		//
			&noTest, &noTest, &latchTest, &noTest,		//
			&noTest, &noTest, &noTest, &latchTest,		//
	};
	memcpy(&Board->TestArray, tempTestARR, sizeof(tempTestARR));
	Board->ArrayPtr = 0;

	uint8 tempPcARR[4][2] = {
			{ 0xC0, 0xC1 },
			{ 0xC4, 0xC5 },
			{ 0xC8, 0xC9 },
			{ 0xCC, 0xCD },
	};
	memcpy(&Board->PortCodes, &tempPcARR, sizeof(tempPcARR));

	Board->BoardType = b422x;
	Board->Subclass = 0;
	Board->latchPortCount = 4;
	Board->analogInputCount = 0;
	Board->digitalInputCout = 0;
	Board->testNum = 4;
}

void TestConfig427x(TboardConfig * Board){
	uint32 *tempTestARR[20] = {
			&latchTest, &noTest, &noTest, &noTest, &OnevoltTest,	//
			&noTest, &latchTest, &noTest, &noTest, &currentTest,	//
			&noTest, &noTest, &latchTest, &noTest, &asyncTest,		//
			&noTest, &noTest, &noTest, &latchTest, &sdi12Test,		//
	};
	memcpy(&Board->TestArray, tempTestARR, sizeof(tempTestARR));
	Board->ArrayPtr = 0;

	uint8 tempPcARR[5][4] = {
			{ 0xC0, 0xC1, 0x00, 0x00 },
			{ 0xC4, 0xC5, 0x00, 0x00 },
			{ 0xC8, 0xC9, 0x00, 0x00 },
			{ 0xCC, 0xCD, 0x00, 0x00 },
			{ 0x90, 0x91, 0x92, 0x93 },
	};
	memcpy(&Board->PortCodes, &tempPcARR, sizeof(tempPcARR));

	Board->BoardType = b427x;
	Board->Subclass = 0;
	Board->latchPortCount = 4;
	Board->analogInputCount = 1;
	Board->digitalInputCout = 0;
	Board->testNum = 4;
}
//==================================================================================================================================//


 //=====================================================  SET TEST PARAMETERS  =====================================================//
void SetTestParam(TboardConfig *Board, uint8 TestCount, uns_ch * Para, uint8 * Count) {
		uint8 TotalPort = Board->latchPortCount + Board->analogInputCount + Board->digitalInputCout;	//Error check to ensure no overflow of test count
			*Count = 0;
			// ======= Load Both Arrays into Para to be sent to Target Board  ======= //
			uint8 * PCptr = &(Board->PortCodes[0]);
			for (uint8 PortCount = 0; PortCount < TotalPort; PortCount++) {
				Set_Test(Board, PortCount);	//Increment This test to the next testarray variable
				if(Board->ThisTest->Code){
					*Para = *PCptr++;
					(*Count)++; Para++;
					*Para = Board->ThisTest->Code;
					(*Count)++; Para++;

					if(*PCptr){
						*Para = *PCptr++;
						(*Count)++; Para++;
						*Para = Board->ThisTest->Channels;
						(*Count)++; Para++;
					} else PCptr++;

					if ( (Board->ThisTest->Options) && *PCptr) {
						*Para = *PCptr++;
						(*Count)++; Para++;
						*Para = Board->ThisTest->Options;
						(*Count)++; Para++;
					} else PCptr++;

					if ( (Board->ThisTest->GateTime) && *PCptr ) {
						*Para = *PCptr++;
						(*Count)++; Para++;
						*Para = Board->ThisTest->GateTime;
						(*Count)++; Para++;
					} else PCptr++;
				} else PCptr += 4;

				if(Board->ThisTest->Code == SDI_TWELVE)
					SDIenabled = true;
				if(Board->ThisTest->Code == ASYNC_PULSE){
					switch( PortCount - (Board->latchPortCount) ){
					case Port_1:
						Async_Port1.FilterEnabled = ((Board->ThisTest->Options) & 0x20);
						break;
					case Port_2:
						Async_Port2.FilterEnabled = (Board->ThisTest->Options & 0x20);
 						break;
					case Port_3:
						Async_Port3.FilterEnabled = (Board->ThisTest->Options & 0x20);
						break;
					case Port_4:
						Async_Port4.FilterEnabled = (Board->ThisTest->Options & 0x20);
						break;
					case Port_5:
						Async_Port5.FilterEnabled = (Board->ThisTest->Options & 0x20);
						break;
					case Port_6:
						Async_Port6.FilterEnabled = (Board->ThisTest->Options & 0x20);
						break;
					case Port_7:
						Async_Port7.FilterEnabled = (Board->ThisTest->Options & 0x20);
						break;
					case Port_8:
						Async_Port8.FilterEnabled = (Board->ThisTest->Options & 0x20);
						break;
					case Port_9:
						Async_Port9.FilterEnabled = (Board->ThisTest->Options & 0x20);
						break;
					}
				}
			}
			if( ( (Board->BoardType == b935x) || (Board->BoardType == b937x) ) && (Board->GlobalTestNum == 0)){
				*Para = 0xA0; (*Count)++; Para++;
				*Para = 0x02; (*Count)++; Para++;
				*Para = 0xA1; (*Count)++; Para++;
				*Para = 0x01; (*Count)++; Para++;
				*Para = 0xA2; (*Count)++; Para++;
				*Para = 0x01; (*Count)++; Para++;
			}

	}

void Set_Test(TboardConfig *Board, uint8 Port) {
	Board->ThisTest =  Board->TestArray[Board->ArrayPtr++];
	Board->TestCode[Port] = Board->ThisTest->Code;
}

_Bool CheckTestNumber(TboardConfig * Board) {
	uint8 Test = Board->GlobalTestNum;
	uint8 maxTest = Board->testNum;
	if (Test == maxTest) {
		sprintf(debugTransmitBuffer,"\n ========== Maximum Test Number Reached: %d ==========\n",Test);
		printT(&debugTransmitBuffer);
		reset_ALL_MUX();
		reset_ALL_DAC();

		LCD_ClearLine(2);
		LCD_ClearLine(3);
		LCD_ClearLine(4);
		LCD_setCursor(2, 0);
		sprintf(debugTransmitBuffer, "    Test Results    ");
		LCD_printf(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
		uint8 spacing = (20 - (Test) - (Test-1));
		spacing = (spacing & 1) ? spacing+1 : spacing;
		spacing /= 2;
		LCD_setCursor(3, spacing);
		for (uint8 i = 0; i < Test; i++) {
			if ( ( Board->TPR & (1 << i) ) == false) {
				sprintf(debugTransmitBuffer, "X ");
				LCD_printf(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
				CLEAR_BIT( Board->BSR, BOARD_TEST_PASSED );
			} else {
				sprintf(debugTransmitBuffer, ". ");
				LCD_printf(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
			}
		}
			//Not 100% required
		if ( (Board->TPR == 0xFFFF) )
			SET_BIT(Board->BSR, BOARD_TEST_PASSED);
		return false;
		HAL_Delay(500);
	}
	return true;
}
//	=================================================================================//
//	=================================================================================//
//	=================================================================================//
