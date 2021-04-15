//UART Routine for UART2 and UART3
//will handle both receiving and transmitting

//Includes:
#include "main.h"
#include "stm32f4xx_it.h"
#include "UART_Routine.h"

//Author: David Mason
//last updated 19/11/2020

void UART2_transmit(unsigned char *data, unsigned char arraysize) {
	//Disable transmit interrupt
	USART2->CR1 &= ~(USART_CR1_TXEIE);
	USART2->CR1 &= ~(USART_CR1_TCIE);
	if (!Sleepstate) {
		Sleepstate = 1;
		unsigned char i = 0;
		while (i < 4) {
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
	USART2->CR1 |= USART_CR1_TXEIE;
}

void printT(uns_ch * Text) {
	HAL_UART_Transmit(&D_UART, Text, strlen(Text), HAL_MAX_DELAY);
}
