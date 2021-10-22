/*
 * Board_Config.h
 *
 *  Created on: 24 Jul 2020
 *      Author: Admin
 */

#ifndef INC_BOARD_CONFIG_H_
#define INC_BOARD_CONFIG_H_

	#include <stdio.h>
	#include <stdlib.h>
	#include <main.h>
	//===============	 OUTPUT FUNCTIONALITY 	===============//
	#define TWO_WIRE_LATCHING 0x10	//One Pulse- Top 4 Bits, Two Wire Latching-Bottom 4 bits
	#define THREE_WIRE_LATCHING 0x11 // One Pulse-Top 4 Bits, Three Wire Latching-Bottom 4 bits
	#define DC_RELAY 0x02
	#define PUMP_START 0x03
	#define	FLOOD_GATE 0x04
	//===============		SENSOR VOLTAGES		===============//

	#define MAX_TEST_ARRAY_SIZE 81
	#define MAX_PORT_CODE_ARRAY_SIZE 64
	#define MAX_TEST_CODE_ARRAY_SIZE 10

	//============		BOARD STATUS VARIABLES		===========//
	#define BOARD_TEST_PASSED 	(1 << 0)
	#define BOARD_INITIALISED 	(1 << 1)
	#define BOARD_CALIBRATED 	(1 << 2)
	#define BOARD_PROGRAMMED	(1 << 3)
	#define BOARD_SERIALISED	(1 << 4)
	#define BOARD_POWER_STABLE	(1 << 5)

	//============		BOARD VOLTAGE VARIABLES		===========//
	#define FUSE_V_STABLE		(1 << 0)
	#define BOARD_SOLAR_STABLE	(1 << 1)
	#define V12_OUTPUT_STABLE	(1 << 2)
	#define V3_SAMPLE_STABLE	(1 << 3)
	#define V12_SAMPLE_STABLE	(1 << 4)
	#define BATT_LVL_STABLE		(1 << 5)

	//============		TEST PASSED VARIABLES		===========//
	#define TEST_ONE_PASSED 	(1 << 0)
	#define TEST_TWO_PASSED 	(1 << 1)
	#define TEST_THREE_PASSED 	(1 << 2)
	#define TEST_FOUR_PASSED 	(1 << 3)
	#define TEST_FIVE_PASSED 	(1 << 4)
	#define TEST_SIX_PASSED 	(1 << 5)
	#define TEST_SEVEN_PASSED 	(1 << 6)
	#define TEST_EIGHT_PASSED 	(1 << 7)

	//============		LATCH TEST VARIABLES		===========//
	#define LATCH_PORT_ONE 		(1 << 0)
	#define LATCH_PORT_TWO 		(1 << 1)
	#define LATCH_PORT_THREE 	(1 << 2)
	#define LATCH_PORT_FOUR 	(1 << 3)

	//============		PROGRAMMING VARIABLES		===========//
	#define PROG_INITIALISED 	(1 << 0)
	#define PROG_ENABLED		(1 << 1)
	#define CHIP_ERASED			(1 << 2)
	#define CLOCK_SET			(1 << 3)
	#define FUSE_VALIDATED		(1 << 4)
	#define FILE_FOUND_OPEN 	(1 << 5)
	#define PAGE_WRITE_READY	(1 << 6)
	#define FINAL_PAGE_WRITE	(1 << 7)

	//============			FUSE BYTES				===========//
	// Default FUSE Bytes
	#define EXT_FUSE_BYTE_40	0xFC
	#define EXT_FUSE_BYTE_90	0xFD
	#define HIGH_FUSE_BYTE 		0xD7
	#define LOW_FUSE_BYTE		0xDD
	//	High CLK FUSE Bytes
	#define HIGH_CLK_EXT_FUSE	0xFD
	#define HIGH_CLK_HIGH_FUSE	0xD7
	#define HIGH_CLK_LOW_FUSE	0xD2

	typedef struct {
			//=== Port Code ===//
			unsigned char Code;
			//=== Port Options ===//
			unsigned char Channels;
			unsigned char Options;
			unsigned char GateTime;
	}TportConfig;

	TportConfig outputTest;
	TportConfig latchTest;
	TportConfig currentTest;
	TportConfig OnevoltTest;
	TportConfig TwovoltTest;
	TportConfig asyncFilteredTest;
	TportConfig asyncUnfilteredTest;
	TportConfig sdi12Test;
	TportConfig rs485Test;
	TportConfig noTest;
	TportConfig SampleTime;

	typedef struct {
			TloomConnected BoardType;
			uns_ch Subclass;
			uint16 Module;
			uint16 Network;
			uint8 Version;
			uint8 latchPortCount;
			uint8 analogInputCount;
			uint8 digitalInputCout;
			uint8 testNum;
			uint8 ChCount;
			uint8 ArrayPtr;
			TportConfig * TestArray[MAX_TEST_ARRAY_SIZE];	//Pointer to TportConfig
			TportConfig * ThisTest;
			uint8 PortCodes[MAX_PORT_CODE_ARRAY_SIZE];
			uint8 TestCode[MAX_TEST_CODE_ARRAY_SIZE];
			int32 TestResults[8][17];
			uint8 LatchTestPort;
			uint32 SerialNumber;
			uint8 GlobalTestNum;
			float VoltageBuffer[8];
			uint8 rawBatteryLevel[7];
			float BatteryLevel;
			uint16 TPR; //TestPassedRegister;
			uint8 BSR; //Board Status Register
			uint8 LTR; //Latch Test Register
			uint8 BPR; //Board Programming Register
			uint8 BVR; // Board voltage register
	}TboardConfig;

	TboardConfig BoardConnected;
	uint32 * TestArray937x[6][4];


#endif /* INC_BOARD_CONFIG_H_ */
