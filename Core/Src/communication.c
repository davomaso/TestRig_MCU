/*
 * communication.c
 *
 *  Created on: Apr 12, 2020
 *      Author: David Mason
 */

//This routine will handle all communication protocol associated with sending command/receiving commands to/from the target board

//INCLUDES
#include "main.h"
#include "stm32f4xx_it.h"
#include "interogate_project.h"
#include "UART_Routine.h"
#include "Board_Config.h"
#include "Test.h"
#include "Channel_Analysis.h"

unsigned short uart_CalcCRC16(unsigned char*, unsigned char);
_Bool CRC_Check(unsigned char*, unsigned char);
void RTC_Set(unsigned char*,unsigned char);

extern unsigned char OneByte_HexToAsc(unsigned char *, unsigned char);
//extern unsigned char Decompress_Channels(unsigned char *, unsigned char);
	//Test.c Functions
extern TboardConfig TestConfig935x(void);
extern void ConfigInit(void);

extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart6;
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim14;

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

//result:				1 byte for the storage of hexadecimal characters

void communication_array(unsigned char * ComRep,unsigned char* Para, uint8_t  Paralen)
{
	UART2_ReceiveComplete = false;
	Length = 14 + Paralen;
//	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
	Comlen = Length + 3;

//Assign Elements to Communications Buffer Array
 Com_buffer[0] = 0xB2;
 Com_buffer[1] = 0x21;
 Com_buffer[2] = Length;
 Com_buffer[3] = NetID[0];   //LSB first, so that user can easily input network when terminal is implemented
 Com_buffer[4] = NetID[1];
 Com_buffer[5] = '\x01';
 Com_buffer[6] = Module[0];
 Com_buffer[7] = Module[1];
 for(int i = 8;i <= 13; i++)
 {
	 Com_buffer[i] = '\x00';
 }
 Com_buffer[14] = *ComRep;
 //set para below, not required at this stage
 //probably use switch/case statement to determine the size of parameter with respect to the C/R input
 if(Paralen > 0)
 {
	 //can use length because parameters stop 3 bits before the end of the array eg. before CRC and C/R
	 for(unsigned char i = 15; i < (15 + Paralen); i++)
	 {
		 Com_buffer[i] = *Para++;
	 }
 }
 //use the CRC calculator to propagate a CRC to add to the end of the Communications buffer
 //CRC is calculated from the beginning of the header to the start of the CRC hence only adding 1
 Crc = uart_CalcCRC16(&Com_buffer[0], (Length + 1));
 Com_buffer[Comlen-2] = Crc;
 Com_buffer[Comlen-1] = (Crc >> 8);

 	 //Switch Comms to Radio or RS485 depending on Board Connected
 if((LoomConnected == b935x) || (LoomConnected == b937x)) {
	 HAL_GPIO_WritePin(MUX_A0_GPIO_Port, MUX_A0_Pin, GPIO_PIN_RESET);
 } else if((LoomConnected == b401x) || (LoomConnected == b422x)) {
	 USART2->CR1 &= ~(USART_CR1_RE);
	 HAL_GPIO_WritePin(MUX_A0_GPIO_Port, MUX_A0_Pin, GPIO_PIN_SET);
	 HAL_GPIO_WritePin(RS485_EN_GPIO_Port, RS485_EN_Pin, GPIO_PIN_SET);
 } else if((LoomConnected == b402x) || (LoomConnected == b427x)) {
	 if(GlobalTestNum < 4){
		 HAL_GPIO_WritePin(MUX_A0_GPIO_Port, MUX_A0_Pin, GPIO_PIN_RESET);
	 } else {
		 USART2->CR1 &= ~(USART_CR1_RE);
		 HAL_GPIO_WritePin(MUX_A0_GPIO_Port, MUX_A0_Pin, GPIO_PIN_SET);
		 HAL_GPIO_WritePin(RS485_EN_GPIO_Port, RS485_EN_Pin, GPIO_PIN_SET);
	 }
 }

 //Transmit the Communication array
 UART2_transmit(&Com_buffer[0], Comlen);
 // UART2 Receive Interrupt Enable.
 USART2->CR1  |= USART_CR1_RXNEIE;
 }

