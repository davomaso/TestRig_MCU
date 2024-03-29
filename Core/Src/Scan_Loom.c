#include <main.h>
#include "Global_Variables.h"
#include "LCD.h"
#include "Delay.h"
#include "UART_Routine.h"

/*
 * Scan through the loom for grounds on each loomsel pin to determine which board is connected.
 * Multiplexer is used to cutdown on pin usage. hardware change could be made to remove the multiplexer, allowing for no stopping when programming or latching occurs
 */
void ScanLoom(uint8 *Loom) {
	uint8 PrevLoomState;
	PrevLoomState = *Loom;
	*Loom = 0x00;
	for (int i = 0; i < 4; i++) {
		ADC_MUXsel(i);
		delay_us(50); // Wait mux and input to become stable
		if (!HAL_GPIO_ReadPin(Loom_Sel_GPIO_Port, Loom_Sel_Pin)) {
			*Loom |= (1 << (i));	// bitshift 1 up the loom pin being scanned if ground is detected
		}
	}
	if (*Loom != PrevLoomState) {
		CurrentState = csCheckLoom;	// Return the system to check loom if a change occurs during normal operation
		ProcessState = psInitalisation;
	}
	CheckLoomTimer = 250;	// 250ms until next scan
}

// Once loom is scanned board type can be established. Routine sets the board time and prints the loom connected to the terminal
void SetBoardType(TboardConfig *Board, uint8 Loom) {
	switch (Loom) {
	case 0:
		Board->BoardType = bNone;
		printT((uns_ch*) "Connect Loom...\n");
		break;
	case 1:
		Board->BoardType = b935x;
		printT((uns_ch*) "9352 Connected...\n");
		break;
	case 2:
		Board->BoardType = b937x;
		printT((uns_ch*) "9371 Connected...\n");
		break;
	case 3:
		Board->BoardType = b401x;
		printT((uns_ch*) "401x Connected...\n");
		break;
	case 4:
		Board->BoardType = b402x;
		printT((uns_ch*) "4022 Connected...\n");
		break;
	case 5:
		Board->BoardType = b427x;
		printT((uns_ch*) "4271 Connected...\n");
		break;
	case 6:
		Board->BoardType = b422x;
		printT((uns_ch*) "4220 Connected...\n");
		break;
	default:
		Board->BoardType = bNone;
		break;
	}
}

// Routine populates the boardconnected struct with the various variables and structs to run a successful test
void currentBoardConnected(TboardConfig *Board) {
	switch (Board->BoardType) {
	case b935x:
		TestConfig935x(Board);
		break;
	case b937x:
		TestConfig937x(Board);
		break;
	case b401x:
		TestConfig401x(Board);
		break;
	case b402x:
		TestConfig402x(Board);
		break;
	case b422x:
		TestConfig422x(Board);
		break;
	case b427x:
		TestConfig427x(Board);
		break;
	case bNone:
		break;
	default:
		printT((uns_ch*) "\n BoardConfig Error/Loom Connected Error \n");
		break;
	}
}

