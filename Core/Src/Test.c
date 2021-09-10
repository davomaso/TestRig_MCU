#include "main.h"
#include "Test.h"
#include "stdio.h"
#include "LCD.h"
#include "Board_Config.h"
#include "main.h"
#include "v1.h"
#include "Global_Variables.h"
#include "SetVsMeasured.h"
#include "UART_Routine.h"
#include "DAC.h"
#include "TestFunctions.h"

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
	OnevoltTest.GateTime = 64;

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

	//RS485 Test
	rs485Test.Code = AQUASPY;
	rs485Test.Channels = 1;
	rs485Test.Options = 0;
	rs485Test.GateTime = 0;
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
	uint32 *tempTestARR[30] = {	//
			(uint32*)&latchTest, (uint32*)&TwovoltTest, (uint32*)&OnevoltTest, (uint32*)&asyncTest, (uint32*)&asyncDigitalTest, 				//
			(uint32*)&noTest, (uint32*)&sdi12Test, (uint32*)&currentTest, (uint32*)&asyncDigitalTest, (uint32*)&asyncTest,				//
			(uint32*)&noTest, (uint32*)&asyncDigitalTest, (uint32*)&sdi12Test, (uint32*)&asyncDigitalTest, (uint32*)&asyncDigitalTest, 		//
			(uint32*)&noTest, (uint32*)&asyncTest, (uint32*)&currentTest, (uint32*)&asyncTest, (uint32*)&asyncDigitalTest, 				//
			(uint32*)&noTest, (uint32*)&currentTest, (uint32*)&TwovoltTest, (uint32*)&asyncDigitalTest, (uint32*)&asyncTest, 				//
			(uint32*)&noTest, (uint32*)&currentTest, (uint32*)&asyncTest, (uint32*)&asyncDigitalTest, (uint32*)&asyncDigitalTest		 		//
	};
	memcpy(&Board->TestArray, tempTestARR, sizeof(tempTestARR) );
	Board->ArrayPtr = 0;
		// Port Code Array
	uint8 tempPcARR[20] = {
			 0x80, 0x81, 0x82, 0x83 ,	//
			 0xC0, 0xC1, 0xC2, 0xC3 ,	//
			 0xD0, 0xD1, 0xD2, 0xD3 ,	//
			 0xE0, 0xE1, 0xE2, 0x00 ,	//
			 0xF0, 0xF1, 0xF2, 0x00 ,	//
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
					&latchTest, &noTest, &TwovoltTest, &asyncDigitalTest,	//
					&noTest, &latchTest, &TwovoltTest, &asyncTest,	//
					&noTest, &noTest, &sdi12Test, &currentTest,	//
					&noTest, &noTest, &asyncDigitalTest, &sdi12Test,	//
					&noTest, &noTest, &OnevoltTest, &currentTest,	//
					&noTest, &noTest, &currentTest, &OnevoltTest	//
			};
	memcpy(&Board->TestArray, tempTestARR, sizeof(tempTestARR));
	Board->ArrayPtr = 0;
		// Port Code Array
	 uint8 tempPcARR[16] = {	//
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
	TportConfig *tempTestARR[24] = {
			&(asyncDigitalTest), &(OnevoltTest), &(currentTest), &(asyncTest),		//
			&asyncTest,   &sdi12Test, &OnevoltTest, &currentTest,				//
			&currentTest, &asyncTest, &sdi12Test, &OnevoltTest,				//
			&OnevoltTest, &currentTest, &asyncTest, &sdi12Test,				//
			&TwovoltTest, &currentTest, &TwovoltTest, &currentTest,			//
			&currentTest, &TwovoltTest, &currentTest, &TwovoltTest,			//
	};
	memcpy(&Board->TestArray, tempTestARR, sizeof(tempTestARR));
	Board->ArrayPtr = 0;
		//Port Code Array
	uint8 tempPcARR[16] = {
			 0xC0, 0xC2, 0xC1, 0xC3,		//Transmit Control bits first, then channel number
			 0xD0, 0xD2, 0xD1, 0xD3,		//
			 0xE0, 0xE2, 0xE1, 0xE3,		//
			 0xF0, 0xF2, 0xF1, 0xF3,		//
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
	TportConfig *tempTestARR[70] = {
			&OnevoltTest, &OnevoltTest, &OnevoltTest, &OnevoltTest, &OnevoltTest, &OnevoltTest, &asyncTest, &asyncTest, &asyncTest, &outputTest,				//
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
	uint8 tempPcARR[40] = {
			0xA0, 0xA2, 0xA1, 0xA3,	//
			0xA8, 0xAA, 0xA9, 0xAB,	//
			0xB0, 0xB2, 0xB1, 0xB3,	//
			0xB8, 0xBA, 0xB9, 0xBB,	//
			0xC0, 0xC2, 0xC1, 0xC3,	//
			0xC8, 0xCA, 0xC9, 0xCB,	//
			0xD0, 0xDA, 0xD1, 0xD3,	//
			0xD8, 0xDA, 0xD9, 0xDB,	//
			0xE0, 0xE2, 0xE1, 0xE3,	//
			0xE8, 0x00, 0x00, 0x00,	//
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
	TportConfig *tempTestARR[16] = {
			&latchTest, &noTest, &noTest, &noTest,		//
			&noTest, &latchTest, &noTest, &noTest,		//
			&noTest, &noTest, &latchTest, &noTest,		//
			&noTest, &noTest, &noTest, &latchTest,		//
	};
	memcpy(&Board->TestArray, tempTestARR, sizeof(tempTestARR));
	Board->ArrayPtr = 0;

	uint8 tempPcARR[8] = {
			 0xC0, 0xC1 ,		//
			 0xC4, 0xC5,		//
			 0xC8, 0xC9,		//
			 0xCC, 0xCD,		//
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
	TportConfig *tempTestARR[20] = {
			&latchTest, &noTest, &noTest, &noTest, &asyncTest,		//
			&noTest, &latchTest, &noTest, &noTest, &OnevoltTest,	//
			&noTest, &noTest, &latchTest, &noTest, &currentTest,	//
			&noTest, &noTest, &noTest, &latchTest, &sdi12Test		//
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
			 if ( (Board->BoardType == b427x) && (Board->GlobalTestNum == 0) ) {
					*Para = 0x81; (*Count)++; Para++;
					*Para = 0x02; (*Count)++; Para++;
					*Para = 0x82; (*Count)++; Para++;
					*Para = 0x01; (*Count)++; Para++;
				} else if ( (Board->BoardType == b422x ) && (Board->GlobalTestNum == 0) ) {
					*Para = 0x80; (*Count)++; Para++;
					*Para = 0x64; (*Count)++; Para++;
					*Para = 0x83; (*Count)++; Para++;
					*Para = 0x80; (*Count)++; Para++;
				} else if ((Board->BoardType == b402x) && (Board->GlobalTestNum == 0) ) {
					*Para = 0x15; (*Count)++; Para++;
					*Para = 0x6E; (*Count)++; Para++;
					*Para = 0x16; (*Count)++; Para++;
					*Para = 0x78; (*Count)++; Para++;
					*Para = 0x80; (*Count)++; Para++;
					*Para = 0x01; (*Count)++; Para++;
					*Para = 0x81; (*Count)++; Para++;
					*Para = 0x02; (*Count)++; Para++;
					*Para = 0x82; (*Count)++; Para++;
					*Para = 0x14; (*Count)++; Para++;
					*Para = 0x83; (*Count)++; Para++;
					*Para = 0x1E; (*Count)++; Para++;
					*Para = 0x84; (*Count)++; Para++;
					*Para = 0x00; (*Count)++; Para++;
				}else if ((Board->BoardType == b401x) && (Board->GlobalTestNum == 0) ) {
					*Para = 0x80; (*Count)++; Para++;
					*Para = 0x01; (*Count)++; Para++;
					*Para = 0x81; (*Count)++; Para++;
					*Para = 0x02; (*Count)++; Para++;
					*Para = 0x82; (*Count)++; Para++;
					*Para = 0x14; (*Count)++; Para++;
					*Para = 0x83; (*Count)++; Para++;
					*Para = 0x78; (*Count)++; Para++;
					*Para = 0x84; (*Count)++; Para++;
					*Para = 0x00; (*Count)++; Para++;
				}
			for (uint8 PortCount = 0; PortCount < TotalPort; PortCount++) {
				Set_Test(Board, PortCount, TotalPort);	//Increment This test to the next testarray variable
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

				if (Board->ThisTest->Code == SDI_TWELVE)
					SDIenabled = true;
				if (Board->ThisTest->Code == AQUASPY)
					RS485enabled = true;
				if (Board->ThisTest->Code == ASYNC_PULSE) {
					Port[PortCount - (Board->latchPortCount)].Async.FilterEnabled = ((Board->ThisTest->Options) & 0x20);
					}
			}
			if( ( (Board->BoardType == b935x) || (Board->BoardType == b937x) ) && (Board->GlobalTestNum < 3)){
				*Para = 0xA0; (*Count)++; Para++;
				if (Board->GlobalTestNum == 2)
					*Para = 0x00;
				else
					*Para = 0x02;
				(*Count)++; Para++;
				*Para = 0xA1; (*Count)++; Para++;
				if (Board->GlobalTestNum == 0)
					*Para = 0x00;
				else
					*Para = 0x01;
				(*Count)++; Para++;
				*Para = 0xA2; (*Count)++; Para++;
				*Para = 0x01; (*Count)++; Para++;
			}
	}

void Set_Test(TboardConfig *Board, uint8 Port, uint8 TotalPort) {
	if (Port == 0)
		Board->ArrayPtr = Board->GlobalTestNum * TotalPort;
	Board->ThisTest =  Board->TestArray[Board->ArrayPtr++];
	Board->TestCode[Port] = Board->ThisTest->Code;
}

_Bool CheckTestNumber(TboardConfig * Board) {
	uint8 Test = Board->GlobalTestNum;
	uint8 maxTest = Board->testNum;
	if (Test == maxTest) {
		sprintf((char*) &debugTransmitBuffer,"\n ========== Maximum Test Number Reached: %d ==========\n",Test);
		printT((uns_ch*)&debugTransmitBuffer);
		reset_ALL_MUX();
		reset_ALL_DAC();

		LCD_ClearLine(3);
		LCD_ClearLine(4);
		LCD_printf((uns_ch*)"    Test Results    ",2,0);
		uint8 spacing = (20 - (Test) - (Test-1));
		spacing = (spacing & 1) ? spacing+1 : spacing;
		spacing /= 2;
		LCD_setCursor(3, spacing);
		for (uint8 i = 0; i < maxTest; i++) {
			if ( (Board->TPR & (1 << i) ) == true) {
				sprintf((char*)&debugTransmitBuffer[0], "X ");
				LCD_displayString((uns_ch*)&debugTransmitBuffer[0], strlen((char*)debugTransmitBuffer));
				CLEAR_BIT( Board->BSR, BOARD_TEST_PASSED );
			} else {
				sprintf((char*)&debugTransmitBuffer[0], ". ");
				LCD_displayString((uns_ch*)&debugTransmitBuffer[0], strlen((char*)debugTransmitBuffer));
			}
		}
		CheckPowerRegisters(Board);

		if ( (READ_REG(Board->TPR) == 0) && (READ_REG(Board->BSR == 0x3E) ) )
			SET_BIT(Board->BSR, BOARD_TEST_PASSED);
		return false;
	}
	return true;
}
//	=================================================================================//
//	=================================================================================//
