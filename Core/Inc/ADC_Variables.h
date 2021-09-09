/*
 	  	  	  	  	  _____ Vsource
						|
						/
						\  ADC_R1
						/
Latch Port Input________|________/\/\/\______________OPAMP
								ADC_R2        |
							  	  	  	  	  |
							  	  	  	  	  /
							  	  	  	  	  \ ADC_R3
							  	  	  	  	  /
							  	  	  	  	  |
							 	 	 	 	 GND
*/
#ifndef INC_ADC_VARIABLES_H_
#define INC_ADC_VARIABLES_H_

#include "math.h"

#define ADC_R1 6200
#define ADC_R2 5100
#define ADC_R3 1000
#define ADC_Rcurrent 3.3

#define V_SOURCE 15.25
#define ADC_VREF 2.5
#define ADC_RESOLUTION 4096
#define ADC_MAX_INPUT_VOLTAGE ( ADC_VREF * ( (ADC_R2+ADC_R3) / ADC_R3 ) )

#define LATCH_HIGH_VOLTAGE_THRESHOLD  ( (9.8 * 0.9) / ADC_MAX_INPUT_VOLTAGE) * ADC_RESOLUTION // Ensure LATCH high voltage is above 90% of source voltage
#define LATCH_LOW_VOLTAGE_THRESHOLD  ( (9.8 * 0.2 ) / ADC_MAX_INPUT_VOLTAGE) * ADC_RESOLUTION // Ensure LATCH low voltage is below 10% of source voltage


// Input Voltages
/*
 	  	  	  	  	  _____ Vsource
						|
						/
						\  INPUT_ADC_R1
						/
Latch Port Input________|________/\/\/\______________OPAMP
					|	  INPUT_ADC_R2        |
					|		  	  	  	  	  |
					/		  	  	  	  	  /
					\ _ADC_R4	  	  	  	  \ INPUT_ADC_R3
					/		  	  	  	  	  /
					|		  	  	  	  	  |
				   GND		 	 	 	 	 GND
*/

#define INPUT_ADC_R1 6200
#define INPUT_ADC_R2 5100
#define INPUT_ADC_R3 1000
#define INPUT_ADC_R4 100	//Not required for calculations
#define MAX_SOURCE_VALUE ( (ADC_VREF * (INPUT_ADC_R2 + INPUT_ADC_R3) ) / INPUT_ADC_R3 )


	//Latch Test Registers
#define PORT_A_PULSEWIDTH_ERROR 1
#define PORT_A_VOLTAGE_ERROR 	2
#define PORT_B_PULSEWIDTH_ERROR	4
#define PORT_B_VOLTAGE_ERROR	8
#define INPUT_VOLTAGE_ERROR		16
#define FUSE_VOLTAGE_ERROR		32
#define PORT_A_MOSFET_ERROR		64
#define PORT_B_MOSFET_ERROR		128

uint8 LatchTestErrorRegister;

	// Latch Test Status Registers
#define STABLE_INPUT_VOLTAGE 	1
#define LATCH_ON_COMPLETE 		2
#define LATCH_OFF_COMPLETE		4
#define LATCH_SAMPLING			8
#define LATCH_ON_SAMPLING		16
#define LATCH_OFF_SAMPLING		32

uint8 LatchTestStatusRegister;

	// Voltage Constants	// Thresholds for voltage calculations in stm32_f4_it.c
#define VIN_STABLE_THRESHOLD	3000
#define LATCH_ON_THRESHOLD		2400
#define LATCH_OFF_THRESHOLD		100

#endif /* INC_ADC_VARIABLES_H_ */
