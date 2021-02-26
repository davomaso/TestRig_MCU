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
			uint8_t outputPortCount;
			uint8_t analogInputCount;//change all channels to ports'
			uint8_t digitalInputCout;
			uint8_t testNum;
 			uint8_t paramNum;
			TportConfig *TestArray;
			uint8_t *PortCodes;
			uint32 SerialNumber;
	}TboardConfig;

	TboardConfig BoardConnected;

#endif /* INC_BOARD_CONFIG_H_ */
