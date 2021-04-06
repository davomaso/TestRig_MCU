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
			uint8 latchPortCount;
			uint8 analogInputCount;//change all channels to ports'
			uint8 digitalInputCout;
			uint8 testNum;
			uint8 ArrayPtr;
			TportConfig * TestArray[MAX_TEST_ARRAY_SIZE];	//Pointer to TportConfig
			TportConfig * ThisTest;
			uint8 PortCodes[MAX_PORT_CODE_ARRAY_SIZE];
			uint8 TestCode[MAX_TEST_CODE_ARRAY_SIZE];
			uint32 SerialNumber;
			uint8 GlobalTestNum;
	}TboardConfig;

	TboardConfig BoardConnected;
	uint32 * TestArray937x[6][4];


#endif /* INC_BOARD_CONFIG_H_ */
