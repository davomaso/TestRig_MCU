/*
 * UART_Routine.h
 *
 *  Created on: Apr 14, 2020
 *      Author: David Mason
 */

#ifndef INC_UART_ROUTINE_H_
#define INC_UART_ROUTINE_H_

//Includes
//#include "interogate_project.h"
#include "stdbool.h"

//Defines
#define LRGBUFFER 254
#define SMLBUFFER 32
#define XSMLBUFFER 16

//Routines
void UART2_transmit(uns_ch*, uns_ch);	//Transmit function for transmitting out UART2
void UART3_transmit(uns_ch*, uns_ch);	//Transmit function for transmitting out UART2

//Variables for UART2 Transmit
uns_ch UART2_TXbuffer[LRGBUFFER]; //Buffer for transmitting data to UART2
uint8 UART2_TXpos;				//Position of the array when transmitting
uint8 UART2_TXend;				//End reference point to stop transmitting, turn of interrupts
//Variables for UART2 Receive
uns_ch UART2_Receive[LRGBUFFER];	//Storage buffer for receiving data, before communications/other routines are called
uint16 UART2_RecPos;				//Positioning for where in the array data is stored to
bool UART2_Recdata;					//flag for whether data can be received
uns_ch UART2_Length[1];			//length of the Receiving array minus the header
bool UART2_ReceiveComplete;			//Flag given once reception of data is complete

//Sleep/Wake function
bool Sleepstate;				//Sleep state for when transmitting data on UART2, Wakes target board with 55 & 00


#endif /* INC_UART_ROUTINE_H_ */
