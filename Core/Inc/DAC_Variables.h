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
#define DAC_RESOLUTION 4096.0							//DAC resolution
#define DAC_MAX_OUT 2.048								//MAX DAC Voltage

	//Voltage Calculations
#define V_R1 15.0
#define V_Rf 20.0
#define VOLTAGE_GAIN (1.0+V_R1/V_Rf)
#define DAC_MAX_VOLTAGE (DAC_MAX_OUT * VOLTAGE_GAIN)	//MAX Voltage possible at V out

#define DAC_1volt (round( (1/DAC_MAX_VOLTAGE) * DAC_RESOLUTION))
#define DAC_05volt (round( (0.5/DAC_MAX_VOLTAGE) * DAC_RESOLUTION))
#define DAC_24volt (round( (2.4/DAC_MAX_VOLTAGE) * DAC_RESOLUTION))

	//Current Calculations
#define I_R1 10000.0
#define I_R2 20000.0
#define I_Rp 180.0
#define CURRENT_GAIN ((I_R2+I_Rp) / I_R1)
#define DAC_MAX_CURRENT ( (DAC_MAX_OUT * CURRENT_GAIN) / I_Rp * 1000)

#define DAC_4amp ( (4.0 / DAC_MAX_CURRENT) * DAC_RESOLUTION)
#define DAC_175amp  ( (17.5 / DAC_MAX_CURRENT) * DAC_RESOLUTION)
#define DAC_20amp ( (20.0 / DAC_MAX_CURRENT) * DAC_RESOLUTION)

#endif /* INC_DAC_VARIABLES_H_ */