void communication_command()
{
	switch (ComRep)
	{
		case 0x09:
				ComRep = 0x34;
				SetPara();
				communication_array(&ComRep, &Para[0], Paralen);
			break;
		case 0x35:
				ComRep = 0x56;	//Set this to 0x56 when configuration command is working	//0x1A required to upload samples
				ConfigInit();	//This are required for the 0x56 command
				GlobalTestNum = 0; //Set to one for test count in set v measured
				SetPara();
			break;
		case 0x57:
		case 0x27:
				Comm_Ready = false;
				ComRep = 0x1A;
				SetPara();
				communication_array(&ComRep, &Para[0], Paralen);
				USART6->CR1 |= (USART_CR1_RXNEIE);
			break;
		case 0x1B:
				sprintf(Buffer, "Samples Uploaded\n\n");
				HAL_UART_Transmit(&T_UART, &Buffer[0], strlen(Buffer), 100);
				sampleUploadComplete = false;
				ComRep = 0x18;
				SetPara();
				communication_array(&ComRep, &Para[0], Paralen);
				sprintf(Buffer, "Requesting Results\n\n");
				HAL_UART_Transmit(&T_UART, &Buffer[0], strlen(Buffer), 100);
			break;
		case 0x19:
				Comm_Ready = false;
				ComRep = 0x56;
				SetPara();
			break;
	}
}

