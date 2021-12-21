/*
 * DAC_Variables.h
 *
 *  Created on: Mar 29, 2021
 *      Author: mason
 */

#ifndef INC_DAC_VARIABLES_H_
#define INC_DAC_VARIABLES_H_

#include "math.h"
	//DAC Constants
#define DAC_RESOLUTION 4096.0		// DAC resolution
#define DAC_MAX_OUT 2.048			// MAX DAC Voltage

	//Voltage Calculations
#define V_R1 20.0					// Voltage amplifier resistor R1
#define V_Rf 15.0					// Feedback resistance
#define VOLTAGE_GAIN (1.0+V_Rf/V_R1)
#define DAC_MAX_VOLTAGE (DAC_MAX_OUT * VOLTAGE_GAIN)	//MAX Voltage possible at V out

	//Current Calculations
#define I_R1 10000.0				// Current Pump R1
#define I_R2 20000.0				// Current Pump R2
#define I_Rp 180.0					// Current Pump Rp

#define CURRENT_GAIN ((I_R2+I_Rp) / I_R1)
#define DAC_MAX_CURRENT ( (DAC_MAX_OUT * CURRENT_GAIN) / I_Rp * 1000) //Maximum DAC output multiplied by the current gain of the pump, divided by the pump resistance times 1000 to get in mA base

//Using the value calculated above the DAC values required to generate the uncorrected current values below can be determined
#define DAC_VOLTAGE_CALC(VAL) (round( (VAL/DAC_MAX_VOLTAGE) * DAC_RESOLUTION))
#define DAC_CURRENT_CALC(VAL) ( (VAL / DAC_MAX_CURRENT) * DAC_RESOLUTION)

#endif /* INC_DAC_VARIABLES_H_ */
