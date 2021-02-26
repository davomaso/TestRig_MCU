/*
 * Asynchronous_pulse.h
 *  Created on: May 19, 2020
 *      Author: David
 */
#ifndef INC_ASYNCHRONOUS_PULSE_H_
#define INC_ASYNCHRONOUS_PULSE_H_

#include "stdlib.h"

#define PULSE_TIMER 40			//Overall Timer, Run for 2Sec

_Bool Pulse_High;				//Pulse output, will be placed onto a GPIO pin
unsigned char Random_timer_val;	//Length between pulses
unsigned char Low_timer;		//Counts to the time value between pulses
unsigned char Pulse_count;		//Counts amount of pulses in the specified time
unsigned char Master_count;


#endif /* INC_ASYNCHRONOUS_PULSE_H_ */
