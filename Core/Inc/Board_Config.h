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
	#define MAX_FILE_NAME_LENGTH 32

	//============		BOARD STATUS VARIABLES		===========//
	#define BOARD_TEST_PASSED 	(1 << 0)
	#define BOARD_INITIALISED 	(1 << 1)
	#define BOARD_CALIBRATED 	(1 << 2)
	#define BOARD_PROGRAMMED	(1 << 3)
	#define BOARD_SERIALISED	(1 << 4)

	//============		TEST PASSED VARIABLES		===========//
	#define TEST_ONE_PASSED 	(1 << 0)
	#define TEST_TWO_PASSED 	(1 << 1)
	#define TEST_THREE_PASSED 	(1 << 2)
	#define TEST_FOUR_PASSED 	(1 << 3)
	#define TEST_FIVE_PASSED 	(1 << 4)
	#define TEST_SIX_PASSED 	(1 << 5)
	#define TEST_SEVEN_PASSED 	(1 << 6)
	#define TEST_EIGHT_PASSED 	(1 << 7)

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
	TportConfig asyncTest;
	TportConfig asyncDigitalTest;
	TportConfig sdi12Test;
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
			uint8 ArrayPtr;
			TportConfig * TestArray[MAX_TEST_ARRAY_SIZE];	//Pointer to TportConfig
			TportConfig * ThisTest;
			uint8 PortCodes[MAX_PORT_CODE_ARRAY_SIZE];
			uint8 TestCode[MAX_TEST_CODE_ARRAY_SIZE];
			uint32 TestResults[8][16];
			uint16 TPR; //TestPassedRegister;
			uint32 SerialNumber;
			uint8 GlobalTestNum;
			uint8 BSR; //Board Status Register
			uint8 LTR; //Latch Test Register
	}TboardConfig;

	TboardConfig BoardConnected;
	uint32 * TestArray937x[6][4];


#endif /* INC_BOARD_CONFIG_H_ */
