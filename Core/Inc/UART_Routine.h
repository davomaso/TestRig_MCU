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
//void UART2_transmit(uns_ch*, uns_ch);	//Transmit function for transmitting out UART2	//Transmit function for transmitting out UART2

//Variables for UART2 Transmit
uns_ch UART2_TXbuffer[LRGBUFFER]; //Buffer for transmitting data to UART2
uint8 UART2_TXpos;				//Position of the array when transmitting
uint8 UART2_TXcount;				//End reference point to stop transmitting, turn of interrupts
//Variables for UART2 Receive
uns_ch UART2_RXbuffer[LRGBUFFER];	//Storage buffer for receiving data, before communications/other routines are called
uint8 UART2_RecPos;				//Positioning for where in the array data is stored to
bool UART2_Recdata;					//flag for whether data can be received
uint8 UART2_Length;			//length of the Receiving array minus the header
bool UART2_ReceiveComplete;			//Flag given once reception of data is complete

typedef enum {RxBAD = 0, RxGOOD = 1, RxWaiting = 2, RxNone = 3}TcommsReceive;
TcommsReceive BoardCommsReceiveState;

//Sleep/Wake function
bool Sleepstate;				//Sleep state for when transmitting data on UART2, Wakes target board with 55 & 00


#endif /* INC_UART_ROUTINE_H_ */
