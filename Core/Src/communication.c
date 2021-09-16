// Source File Description
/*
 *  communication_array :- Takes command and parameters to be sent to the board and prepares the string to be transmit out UART2
 *
 *  communication_command :-
 *
 *
 */

//INCLUDES
#include "main.h"
#include "Global_Variables.h"
#include "Communication.h"

#include "CRC16_Slow.h"
#include "UART_Routine.h"
#include "Board_Config.h"
#include "Test.h"
#include "Delay.h"
#include "LCD.h"
#include "stdio.h"
#include "string.h"

//VARIABLE DEFINITIONS
//B2 21 NOTIFIES the boards that data is being transmitted
//following the B2 21, the next byte signifies the length of the array
//Length: 				1 byte
//NET ID: 				2 bytes
//Dir/direction: 		1 byte
//Module:				2 bytes + 2 bytes
//Adress term:			2 bytes
//Misc:					2 bytes
//Com/Response:			1 byte
//Parameters:			variable
//CRC:					2 bytes

void communication_array(uns_ch Command, uns_ch *Para, uint8_t Paralen) {
	/*
	 * Communication routine that sets the buffer to be transmitted to the target board, implementing the parameters set by SetPara and
	 * puts the CRC at the end of the string from uart_CRC.
	 *
	 */
	uint8 Comlen;
	uns_ch Com_buffer[LRGBUFFER];
	uint16 Length;
	uint16 Crc;
	UART2_ReceiveComplete = false;
	Length = 14 + Paralen;
	Comlen = Length + 3;

	Com_buffer[0] = 0xB2;
	Com_buffer[1] = 0x21;
	Com_buffer[2] = Length;
	Com_buffer[3] = BoardConnected.Network;
	Com_buffer[4] = (BoardConnected.Network >> 8);
	Com_buffer[5] = '\x01';
	Com_buffer[6] = BoardConnected.Module;
	Com_buffer[7] = (BoardConnected.Module >> 8);
	for (int i = 8; i <= 13; i++) {
		Com_buffer[i] = '\x00';
	}
	Com_buffer[14] = Command;

	if (Paralen > 0)
		memcpy(&Com_buffer[15], Para, Paralen);

	Crc = uart_CalcCRC16(&Com_buffer[0], (Length + 1));
	Com_buffer[Comlen - 2] = Crc;
	Com_buffer[Comlen - 1] = (Crc >> 8);

	//Switch Comms to Radio or RS485 depending on Board Connected
	switchCommsProtocol(&BoardConnected);

	//Transmit the Communication array
	UART2_transmit((uns_ch*) &Com_buffer[0], Comlen);
	// UART2 Receive Interrupt Enable.
	USART2->CR1 |= USART_CR1_RXNEIE;
}

void communication_response(TboardConfig *Board, uns_ch *Response, uns_ch *data, uint8 arraysize) {
	switch (*Response) {
	case 0x11:	//Serialise Command, Used to check the version of the board
		memcpy(&Board->SerialNumber, data, 4);
		data += 4;
		if ((Board->SerialNumber != 0) && (~(Board->SerialNumber) != 0))
			SET_BIT(Board->BSR, BOARD_SERIALISED);
		data += 2;  //LSB board number coming in
		Board->Version = *data++; //version currently installed on board
		Board->Subclass = *data++; //if a 93xx board is connected, what variety is it C, M, X, F...

		memcpy(&Board->Network, data, 2);
		data += 2;
		memcpy(&Board->Module, data, 2);
		if (CurrentState != csIDLE) {
			printT((uns_ch*) "====Interogation Complete====\n");
				//Print Board Info //Transmit Info To Terminal
			printT((uns_ch*) "=====Board Info=====\n");
				//Board
			sprintf((char*) &debugTransmitBuffer[0], "Board :		 %x%c \n", Board->BoardType, Board->Subclass);
			printT(&debugTransmitBuffer[0]);
				//Version
			sprintf((char*) &debugTransmitBuffer[0], "Version :	 %x \n", Board->Version);
			printT(&debugTransmitBuffer[0]);
				//Network
			sprintf((char*) &debugTransmitBuffer[0], "Network :	 %d \n", Board->Network);
			printT(&debugTransmitBuffer[0]);
				//Module
			sprintf((char*) &debugTransmitBuffer[0], "Module :	 %d \n", Board->Module);
			printT(&debugTransmitBuffer[0]);

		}
		break;

	case 0x1B:
		sampleTime = *data++;
		sampleTime |= (*data++ << 8);
		sampleTime *= 100;
		sampleCount = 0;
		samplesUploaded = false;
		sampleTime = sampleTime > 3500 ? 3500 : sampleTime;
		samplesUploading = true;
		//Uploading begun
		if (TestRigMode == VerboseMode) {
			sprintf((char*) &debugTransmitBuffer, "Waiting : %.2f seconds....\n", (float) sampleTime / 1000);
			printT((uns_ch*) &debugTransmitBuffer);
			}
		break;

	case 0x19:
		if (TestRigMode == VerboseMode) {
			printT((uns_ch*) "=====Sampling Complete=====\n");
			printT((uns_ch*) "Starting Test Procedure...\n\n");
		}
		memcpy(&sampleBuffer[0], data, (arraysize));
		break;
	case 0x03:	//Board Busy
		samplesUploading = true;
		samplesUploaded = false;
		sampleCount = 0;
		sampleTime = 500;
		break;
	case 0xCD:	// Initialise board command
		if (TestRigMode == VerboseMode)
			printT((uns_ch*) "\n=====     Board Initialised     =====\n");
		break;

	}
}

