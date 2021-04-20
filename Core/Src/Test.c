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
void TestConfig935x(TboardConfig * Board) {
	uint32 *TestArray935x[6][5] = {
			{ &latchTest, &currentTest,&OnevoltTest, &asyncDigitalTest, &asyncDigitalTest },
			{ &latchTest, &sdi12Test, &TwovoltTest, &asyncDigitalTest, &asyncDigitalTest },
			{ &latchTest, &asyncDigitalTest, &sdi12Test, &asyncDigitalTest,&asyncDigitalTest },
			{ &latchTest, &OnevoltTest, &currentTest, &asyncDigitalTest, &asyncDigitalTest },
			{ &latchTest, &TwovoltTest, &currentTest, &asyncDigitalTest, &asyncDigitalTest },
			{ &latchTest, &currentTest, &asyncTest, &asyncDigitalTest, &asyncDigitalTest }
	};
	uint8 portCodeArray935x[5][4] = {
			{ 0x80, 0x81, 0x82, 0x83 },
			{ 0xC0, 0xC1, 0xC2, 0xC3 },
			{ 0xD0, 0xD1, 0xD2, 0xD3 },
			{ 0xE0, 0xE1, 0xE2, 0x00 },
			{ 0xF0, 0xF1, 0xF2, 0x00 },
	};
	memcpy(&Board->PortCodes, &portCodeArray935x, sizeof(portCodeArray935x));

	Board->BoardType = b935x;
	Board->latchPortCount = 1;
	Board->analogInputCount = 2;
	Board->digitalInputCout = 2;
	Board->testNum = 6;
}

void TestConfig937x(TboardConfig * Board) {
	Board->BoardType = 0x9370;
	Board->Subclass = 'D';
	Board->latchPortCount = 2;
	Board->analogInputCount = 2;
	Board->digitalInputCout = 0;
	Board->testNum = 6;

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

	 uint8 tempPcARR[16] = {	//
			 0x80, 0x81, 0x00, 0x00 ,	//
			 0x84, 0x85, 0x88, 0x89 ,	//
			 0xC0, 0xC1, 0xC2, 0x00 ,	//
			 0xD0, 0xD1, 0xD2, 0x00 ,	//
	};
	memcpy(&Board->PortCodes, &tempPcARR, sizeof(tempPcARR));

}
//==================================================================================================================================//


//====================================================  INPUT EXPANSION BOARDS  ====================================================//
void TestConfig401x(TboardConfig * Board){
	uint32 *TestArray401x[6][4] = {
			{ &sdi12Test, &OnevoltTest, &currentTest, &asyncTest },
			{ &asyncTest, &sdi12Test, &OnevoltTest, &currentTest },
			{ &currentTest, &asyncTest, &sdi12Test, &OnevoltTest },
			{ &OnevoltTest, &currentTest, &asyncTest, &sdi12Test },
			{ &TwovoltTest, &currentTest, &TwovoltTest, &currentTest },
			{ &currentTest, &TwovoltTest, &currentTest, &TwovoltTest },
	};

	const uint8 * portCodeArray401x[4][4] = {
			{ 0xC0, 0xC1, 0xC2, 0xC3 },
			{ 0xD0, 0xD1, 0xD2, 0xD3 },
			{ 0xE0, 0xE1, 0xE2, 0xE3 },
			{ 0xF0, 0xF1, 0xF2, 0xF3 },
	};
	Board->BoardType = b401x;
	Board->latchPortCount = 0;
	Board->analogInputCount = 4;
	Board->digitalInputCout = 0;
	Board->testNum = 6;
//	Board->PortCodes = portCodeArray401x;
}