void communication_response(unsigned char *data, unsigned char arraysize)
{
	unsigned char *ptr;
	//Stop re-entry into communication Routines
	UART2_ReceiveComplete = false;
	if(CRC_Check(data, arraysize))
			{
				//from here break the Receive Buffer array down
					//Length
					Length = *(data + 2);
					//	C/R will determine how the system will behave following
					ComRep = *(data + 14);
							switch(ComRep)
								{
							case 0x09:
										sprintf(Buffer,"====Interogation Complete====\n\n");
										HAL_UART_Transmit(&T_UART,&Buffer[0],strlen(Buffer), 100);
										ptr = data + 15;
										NetID[0] =	*ptr++;
										NetID[1] =	*ptr++;
										Module[0] = *ptr++;
										Module[1] = *ptr++;
										Comm_Ready = true;

								break;
							case 0x35:
										ptr = data + 15;
										Board[1] = *ptr++;
										Board[0] = *ptr++; //LSB board number coming in
										Version[0] = *ptr++; //version currently installed on board
										Flags = *ptr++;		//
										Subclass[0] = *ptr++;//if a 93xx board is connected, what variety is it C, M, X, F...

										ptr = data + (arraysize-3);
										Samplerate[0] = *ptr++;	//Sample rate for period of time between samples
										Samplerate[1] = *ptr++;
										//Print Board Info //Transmit Info To Terminal
										sprintf(Buffer, "=====Board Info=====\n");
										HAL_UART_Transmit(&T_UART, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);
											//Network
										intBuffer = (NetID[1] << 8) + NetID[0];
										sprintf(Buffer, "Network :	 %i \n",intBuffer);
										HAL_UART_Transmit(&T_UART, &Buffer[0], strlen(Buffer), 50);
											//Module
										intBuffer = (Module[1] << 8) + Module[0];
										sprintf(Buffer, "Module :	 %i \n",intBuffer);
										HAL_UART_Transmit(&T_UART, &Buffer[0], strlen(Buffer), 50);
											//Board
										intBuffer = (Board[0] << 8) + Board[1];
										sprintf(Buffer, "Board :		 %x \n",intBuffer);
										HAL_UART_Transmit(&T_UART, &Buffer[0], strlen(Buffer), 50);
											//Version
										intBuffer = Version[0];
										sprintf(Buffer, "Version :	 %x \n",intBuffer);
										HAL_UART_Transmit(&T_UART, &Buffer[0], strlen(Buffer), 50);

//										LCD_setCursor(1, 0);
//										sprintf(Buffer,"      S/N: %d  ", BoardConnected.SerialNumber);
//										LCD_printf(&Buffer[0], strlen(Buffer));

										Comm_Ready = true;
								break;
							case 0x57:
									HAL_Delay(250); //Delay for the reset of board, as to stop interference with ADC measurements
									sprintf(Buffer,"===Board Configuration Successful===\n\n");
									HAL_UART_Transmit(&T_UART, &Buffer[0], strlen(Buffer), 100);
									TestFunction(&TestCode, BoardConnected.outputPortCount, BoardConnected.analogInputCount, BoardConnected.digitalInputCout);
									if(LatchPort1)
										runLatchTest(0);
									if(LatchPort2)
										runLatchTest(1);
									if(LatchPort3)
										runLatchTest(2);
									if(LatchPort4)
										runLatchTest(3);
									AsyncComplete = false;
									Async_Port1.Active = Async_Port1.PulseCount ? true:false;
									Async_Port2.Active = Async_Port2.PulseCount ? true:false;
									Async_Port3.Active = Async_Port3.PulseCount ? true:false;
									Async_Port4.Active = Async_Port4.PulseCount ? true:false;
									Async_Port5.Active = Async_Port5.PulseCount ? true:false;
									Async_Port6.Active = Async_Port6.PulseCount ? true:false;
									Async_Port7.Active = Async_Port7.PulseCount ? true:false;
									Async_Port8.Active = Async_Port8.PulseCount ? true:false;
									Async_Port9.Active = Async_Port9.PulseCount ? true:false;
									while(!AsyncComplete){
									}
									Comm_Ready = true;
									break;

							case 0x1B:
									ptr = data +15;
									sampleTime = *ptr++;
									sampleTime |= (*ptr++ << 8);
									sampleTime *= 10;
									sampleCount = 0;
									samplesUploading = true;
									sampleTime = sampleTime > 100 ? 100 : sampleTime;
									//Uploading begun
									sprintf(Buffer,"Waitng : %.2f seconds....\n", (float)sampleTime/100 );
									HAL_UART_Transmit(&T_UART, &Buffer[0], strlen(Buffer), 100);
									break;

							case 0x19:

									sprintf(Buffer,"=====Sampling Complete=====\n\n");
									HAL_UART_Transmit(&T_UART, &Buffer[0], strlen(Buffer), 100);

									sprintf(Buffer, "Starting Test Procedure...\n\n");
									HAL_UART_Transmit(&T_UART, &Buffer[0], strlen(Buffer), 100);
									ptr = data + 15;
									unsigned int sampleBufferLength = 0;
									for(sampleBufferLength = 0;sampleBufferLength < (arraysize - 17);sampleBufferLength++)
										sampleBuffer[sampleBufferLength] = *ptr++;
									Decompress_Channels(&sampleBuffer[0],TportCount); //Returns the ammount of channels sampled
									CompareResults(&measuredBuffer[GlobalTestNum][1], &CHval[GlobalTestNum][0], TportCount);
									Comm_Ready = true;
								break;
							case 0x03:
									samplesUploading = true;
									sampleCount = 0;
									sampleTime = 500;
								break;
				}
			}
	else
		{
			sprintf(Buffer, "CRC ERROR...\n\n");
			HAL_UART_Transmit(&T_UART, &Buffer[0], strlen(Buffer), 100);
			UART2_RecPos = 0;
			UART2_Length[0] = 0;
			UART2_ReceiveComplete = false;
		}
	//depending on the command/response received will determine whether another C/R is required
	//given a s08 r09, a s34 r35 command should be used if no data is lost, else the s08 command should be run again

}

