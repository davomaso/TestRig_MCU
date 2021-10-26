#include "main.h"
#include "Global_Variables.h"
#include "stm32f4xx_it.h"
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"
#include "UART_Routine.h"
#include "Delay.h"

void UART2_transmit(unsigned char *data, unsigned char arraysize) {
	/*
	 * UART2 Transmit Routine for board comms
	 * While in the sleepstate 0x55 will be transmitted twice and 0x00 transmitted twice to wake the target board
	 * following this the data pointer will be loaded into the TXbuffer for the size specified.
	 * Once Complete the transmit interrupt is enabled and the data begun to be transmitted out the UART port.
	 */
	//Disable transmit interrupt
	const uint8 WakeBuffer[20] = { 0x55, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	USART2->CR1 &= ~(USART_CR1_TXEIE);		// Disable transmit interupt enable and transmit complete registers
	USART2->CR1 &= ~(USART_CR1_TCIE);
	if (!Sleepstate) {
		Sleepstate = true;
		unsigned char i = 0;
		while (i < 20) {
			while (i < 2) {
				//0x55 55 @ start of communication to ensure target board is awake
				UART2_TXbuffer[UART2_TXcount++] = '\x55';
				i++;
			}
			UART2_TXbuffer[UART2_TXcount++] = '\x00';
			i++;
		}
	}
		// Copy data to be transmitted into UART2 transmit buffer
	memcpy(&UART2_TXbuffer[UART2_TXcount], data, arraysize);
	UART2_TXcount += arraysize;
	//Return system to sleep state
	if (Sleepstate) {
		memcpy(&UART2_TXbuffer[UART2_TXcount], &WakeBuffer[0], 2);
		UART2_TXcount+=2;
		Sleepstate = false;									// Set Sleepstate to false following population of buffer
	}
	BoardCommsReceiveState = RxWaiting;						// Set boardcomms state to waiting, waiting for a reply from the target board
	USART2->CR1 |= USART_CR1_TXEIE;							// Reenable transmit interupt
}

void printT(uns_ch * Text) {
	// Routine to take string and print it on the debug terminal
	setTimeOut(10);														// 20ms timeout to display string on the terminal
	while (CDC_Transmit_FS(Text, strlen((char*)Text)) != USBD_OK) {		// Reattempt to send string if the CDC does not return OK

	}
	timeOutEn = false;													// Set timeout enable to false following transmission
}
