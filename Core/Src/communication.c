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
//extern TboardConfig TestConfig935x(void);
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

void communication_array(uns_ch Command, uns_ch * Para, uint8_t  Paralen)
{
	UART2_ReceiveComplete = false;
	Length = 14 + Paralen;
//	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
	Comlen = Length + 3;

//Assign Elements to Communications Buffer Array
 Com_buffer[0] = 0xB2;
 Com_buffer[1] = 0x21;
 Com_buffer[2] = Length;
 Com_buffer[3] = NetID;   //LSB first, so that user can easily input network when terminal is implemented
 Com_buffer[4] = (NetID >> 8);
 Com_buffer[5] = '\x01';
 Com_buffer[6] = Module;
 Com_buffer[7] = (Module >> 8);
 for(int i = 8;i <= 13; i++)
 {
	 Com_buffer[i] = '\x00';
 }
 Com_buffer[14] = Command;
 //set para below, not required at this stage
 //probably use switch/case statement to determine the size of parameter with respect to the C/R input
 if(Paralen > 0)
	 memcpy(&Com_buffer[15], Para, Paralen);

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
	 if(BoardConnected.GlobalTestNum < 4){
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

void communication_command(uns_ch * ComRep)
{
	switch (*ComRep)
	{
		case 0x09:
				SetPara(*ComRep);
				communication_array(*ComRep, &Para[0], Paralen);
			break;
		case 0x35:
				//Set this to 0x56 when configuration command is working	//0x1A required to upload samples
				BoardConnected.GlobalTestNum = 0; //Set to one for test count in set v measured
			break;
		case 0x57:
		case 0x27:
				USART6->CR1 |= (USART_CR1_RXNEIE);
			break;
		case 0x1B:
				sprintf(Buffer, "Samples Uploaded\n\n");
				CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
				HAL_UART_Transmit(&huart1, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);
				sprintf(Buffer, "Requesting Results\n\n");
				CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
				HAL_UART_Transmit(&huart1, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);
			break;
		case 0x19:
				SetPara(*ComRep);
			break;
	}
}

void communication_response(uns_ch * Response, uns_ch *data, uint8 arraysize)
{
	unsigned char *ptr;
	//Stop re-entry into communication Routines
	UART2_ReceiveComplete = false;
	if (CRC_Check(data, arraysize)) {	//from here break the Receive Buffer array down
						//Length
					Length = *(data + 2);
					*Response = *(data + 14);
							switch(*Response) {
							case 0x09:
										sprintf(Buffer,"====Interogation Complete====\n\n");
										CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
										HAL_UART_Transmit(&huart1, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);
										ptr = data + 15;
										NetID =	*ptr++;
										NetID += (*ptr++ << 8);
										Module = *ptr++;
										Module += (*ptr++ << 8);

								break;
							case 0x35:
										ptr = data + 15;
										Board = *ptr++;
										Board += (*ptr++ << 8); //LSB board number coming in
										Version = *ptr++; //version currently installed on board
										Flags = *ptr++;		//
										Subclass = *ptr++;//if a 93xx board is connected, what variety is it C, M, X, F...

										ptr = data + (arraysize-3);
										Samplerate[0] = *ptr++;	//Sample rate for period of time between samples
										Samplerate[1] = *ptr++;
										//Print Board Info //Transmit Info To Terminal
										sprintf(Buffer, "=====Board Info=====\n");
										CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
										  HAL_UART_Transmit(&huart1, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);
											//Network
										sprintf(Buffer, "Network :	 %i \n",NetID);
										CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
										  HAL_UART_Transmit(&huart1, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);
											//Module
										sprintf(Buffer, "Module :	 %i \n",Module);
										  HAL_UART_Transmit(&huart1, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);
										CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
											//Board
										sprintf(Buffer, "Board :		 %x \n",Board);
										CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
										  HAL_UART_Transmit(&huart1, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);
											//Version
										sprintf(Buffer, "Version :	 %x \n",Version);
										CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
										  HAL_UART_Transmit(&huart1, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);
								break;
							case 0x57:
									HAL_Delay(250); //Delay for the reset of board, as to stop interference with ADC measurements
									sprintf(Buffer,"===Board Configuration Successful===\n\n");
									CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
									  HAL_UART_Transmit(&huart1, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);
									TestFunction(&BoardConnected);
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
									CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
									HAL_UART_Transmit(&huart1, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);
									break;

							case 0x19:
									sprintf(Buffer,"=====Sampling Complete=====\n\n");
									CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
									HAL_UART_Transmit(&huart1, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);

									sprintf(Buffer, "Starting Test Procedure...\n\n");
									CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
									HAL_UART_Transmit(&huart1, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);
									ptr = data + 15;
									memcpy(&sampleBuffer[0], ptr, (arraysize-17) );
									Decompress_Channels(&sampleBuffer,TportCount); //Returns the ammount of channels sampled
									CompareResults(&BoardConnected,&measuredBuffer[BoardConnected.GlobalTestNum][0], &CHval[BoardConnected.GlobalTestNum][0]);
								break;
							case 0x03:
									samplesUploading = true;
									sampleCount = 0;
									sampleTime = 500;
								break;
							case 0xC1:
									BoardCalibrated = true;
									sprintf(Buffer, "=====     Board Calibrated     =====\n");
									HAL_UART_Transmit(&huart1, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);
								break;
							case 0xCD:
									sprintf(Buffer, "\n=====     Board Initialised     =====\n");
									HAL_UART_Transmit(&huart1, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);
								break;
//							case 0x11:
//								ptr = data + 12;
//								Board = (*ptr++ << 8);
//								Board += *ptr++; //LSB board number coming in
//								Version = *ptr++; //version currently installed on board
//								Subclass = *ptr++;//if a 93xx board is connected, what variety is it C, M, X, F...
//
//								//Print Board Info //Transmit Info To Terminal
//								sprintf(Buffer, "=====Board Info=====\n");
//								CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
//								  HAL_UART_Transmit(&huart1, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);
//									//Board
//								sprintf(Buffer, "Board :		 %x \n",Board);
//								CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
//								  HAL_UART_Transmit(&huart1, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);
//									//Version
//								sprintf(Buffer, "Version :	 %x \n",Version);
//								CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
//								  HAL_UART_Transmit(&huart1, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);
//								break;
//							case 0x12:
//								break;
				}
			} else {
			sprintf(Buffer, "CRC ERROR...\n\n");
			CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
			HAL_UART_Transmit(&huart1, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);
			UART2_RecPos = 0;
			UART2_Length = 0;
			UART2_ReceiveComplete = false;
		}
}

_Bool CRC_Check(uns_ch *input, uint8 length)
{
		Crc_response = uart_CalcCRC16(input,length-2);
		Receive_buffer[0] = Crc_response;
		Receive_buffer[1] = (Crc_response >> 8);
		input += (length-2);
		if(Receive_buffer[0] == *input++ && Receive_buffer[1] == *input)
			return true;
		else
			return false;
}

void SetPara(uns_ch Command)
{
			//Set parameters 00 - 0F sample inputs, command
		switch (Command)
		{
			case 0x08:
				Paralen = 0;
				break;
			case 0x34:
				sprintf(Buffer, "Getting Board Info...\n");
				CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
				HAL_UART_Transmit(&huart1, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);
				BoardConnected.GlobalTestNum = 0;
				break;

			case 0x18:
				for	(uint8 i = 0; i <= 15; i++)	{
						Para[i] = i;
						Paralen = i+1;
					}
				break;
			case 0x1A:
				for (uint8 i = 0; i <= 15; i++) {
						Para[i] = i;
						Paralen = i+1;
					}
				sprintf(Buffer,"Target Board Uploading Samples...\n");
				CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
				HAL_UART_Transmit(&D_UART, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);
				break;
			case 0x56:
						sprintf(Buffer, "\nConfiguring Board...\n");
//						CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
						HAL_UART_Transmit(&huart1, &Buffer[0], strlen(Buffer), HAL_MAX_DELAY);

						SetTestParam(&BoardConnected, BoardConnected.GlobalTestNum, &Para[0], &Paralen);
						TportCount = BoardConnected.latchPortCount + BoardConnected.analogInputCount + BoardConnected.digitalInputCout;
						communication_array(Command, &Para[0], Paralen);
					 	LCD_ClearLine(1);
						LCD_setCursor(1, 0);
						sprintf(Buffer,"%d  S/N: %d  ", BoardConnected.BoardType, BoardConnected.SerialNumber);
						LCD_printf(&Buffer[0], strlen(Buffer));
				break;
			case 0xCC:
					Para[0] = 0x49;
					Paralen = 1;
				break;
			case 0xC0:
					Para[0] = 0x50;
					Paralen = 1;
				break;
		}
}


//void interrogateBoard() {
//	uns_ch ComRep;
//	ComRep = 0x08;
//	communication_array(ComRep,&Para[0], Paralen);
//	while(!UART2_ReceiveComplete) {
//
//	}
//	if (UART2_ReceiveComplete)
//		communication_response(&ComRep,&UART2_Receive[0], UART2_RecPos);
//	while(!Comm_Ready){
//
//	}
//	if (Comm_Ready)
//		communication_command(&ComRep);
//	while(!UART2_ReceiveComplete) {
//
//	}
//	if (UART2_ReceiveComplete)
//		communication_response(&ComRep,&UART2_Receive[0], UART2_RecPos);
//}

void communication_arraySerial(uns_ch Command,uint32 CurrentSerial , uint32 NewSerial)
{
	UART2_ReceiveComplete = false;
	uint8 Length = NewSerial ? 11:7;
	uint8 Comlen = Length + 3;
//Assign Elements to Communications Buffer Array
	Com_buffer[0] = 0xB2;
	Com_buffer[1] = 0x0F;
	Com_buffer[2] = Length;
	memcpy(&Com_buffer[3], &CurrentSerial, 4);
	Com_buffer[7] = Command;

	if (NewSerial > 0)
		memcpy(&Com_buffer[8], &NewSerial, 4);

	Crc = uart_CalcCRC16(&Com_buffer[0], (Comlen-2));
	Com_buffer[Comlen-2] = Crc;
	Com_buffer[Comlen-1] = (Crc >> 8);

 	 	 //Switch Comms to Radio or RS485 depending on Board Connected

	 UART2_transmit(&Com_buffer[0], Comlen);
	 USART2->CR1  |= USART_CR1_RXNEIE;
}