_Bool CRC_Check(unsigned char *input, unsigned char length)
{
		Crc_response = uart_CalcCRC16(input,length-2);
		Receive_buffer[0] = Crc_response;
		Receive_buffer[1] = (Crc_response >> 8);
		if(Receive_buffer[0] == input[length-2] && Receive_buffer[1] == input[length-1])
		{
			return true;
		}
		else return false;
}

void SetPara()
{
			//Set parameters 00 - 0F sample inputs, command
		switch (ComRep)
		{
			case 0x34:
				sprintf(Buffer, "Getting Board Info...\n");
				HAL_UART_Transmit(&T_UART,&Buffer[0],strlen(Buffer), 100);
				GlobalTestNum = 0;
				break;

			case 0x18:
				for(unsigned char i = 0; i <= 0x0F; i++)
					{
						Para[i] = i;
						Paralen = i+1;
					}
				break;
			case 0x1A:
				for(unsigned char i = 0; i <= 0x0F; i++)
					{
						Para[i] = i;
						Paralen = i+1;
					}
				sprintf(Buffer,"Target Board Uploading Samples...\n");
				HAL_UART_Transmit(&T_UART, &Buffer[0], strlen(Buffer), 100);
				break;
			case 0x56:
				{
						sprintf(Buffer, "\nConfiguring Board...\n");
						HAL_UART_Transmit(&T_UART,&Buffer[0],strlen(Buffer), 100);
						uint32 tempSN = BoardConnected.SerialNumber;

						if(Board[0] == 0x93)
						{
							 if( ((Board[1] & 0xF0) == 0x50 )  && (LoomConnected == b935x) ){
								 	BoardConnected = TestConfig935x();
									TportCount = BoardConnected.outputPortCount + BoardConnected.analogInputCount + BoardConnected.digitalInputCout;
							 } else if( ((Board[1] & 0xF0) == 0x70) && (LoomConnected == b937x) ){
									BoardConnected = TestConfig937x();
									TportCount = BoardConnected.outputPortCount + BoardConnected.analogInputCount + BoardConnected.digitalInputCout;
							 }
						}
						else if(Board[0] == 0x40)
						{
							if( ((Board[1] & 0xF0) == 0x10) && (LoomConnected == b401x) ){
								BoardConnected = TestConfig401x();
								TportCount = BoardConnected.outputPortCount + BoardConnected.analogInputCount + BoardConnected.digitalInputCout;
							} else if ( ((Board[1] & 0xF0) == 0x20) && (LoomConnected == b402x)){
								BoardConnected = TestConfig402x();
								TportCount = BoardConnected.outputPortCount + BoardConnected.analogInputCount + BoardConnected.digitalInputCout;
							}
						} else if (Board[0] == 0x42) {
							if( ((Board[1] & 0xF0) == 0x20) && (LoomConnected == b422x)){
								BoardConnected = TestConfig422x();
								TportCount = BoardConnected.outputPortCount + BoardConnected.analogInputCount + BoardConnected.digitalInputCout;
							} else if ( ((Board[1] & 0xF0) == 0x70) && (LoomConnected == b427x)) {
								BoardConnected = TestConfig427x();
								TportCount = BoardConnected.outputPortCount + BoardConnected.analogInputCount + BoardConnected.digitalInputCout;
							}
						} else{
							sprintf(Buffer, "BoardConfig Error/Loom Connected Error");
							HAL_UART_Transmit(&T_UART, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);
						}
					 	BoardConnected.SerialNumber = tempSN;
					 	LCD_ClearLine(1);
						LCD_setCursor(1, 0);
						sprintf(Buffer,"%d  S/N: %d  ", BoardConnected.BoardType, BoardConnected.SerialNumber);
						LCD_printf(&Buffer[0], strlen(Buffer));
				break;
				}
		}
}


