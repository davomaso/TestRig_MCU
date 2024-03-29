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
#include "Init.h"
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

/*
 * Communication routine that sets the buffer to be transmitted to the target board, implementing the parameters set by SetPara and
 * puts the CRC at the end of the string from uart_CRC.
 */
void communication_array(uns_ch Command, uns_ch *Para, uint8_t Paralen) {
	uint8 Comlen;
	uns_ch Com_buffer[LRGBUFFER];
	uint16 Length;
	uint16 Crc;
	UART2_ReceiveComplete = false;							// Disable the receive complete flag prior to the string being transmitted
	Length = 14 + Paralen;
	Comlen = Length + 3;									// Calculate the Length parameter

	Com_buffer[0] = 0xB2;									// Populate header with network, length, comms direction & module
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
	Com_buffer[14] = Command;								// Populate communication buffer with command

	if (Paralen > 0)
		memcpy(&Com_buffer[15], Para, Paralen);				// Copy parameters into the communications string

	Crc = uart_CalcCRC16(&Com_buffer[0], (Length + 1));		// Calculate CRC
	Com_buffer[Comlen - 2] = Crc;							// Populate CRC at end of communication buffer
	Com_buffer[Comlen - 1] = (Crc >> 8);

	switchCommsProtocol(&BoardConnected);					// Switch Comms to Radio or RS485 depending on Board Connected
	UART2_transmit((uns_ch*) &Com_buffer[0], Comlen);		// Transmit the Communication array
	USART2->CR1 |= USART_CR1_RXNEIE;						// UART2 Receive Interrupt Enable.
}


/*
 * Handle the response from the board, pass the data returned in the communication string to be handled
 */
void communication_response(TboardConfig *Board, uns_ch *Response, uns_ch *data, uint8 arraysize) {
	switch (*Response) {
	case 0x11:												//Serialise Command, Used to check the version of the board
		memcpy(&Board->SerialNumber, data, 4);
		data += 4;
		if ((Board->SerialNumber != 0) && (~(Board->SerialNumber) != 0))	// Set the serialised flag if a valid serial number is returned
			SET_BIT(Board->BSR, BOARD_SERIALISED);
		data += 2;  //LSB board number coming in
		Board->Version = *data++; 											//version currently installed on board
		Board->Subclass = *data++;											//if a 93xx board is connected, what variety is it C, M, X, F...
		memcpy(&Board->Network, data, 2);									//copy 2 bytes of network address
		data += 2;
		memcpy(&Board->Module, data, 2);									//copy 2 bytes of module address
		if (CurrentState != csIDLE) {										// print the state of the state machine if the system is not in idle
			printT((uns_ch*) "====Interogation Complete====\n");
			printT((uns_ch*) "=====Board Info=====\n");					//Print Board Info //Transmit Info To Terminal
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
	case 0x1B:		// Handle the sampling request return
		if (SDIenabled || RS485enabled) {	// When testing includes RS485 or SDI-12 extend period in which sampling occurs
			sampleTime = *data++;
			sampleTime |= (*data++ << 8);
			sampleTime *= 100;
			sampleTime = (sampleTime > 2000) ? 2000 : sampleTime;
		} else
			sampleTime = Board->analogInputCount * 100;					// Multiple of 100ms per port for sample
		sampleCount = 0;
		samplesUploaded = false;
		samplesUploading = true;
		if (TestRigMode == VerboseMode) {								// Print wait time if the system is in verbose mode
			sprintf((char*) &debugTransmitBuffer, "Waiting : %.2f seconds....\n", (float) sampleTime / 1000);
			printT((uns_ch*) &debugTransmitBuffer);
		}
		break;

	case 0x19:		// Sampling complete, uploading results response
		if (TestRigMode == VerboseMode) {
			printT((uns_ch*) "=====Sampling Complete=====\n");
			printT((uns_ch*) "Starting Test Procedure...\n\n");
		}
		memcpy(&sampleBuffer[0], data, (arraysize));		// copy the data from the returned array to the Sample array
		break;

	case 0x03:	//Board Busy
		setTimeOut(1000);	// restart timeout
		break;
	case 0xCD:	// Initialise board command
		if (TestRigMode == VerboseMode)
			printT((uns_ch*) "\n=====     Board Initialised     =====\n");
		break;

	}
}
/*
 * Boolean response to determine whether the CRC check on the returned string was valid. Return true if a valid CRC was found
 */
_Bool CRC_Check(uns_ch *input, uint8 length) {// Basic CRC check to determine whether the string received is correct, also add the CRC at the end of the string prior to transmission
	uint16 Crc_response;
	Crc_response = uart_CalcCRC16(input, length - 2);
	input += (length - 2);
	if (((Crc_response & 0xFF) == *input++) && ((Crc_response >> 8) == *input))
		return true;
	else
		return false;
}

/*
 * Set Communication Parameters Routine
 * Dependant on the command being transmitted to the target device a different parameter is required to be sent
 * This function will determine what is sent with the command passed to it
 */
void SetPara(TboardConfig *Board, uns_ch Command) {

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
			printT((uns_ch *)"\nConfiguring Board...\n");
		SetTestParam(Board, Board->GlobalTestNum, &BoardCommsParameters[0], &BoardCommsParametersLength);
		break;
	}
}

/*
 * Rountine to set and read Serial number of the board
 * If no currentserial number is passed, the routine will leave the 4 bytes of data out
 * else a new serial number is being programmed to the board by which both serial numbers
 * are required
 */
void communication_arraySerial(uns_ch Command, uint32 CurrentSerial, uint32 NewSerial) {
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
	HAL_Delay(50);

	UART2_transmit(&Com_buffer[0], Comlen);
	USART2->CR1 |= USART_CR1_RXNEIE;
}

/*
 * Switch board comms protocol depending on board and test being run, some boards are constant radio/RS485 others have both
 */
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
