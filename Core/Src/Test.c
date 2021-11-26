#include "main.h"
#include "ADC_Variables.h"
#include "Test.h"
#include "stdio.h"
#include "LCD.h"
#include "Board_Config.h"
#include "main.h"
#include "v1.h"
#include "Global_Variables.h"
#include "SetVsMeasured.h"
#include "string.h"
#include "UART_Routine.h"
#include "DAC.h"
#include "TestFunctions.h"

// ==================	Global Assignment of Port Configs	================== //
void ConfigInit() {
	//Output Test Assignment
	outputTest.Code = 0x01;
	outputTest.Channels = 0x00;
	outputTest.GateTime = 0x00;

	//Latch Test Assignment
	latchTest.Code = TWO_WIRE_LATCHING;
	latchTest.Channels = 0x32;	//Pulse Time
	latchTest.Options = 0xFA;	//Delay Between Pulses
	latchTest.GateTime = 0x80;	//Options

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
	asyncFilteredTest.Code = ASYNC_PULSE;
	asyncFilteredTest.Channels = 1;
	asyncFilteredTest.Options = 0xA0;
	asyncFilteredTest.GateTime = 0;

	//Async Test for Digital Only Ports
	asyncUnfilteredTest.Code = ASYNC_PULSE;
	asyncUnfilteredTest.Channels = 1;
	asyncUnfilteredTest.Options = 0x80;
	asyncUnfilteredTest.GateTime = 0;

	//SDI12 Test
	sdi12Test.Code = SDI_TWELVE;	// 1200, 7, 1 , Even
	sdi12Test.Channels = 1;
	sdi12Test.Options = 0;
	sdi12Test.GateTime = 0;

	//RS485 Test
	rs485Test.Code = AQUASPY;		// 1200, 8, 1 , None
	rs485Test.Channels = 9;
	rs485Test.Options = 0x00;
	rs485Test.GateTime = 0;
	//No Test
	noTest.Code = NOTEST;
	noTest.Channels = NOTEST;
	noTest.Options = NOTEST;
	noTest.GateTime = NOTEST;
}
//==================================================================================================================================//

