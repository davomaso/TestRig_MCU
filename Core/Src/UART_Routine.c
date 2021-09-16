#include "main.h"
#include "stm32f4xx_it.h"
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
	USART2->CR1 &= ~(USART_CR1_TXEIE);
	USART2->CR1 &= ~(USART_CR1_TCIE);
	if (!Sleepstate) {
		Sleepstate = 1;
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
	//if system has not been woken, send 2 bytes of 0x55 and 14 bytes of 0x00
	while (arraysize-- > 0)
		UART2_TXbuffer[UART2_TXcount++] = *data++;
	//Return system to sleep state
	if (Sleepstate) {
		for (unsigned char i = 0; i < 2; i++) {
			UART2_TXbuffer[UART2_TXcount++] = '\x55';
		}
		Sleepstate = 0;
	}
	BoardCommsReceiveState = RxWaiting;
	USART2->CR1 |= USART_CR1_TXEIE;
}

void printT(uns_ch * Text) {
	// Routine to take string and print it on the debug terminal
//	HAL_UART_Transmit(&D_UART, Text, strlen(Text), HAL_MAX_DELAY);
	CDC_Transmit_FS(Text, strlen((char*)Text));
	HAL_Delay(1);
}
