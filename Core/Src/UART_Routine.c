#include "main.h"
#include "Global_Variables.h"
#include "stm32f4xx_it.h"
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"
#include "UART_Routine.h"
#include "Delay.h"
#include "Init.h"


/*
 * UART2 Transmit Routine for board comms
 * While in the sleepstate 0x55 will be transmitted twice and 0x00 transmitted twice to wake the target board
 * following this the data pointer will be loaded into the TXbuffer for the size specified.
 * Once Complete the transmit interrupt is enabled and the data begun to be transmitted out the UART port.
 */
void UART2_transmit(unsigned char *data, unsigned char arraysize) {
	const uint8 WakeBuffer[20] = { 0x55, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	USART2->CR1 &= ~(USART_CR1_TXEIE);															// Disable transmit interupt enable and transmit complete registers
	USART2->CR1 &= ~(USART_CR1_TCIE);															// Disable transmit interrupt

	memcpy(&UART2_TXbuffer[0], &WakeBuffer, 20);												// Populate the device wake string to wake targetboard if it is in the sleep mode
	UART2_TXcount = 20;																			// Increment position by 20 (size of wake buffer)

	memcpy(&UART2_TXbuffer[UART2_TXcount], data, arraysize);									// Copy data to be transmitted into UART2 transmit buffer
	UART2_TXcount += arraysize;																	// Increment UART2 position by sizeof data added

	memcpy(&UART2_TXbuffer[UART2_TXcount], &WakeBuffer[0], 2);
	UART2_TXcount+=2;																			// Set Sleepstate to false following population of buffer
	BoardCommsReceiveState = RxWaiting;															// Set boardcomms state to waiting, waiting for a reply from the target board
	BoardCommsTimeout = 10;																		// Boardcomms timeout to wait for sleep command following reception of data from targetboard prior to sending next string
	USART2->CR1 |= USART_CR1_TXEIE;																// Reenable transmit interupt
}

void printT(uns_ch * Text) {																	// Routine to take string and print it on the debug terminal
	setTerminalTimeOut(10);																		// 20ms timeout to display string on the terminal
	while ( (CDC_Transmit_FS(Text, strlen((char*)Text)) != USBD_OK) && terminalTimeOutEn) {		// Reattempt to send string if the CDC does not return OK

	}
	terminalTimeOutEn = false;																	// Set timeout enable to false following transmission
}