//=====================================================  SINGLE & DUAL BOARDS  =====================================================//
void TestConfig935x(TboardConfig *Board) {
	// Port Test Array
	TportConfig *tempTestARR[30] = {	//
			&latchTest, &asyncFilteredTest, &asyncUnfilteredTest, &asyncFilteredTest, &asyncFilteredTest, 			//
					&noTest, &sdi12Test, &TwovoltTest, &asyncFilteredTest, &asyncFilteredTest,						//
					&noTest, &asyncFilteredTest, &sdi12Test, &asyncFilteredTest, &asyncFilteredTest, 				//
					&noTest, &TwovoltTest, &asyncUnfilteredTest, &asyncFilteredTest, &asyncFilteredTest, 			//
					&noTest, &currentTest, &currentTest, &asyncFilteredTest, &asyncFilteredTest, 					//
					&noTest, &currentTest, &currentTest, &asyncFilteredTest, &asyncFilteredTest		 			//
			};
	memcpy(&Board->TestArray, tempTestARR, sizeof(tempTestARR));
	Board->ArrayPtr = 0;
	// Port Code Array
	uint8 tempPcARR[20] = { 0x80, 0x81, 0x82, 0x83,	//
			0xC0, 0xC1, 0xC2, 0xC3,	//
			0xD0, 0xD1, 0xD2, 0xD3,	//
			0xE0, 0xE1, 0xE2, 0x00,	//
			0xF0, 0xF1, 0xF2, 0x00,	//
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

void TestConfig937x(TboardConfig *Board) {
	// Port Test Array
	TportConfig *tempTestARR[24] = {	// Array size must not exceed size of MAX_TEST_ARRAY_SIZE
			&latchTest, &noTest, &asyncFilteredTest, &asyncUnfilteredTest,	//
					&noTest, &latchTest, &TwovoltTest, &asyncFilteredTest,			//
					&noTest, &noTest, &sdi12Test, &currentTest,				//
					&noTest, &noTest, &asyncUnfilteredTest, &sdi12Test,		//
					&noTest, &noTest, &OnevoltTest, &currentTest,			//
					&noTest, &noTest, &currentTest, &OnevoltTest			//
			};
	memcpy(&Board->TestArray, tempTestARR, sizeof(tempTestARR));
	Board->ArrayPtr = 0;
	// Port Code Array
	uint8 tempPcARR[16] = {	//
			0x80, 0x81, 0x88, 0x89,	// Ensure that never two latch on same test
					0x84, 0x85, 0x88, 0x89,	//
					0xC0, 0xC1, 0xC2, 0x00,	//
					0xD0, 0xD1, 0xD2, 0x00,	//
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
void TestConfig401x(TboardConfig *Board) {
	// Port Test Array
	TportConfig *tempTestARR[30] = { &sdi12Test, &OnevoltTest, &currentTest, &asyncFilteredTest, &noTest,			//
			&asyncFilteredTest, &sdi12Test, &OnevoltTest, &currentTest, &noTest,			//
			&currentTest, &asyncFilteredTest, &sdi12Test, &OnevoltTest, &noTest,			//
			&OnevoltTest, &currentTest, &asyncFilteredTest, &sdi12Test, &noTest,			//
			&TwovoltTest, &currentTest, &TwovoltTest, &currentTest, &noTest,				//
			&currentTest, &TwovoltTest, &currentTest, &TwovoltTest, &rs485Test				//
			};
	memcpy(&Board->TestArray, tempTestARR, sizeof(tempTestARR));
	Board->ArrayPtr = 0;
	//Port Code Array
	uint8 tempPcARR[20] = { 0xC0, 0xC2, 0xC1, 0xC3,		//Transmit Control bits first, then channel number
			0xD0, 0xD2, 0xD1, 0xD3,		//
			0xE0, 0xE2, 0xE1, 0xE3,		//
			0xF0, 0xF2, 0xF1, 0xF3,		//
			0xC8, 0xCA, 0xC9, 0xCB,		//
			};
	memcpy(&Board->PortCodes, &tempPcARR, sizeof(tempPcARR));
	//Board Type
	Board->BoardType = b401x;
	Board->Subclass = 0;
	// Quantity of each port type and tests
	Board->latchPortCount = 0;
	Board->analogInputCount = 5;
	Board->digitalInputCout = 0;
	Board->testNum = 6;
}

void TestConfig402x(TboardConfig *Board) {
	//Port Test Array
	TportConfig *tempTestARR[64] = { &sdi12Test, &OnevoltTest, &OnevoltTest, &OnevoltTest, &OnevoltTest, &OnevoltTest,
			&asyncFilteredTest, &asyncUnfilteredTest,
			&asyncUnfilteredTest,			//
			&currentTest, &sdi12Test, &currentTest, &currentTest, &currentTest, &currentTest, &asyncUnfilteredTest,
			&asyncFilteredTest,
			&asyncUnfilteredTest,			//
			&TwovoltTest, &TwovoltTest, &sdi12Test, &TwovoltTest, &TwovoltTest, &TwovoltTest, &asyncUnfilteredTest,
			&asyncUnfilteredTest,
			&asyncFilteredTest,			//
			&currentTest, &currentTest, &currentTest, &sdi12Test, &currentTest, &currentTest, &asyncFilteredTest,
			&asyncUnfilteredTest,
			&asyncUnfilteredTest,			//
			&OnevoltTest, &currentTest, &TwovoltTest, &currentTest, &sdi12Test, &OnevoltTest, &asyncUnfilteredTest,
			&asyncFilteredTest,
			&asyncUnfilteredTest,			//
			&TwovoltTest, &OnevoltTest, &OnevoltTest, &TwovoltTest, &OnevoltTest, &sdi12Test, &asyncUnfilteredTest,
			&asyncUnfilteredTest, &asyncFilteredTest, &outputTest		//
			};
	memcpy(&Board->TestArray, tempTestARR, sizeof(tempTestARR));
	Board->ArrayPtr = 0;
	//Port Code Array	//Watch PARAMcount on 4021 boards as config on iConfigure had 5 parameters but 0x00 in the 5th config param, may not be required but check results
	uint8 tempPcARR[36] = { 0xA0, 0xA2, 0xA1, 0xA3,	//
			0xA8, 0xAA, 0xA9, 0xAB,	//
			0xB0, 0xB2, 0xB1, 0xB3,	//
			0xB8, 0xBA, 0xB9, 0xBB,	//
			0xC0, 0xC2, 0xC1, 0xC3,	//
			0xC8, 0xCA, 0xC9, 0xCB,	//
			0xD0, 0xD2, 0xD1, 0xD3,	//
			0xD8, 0xDA, 0xD9, 0xDB,	//
			0xE0, 0xE2, 0xE1, 0xE3,	//
			};
	memcpy(&Board->PortCodes, &tempPcARR, sizeof(tempPcARR));
	//Board Type
	Board->BoardType = b402x;
	Board->Subclass = 0;
	//Quantity of each port type and tests
	Board->latchPortCount = 0;
	Board->analogInputCount = 6;
	Board->digitalInputCout = 3;
	Board->testNum = 6;
}
//==================================================================================================================================//

//====================================================  OUTPUT EXPANSION BOARDS  ====================================================//
void TestConfig422x(TboardConfig *Board) {
	TportConfig *tempTestARR[16] = { &latchTest, &noTest, &noTest, &noTest,		//
			&noTest, &latchTest, &noTest, &noTest,		//
			&noTest, &noTest, &latchTest, &noTest,		//
			&noTest, &noTest, &noTest, &latchTest,		//
			};
	memcpy(&Board->TestArray, tempTestARR, sizeof(tempTestARR));
	Board->ArrayPtr = 0;

	uint8 tempPcARR[8] = { 0xC0, 0xC1,		//
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

void TestConfig427x(TboardConfig *Board) {
	TportConfig *tempTestARR[20] = { &latchTest, &noTest, &noTest, &noTest, &asyncFilteredTest,		//
			&noTest, &latchTest, &noTest, &noTest, &OnevoltTest,	//
			&noTest, &noTest, &latchTest, &noTest, &currentTest,	//
			&noTest, &noTest, &noTest, &latchTest, &sdi12Test		//
			};
	memcpy(&Board->TestArray, tempTestARR, sizeof(tempTestARR));
	Board->ArrayPtr = 0;

	uint8 tempPcARR[20] = { 0xC0, 0xC1, 0x00, 0x00,	//
			0xC4, 0xC5, 0x00, 0x00, //
			0xC8, 0xC9, 0x00, 0x00, //
			0xCC, 0xCD, 0x00, 0x00, //
			0x90, 0x91, 0x92, 0x93 //
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
void SetTestParam(TboardConfig *Board, uint8 TestCount, uns_ch *Para, uint8 *Count) {
	uint8 TotalPort = Board->latchPortCount + Board->analogInputCount + Board->digitalInputCout; //Error check to ensure no overflow of test count
	*Count = 0;
//			if ( (Board->BoardType == b401x) && (Board->GlobalTestNum == Board->testNum) )
//				TotalPort += 1; // Allow for
	// ======= Load Both Arrays into Para to be sent to Target Board  ======= //
	uint8 *PCptr = &(Board->PortCodes[0]);
	SetVoltageParameters(Board, Para, Count);
	Para += *Count;
	Board->ChCount = 0; //Acount for battery voltage
	for (uint8 PortCount = 0; PortCount <= TotalPort; PortCount++) {
		Set_Test(Board, PortCount, TotalPort);	//Increment This test to the next testarray variable
		if ((Board->ThisTest->Code) && *PCptr) {	//
			*Para = *PCptr++;
			(*Count)++;
			Para++;
			*Para = Board->ThisTest->Code;
			(*Count)++;
			Para++;

			if (*PCptr) {
				*Para = *PCptr++;
				(*Count)++;
				Para++;
				*Para = Board->ThisTest->Channels;
				if (Board->ThisTest->Code == TWO_WIRE_LATCHING)
					Board->ChCount += 2;
				else
					Board->ChCount += Board->ThisTest->Channels;
				(*Count)++;
				Para++;
			} else
				PCptr++;

			if (*PCptr) {	//Ensure Options byte and the port code exist
				*Para = *PCptr++;
				(*Count)++;
				Para++;
				*Para = Board->ThisTest->Options;
				(*Count)++;
				Para++;
			} else
				PCptr++;

			if (*PCptr) { //Ensure Gatetime byte and the port code exist
				*Para = *PCptr++;
				(*Count)++;
				Para++;
				if ((Board->BoardType == b402x) && (Board->ThisTest->Code == ASYNC_PULSE))
					*Para = 0x64;
				else
					*Para = Board->ThisTest->GateTime;
				(*Count)++;
				Para++;
			} else
				PCptr++;
		} else
			PCptr += 4;

		if ((Board->ThisTest->Code == NOTEST) && (PortCount < Board->latchPortCount))
			Board->ChCount += 2;

		if (Board->ThisTest->Code == SDI_TWELVE) {
			SDIenabled = true;
			USART6->CR1 &= ~(USART_CR1_UE);
			USART6->CR1 |= (USART_CR1_RXNEIE);			// Enable receivable data on UART6
			USART6->CR1 &= ~UART_PARITY_ODD;
			USART6->CR1 |= UART_PARITY_EVEN;	// Set Parity to even
			USART6->CR1 |= (USART_CR1_UE);
		}
		if (Board->ThisTest->Code == AQUASPY) {
			RS485enabled = true;						// Enable RS485
			USART6->CR1 &= ~(USART_CR1_UE);
			USART6->CR1 |= (USART_CR1_RXNEIE);			// Enable receivable data on UART6
			USART6->CR1 &= ~UART_PARITY_ODD;
			USART6->CR1 |= UART_PARITY_NONE;	// Set Parity to none
			USART6->CR1 |= (USART_CR1_UE);
		}
		if (Board->ThisTest->Code == ASYNC_PULSE) {
			if ((Board->BoardType == b935x) && (PortCount > 2))
				Port[PortCount + 3].Async.FilterEnabled = ((Board->ThisTest->Options) & 0x20);
			else
				Port[PortCount - (Board->latchPortCount)].Async.FilterEnabled = ((Board->ThisTest->Options) & 0x20);
		}
	}
	if (Board->BoardType == b402x) {
		*Para++ = 0xE8;
		*Para++ = 0x01;
		if (Board->GlobalTestNum == 5)
			RelayPort_Enabled = true;
		*Count += 2;
	}
}

void Set_Test(TboardConfig *Board, uint8 Port, uint8 TotalPort) {
	if (Port == 0)
		Board->ArrayPtr = Board->GlobalTestNum * TotalPort;
	Board->ThisTest = Board->TestArray[Board->ArrayPtr++];
	Board->TestCode[Port] = Board->ThisTest->Code;
}

_Bool CheckTestNumber(TboardConfig *Board) {
	uint8 Test = Board->GlobalTestNum;
	uint8 maxTest = Board->testNum;
	SDIenabled = false;
	RS485enabled = false;
	if (Test == maxTest) {
		sprintf((char*) &debugTransmitBuffer, "\n ========== Maximum Test Number Reached: %d ==========\n", Test);
		printT((uns_ch*) &debugTransmitBuffer);
		reset_ALL_MUX();
		reset_ALL_DAC();

		LCD_ClearLine(3);
		LCD_ClearLine(4);
		LCD_printf((uns_ch*) "    Test Results    ", 2, 0);
		uint8 spacing = (20 - (Test) - (Test - 1));
		spacing = (spacing & 1) ? spacing + 1 : spacing;
		spacing /= 2;
		LCD_setCursor(3, spacing);
		for (uint8 i = 0; i < maxTest; i++) {
			if (Board->TPR & (1 << i)) {
				sprintf((char*) &debugTransmitBuffer[0], "X ");
				LCD_displayString((uns_ch*) &debugTransmitBuffer[0], strlen((char*) debugTransmitBuffer));
				CLEAR_BIT(Board->BSR, BOARD_TEST_PASSED);
			} else {
				sprintf((char*) &debugTransmitBuffer[0], ". ");
				LCD_displayString((uns_ch*) &debugTransmitBuffer[0], strlen((char*) debugTransmitBuffer));
			}
		}
		GetBatteryLevel(Board);
		CheckPowerRegisters(Board);

		if ((READ_REG(Board->TPR) == 0) && (READ_REG(Board->BSR == 0x1E)))
			SET_BIT(Board->BSR, BOARD_TEST_PASSED);
		return false;
	}
	return true;
}

void SetVoltageParameters(TboardConfig *Board, uns_ch *Para, uint8 *Count) {

	if (Board->BoardType == b427x) {
//		uns_ch tempBuffer[SMLBUFFER] = {
//				0x81,	//
//				0x02,	//
//				0x82,	//
//				0x01	//
//				};
//		memcpy(Para, &tempBuffer[0], sizeof(&tempBuffer[0]) );
//		Para += sizeof(tempBuffer);
//		*Count += sizeof(tempBuffer);

		*Para = 0x81;
		(*Count)++;
		Para++;
		*Para = 0x02;
		(*Count)++;
		Para++;
		*Para = 0x82;
		(*Count)++;
		Para++;
		*Para = 0x01;
		(*Count)++;
		Para++;
	} else if (Board->BoardType == b422x) {
		uns_ch tempBuffer[SMLBUFFER] = { 0x80,	//
				0x64,	//
				0x83,	//
				0x80	//
				};
		memcpy(Para, &tempBuffer[0], sizeof((char*) &tempBuffer[0]));
		Para += sizeof(tempBuffer);
		*Count += sizeof(tempBuffer);
//		*Para = 0x80;
//		(*Count)++;
//		Para++;
//		*Para = 0x64;
//		(*Count)++;
//		Para++;
//		*Para = 0x83;
//		(*Count)++;
//		Para++;
//		*Para = 0x80;
//		(*Count)++;
//		Para++;
	} else if (Board->BoardType == b402x) {	// 4020 Sample Voltage
		*Para = 0x15;
		(*Count)++;
		Para++;
		*Para = 0x6E;
		(*Count)++;
		Para++;
		*Para = 0x16;
		(*Count)++;
		Para++;
		*Para = 0x78;
		(*Count)++;
		Para++;
		*Para = 0x80;
		(*Count)++;
		Para++;
		*Para = 0x02;
		(*Count)++;
		Para++;
		*Para = 0x81;
		(*Count)++;
		Para++;
		*Para = 0x02;
		(*Count)++;
		Para++;
		*Para = 0x82;
		(*Count)++;
		Para++;
		*Para = 0x14;
		(*Count)++;
		Para++;
		*Para = 0x83;
		(*Count)++;
		Para++;
		if (Board->GlobalTestNum == V_105 || Board->GlobalTestNum == V_trim)
			*Para = TEN_VOLT_SAMPLE_THRESHOLD * 10;	// 0x69 is 10.5
		else if (Board->GlobalTestNum == V_12)
			*Para = TWELVE_VOLT_SAMPLE_VALUE * 10;
		else if (Board->GlobalTestNum == V_3)
			*Para = THREE_VOLT_SAMPLE_VALUE * 10;
		else
			*Para = 0x00;
		(*Count)++;
		Para++;
		*Para = 0x84;
		(*Count)++;
		Para++; // Set trim to get as close to Vuser
		if (BoardConnected.GlobalTestNum == V_trim) {
			uint8 trim = round((10.5 - Board->VoltageBuffer[V_105]) * 333.333);
			*Para = trim;
		} else
			*Para = 0x00;
		(*Count)++;
		Para++;
	} else if (Board->BoardType == b401x) {	// 4011 Sample Voltage
		*Para = 0x80;
		(*Count)++;
		Para++;
		*Para = 0x02;
		(*Count)++;
		Para++;
		*Para = 0x81;
		(*Count)++;
		Para++;
		*Para = 0x02;
		(*Count)++;
		Para++;
		*Para = 0x82;
		(*Count)++;
		Para++;
		*Para = 0x01;
		(*Count)++;
		Para++;
		*Para = 0x83;
		(*Count)++;
		Para++;
		if (Board->GlobalTestNum == V_105 || Board->GlobalTestNum == V_trim)
			*Para = TEN_VOLT_SAMPLE_THRESHOLD * 10;		// 0x69 is 10.5
		else if (Board->GlobalTestNum == V_12)
			*Para = TWELVE_VOLT_SAMPLE_VALUE * 10;
		else if (Board->GlobalTestNum == V_3)
			*Para = THREE_VOLT_SAMPLE_VALUE * 10;
		else
			*Para = 0x00;
		(*Count)++;
		Para++;
		*Para = 0x84;
		(*Count)++;
		Para++; // Set trim to get as close to Vuser
		if (BoardConnected.GlobalTestNum == V_trim) {
			uint8 trim = round((10.5 - Board->VoltageBuffer[V_105]) * 333.333);
			*Para = trim;
		} else
			*Para = 0x00;
		(*Count)++;
		Para++;
	} else if (((Board->BoardType == b935x) || (Board->BoardType == b937x))) {
		*Para++ = 0xA0;
		(*Count)++;			// Permanently on command
		*Para++ = 0x02;
		(*Count)++;
		*Para++ = 0xA1;
		(*Count)++;			// Voltage Selection Command
		if (Board->GlobalTestNum == V_12 || Board->GlobalTestNum > V_3)
			*Para++ = 0x00;		// Driven off menu rather than sample
		else if (Board->GlobalTestNum == V_3)
			*Para++ = 0x01;
		else
			*Para++ = 0x02;
		(*Count)++;
		*Para++ = 0xA2;
		(*Count)++;
		*Para++ = 0x01;
		(*Count)++;

	}
}
//	=================================================================================//
//	=================================================================================//