void TestConfig402x(TboardConfig * Board){
	uint32 *TestArray402x[7][10] = {
			{ &sdi12Test, &OnevoltTest, &OnevoltTest, &OnevoltTest, &currentTest, &currentTest, &asyncTest, &asyncTest, &asyncDigitalTest, &outputTest },
			{ &currentTest, &sdi12Test, &TwovoltTest, &TwovoltTest, &currentTest, &OnevoltTest, &asyncDigitalTest, &asyncTest, &asyncTest, &outputTest },
			{ &currentTest, &TwovoltTest, &sdi12Test, &currentTest, &OnevoltTest, &currentTest, &asyncTest, &asyncDigitalTest, &asyncTest, &noTest },
			{ &OnevoltTest, &currentTest, &currentTest, &sdi12Test, &TwovoltTest, &TwovoltTest, &asyncTest, &asyncTest, &asyncDigitalTest, &noTest },
			{ &TwovoltTest, &currentTest, &currentTest, &currentTest, &sdi12Test, &asyncDigitalTest, &asyncDigitalTest, &asyncTest, &asyncTest, &noTest },
			{ &asyncTest, &asyncDigitalTest, &asyncTest, &asyncDigitalTest, &asyncTest, &sdi12Test, &asyncTest, &asyncDigitalTest, &asyncTest, &noTest },
			{ &asyncDigitalTest, &asyncTest, &asyncDigitalTest, &asyncTest, &asyncDigitalTest, &asyncTest, &asyncDigitalTest, &asyncTest, &asyncDigitalTest, &noTest },
	};
						//Watch PARAMcount on 4021 boards as config on iConfigure had 5 parameters but 0x00 in the 5th config param, may not be required but check results
	const uint8 * portCodeArray402x[10][4] = {
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

	Board->BoardType = b402x;
	Board->latchPortCount = 1;
	Board->analogInputCount = 6;
	Board->digitalInputCout = 3;
	Board->testNum = 6;
//	Board->PortCodes = portCodeArray402x;
}
//==================================================================================================================================//


//====================================================  OUTPUT EXPANSION BOARDS  ====================================================//
void TestConfig422x(TboardConfig * Board){
	uint32 *TestArray422x[4][4] = {
			{ &latchTest, &noTest, &noTest, &noTest },
			{ &noTest, &latchTest, &noTest, &noTest },
			{ &noTest, &noTest, &latchTest, &noTest },
			{ &noTest, &noTest, &noTest, &latchTest },
	};

	const uint8 *  portCodeArray422x[4][2] = {
			{ 0xC0, 0xC1 },
			{ 0xC4, 0xC5 },
			{ 0xC8, 0xC9 },
			{ 0xCC, 0xCD },
	};

	Board->BoardType = b422x;
	Board->latchPortCount = 4;
	Board->analogInputCount = 0;
	Board->digitalInputCout = 0;
	Board->testNum = 4;
//	Board->PortCodes = portCodeArray422x;
}

void TestConfig427x(TboardConfig * Board){
	uint32 *TestArray427x[4][5] = {
			{ &latchTest, &noTest, &noTest, &noTest, &OnevoltTest },
			{ &noTest, &latchTest, &noTest, &noTest, &currentTest },
			{ &noTest, &noTest, &latchTest, &noTest, &asyncTest },
			{ &noTest, &noTest, &noTest, &latchTest, &sdi12Test },
	};

	const uint8 * portCodeArray427x[5][4] = {
			{ 0xC0, 0xC1, 0x00, 0x00 },
			{ 0xC4, 0xC5, 0x00, 0x00 },
			{ 0xC8, 0xC9, 0x00, 0x00 },
			{ 0xCC, 0xCD, 0x00, 0x00 },
			{ 0x90, 0x91, 0x92, 0x93 },
	};

	Board->BoardType = b427x;
	Board->latchPortCount = 4;
	Board->analogInputCount = 1;
	Board->digitalInputCout = 0;
	Board->testNum = 4;
//	Board->PortCodes = portCodeArray427x;
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
			if((Board->BoardType == b935x) || (Board->BoardType == b937x) && (Board->GlobalTestNum == 0)){
				*Para = 0xA0; (*Count)++; Para++;
				*Para = 0x00; (*Count)++; Para++;//TODO: Test User Voltage
				*Para = 0xA1; (*Count)++; Para++;
				*Para = 0x00; (*Count)++; Para++;
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
		CDC_Transmit_FS(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
		HAL_UART_Transmit(&huart1, &debugTransmitBuffer[0], strlen(debugTransmitBuffer), HAL_MAX_DELAY);
		reset_ALL_MUX();
		reset_ALL_DAC();

		//Unmount SD card
		Unmount_SD("/");

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