_Bool CRC_Check(uns_ch *input, uint8 length) {// Basic CRC check to determine whether the string received is correct, also add the CRC at the end of the string prior to transmission
	uint16 Crc_response;
	Crc_response = uart_CalcCRC16(input, length - 2);
	input += (length - 2);
	if (((Crc_response & 0xFF) == *input++) && ((Crc_response >> 8) == *input))
		return true;
	else
		return false;
}

void SetPara(TboardConfig *Board, uns_ch Command) {
	/*
	 * Set Communication Parameters Routine
	 * Dependant on the command being transmitted to the target device a different parameter is required to be sent
	 * This function will determine what is sent with the command passed to it
	 */
	//Set parameters 00 - 0F sample inputs, command
	switch (Command) {
	case 0x18:
		for (uint8 i = 0; i <= 15; i++) {
			BoardCommsParameters[i] = i;
		}
		BoardCommsParametersLength = 16;
		break;
	case 0x1A:
		for (uint8 i = 0; i <= 15; i++) {
			BoardCommsParameters[i] = i;
		}
		BoardCommsParametersLength = 16;
		if (TestRigMode == VerboseMode)
			printT((uns_ch*) "Target Board Uploading Samples...\n");
		break;
	case 0x56:
		if (TestRigMode == VerboseMode)
			printT("\nConfiguring Board...\n");
		SetTestParam(Board, Board->GlobalTestNum, &BoardCommsParameters[0], &BoardCommsParametersLength);
		if (BoardConnected.GlobalTestNum == 0) {
			if (Board->Subclass)
				sprintf((char*) &lcdBuffer[0], "%x%c   S/N: %lu", Board->BoardType, Board->Subclass,
						Board->SerialNumber);
			else
				sprintf((char*) &lcdBuffer[0], "%x   S/N: %lu", Board->BoardType, Board->SerialNumber);
			LCD_printf((uns_ch*) &lcdBuffer[0], 1, 0);
		}
		break;
	}
}

void communication_arraySerial(uns_ch Command, uint32 CurrentSerial, uint32 NewSerial) {
	/*
	 * Rountine to set and read Serial number of the board
	 * If no currentserial number is passed, the routine will leave the 4 bytes of data out
	 * else a new serial number is being programmed to the board by which both serial numbers
	 * are required
	 */
	uint8 Comlen;
	uns_ch Com_buffer[LRGBUFFER];
	UART2_ReceiveComplete = false;
	uint8 Length;
	Length = NewSerial ? 11 : 7;
	Comlen = Length + 3;
	uint16 Crc;
//Assign Elements to Communications Buffer Array
	Com_buffer[0] = 0xB2;
	Com_buffer[1] = 0x0F;
	Com_buffer[2] = Length;
	memcpy(&Com_buffer[3], &CurrentSerial, 4);
	Com_buffer[7] = Command;

	if (NewSerial > 0)
		memcpy(&Com_buffer[8], &NewSerial, 4);

	Crc = uart_CalcCRC16((uns_ch*) &Com_buffer[0], (Comlen - 2));
	Com_buffer[Comlen - 2] = Crc;
	Com_buffer[Comlen - 1] = (Crc >> 8);

	//Switch Comms to Radio or RS485 depending on Board Connected
	switchCommsProtocol(&BoardConnected);

	UART2_transmit(&Com_buffer[0], Comlen);
	USART2->CR1 |= USART_CR1_RXNEIE;
}

void switchCommsProtocol(TboardConfig *Board) {
	if ((Board->BoardType == b935x) || (Board->BoardType == b937x)) {
		HAL_GPIO_WritePin(MUX_A0_GPIO_Port, MUX_A0_Pin, GPIO_PIN_RESET);
	} else if ((Board->BoardType == b401x) || (Board->BoardType == b422x)) {
		USART2->CR1 &= ~(USART_CR1_RE);
		HAL_GPIO_WritePin(MUX_A0_GPIO_Port, MUX_A0_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(RS485_EN_GPIO_Port, RS485_EN_Pin, GPIO_PIN_SET);
	} else if ((Board->BoardType == b402x) || (Board->BoardType == b427x)) {
		if (Board->GlobalTestNum > 2) {
			HAL_GPIO_WritePin(MUX_A0_GPIO_Port, MUX_A0_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(Radio_EN_GPIO_Port, Radio_EN_Pin, GPIO_PIN_RESET);
		} else {
			USART2->CR1 &= ~(USART_CR1_RE);
			HAL_GPIO_WritePin(MUX_A0_GPIO_Port, MUX_A0_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(RS485_EN_GPIO_Port, RS485_EN_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(Radio_EN_GPIO_Port, Radio_EN_Pin, GPIO_PIN_SET);
		}
	}
}
