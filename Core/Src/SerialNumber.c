#include "Global_Variables.h"
#include "SerialNumber.h"

unsigned char ScanKeypad() {
	for (unsigned char i = 0; i <= 9; i++) {
		if (KP[i].Pressed) {			// Pressed toggled and debounced in the interrupt
			KP[i].Pressed = false;		// Disable pressed flag and return the ascii value of the keypressed
			 return 0x30 + i;
		}
	} return 0; 						// return zero if no key pressed.
}
