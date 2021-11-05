#include <main.h>
#include "Global_Variables.h"
#include "SerialNumber.h"
#include "LCD.h"
#include "string.h"
#include "UART_Routine.h"

uns_ch ScanKeypad() {
	for (uint8 i = 0; i <= 9; i++) {
		if (KP[i].Pressed) {			// Pressed toggled and debounced in the interrupt
			KP[i].Pressed = false;		// Disable pressed flag and return the ascii value of the keypressed
			 return 0x30 + i;
		}
	} return 0; 						// return zero if no key pressed.
}
