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

#define LATCH_HIGH_ADC_THRESHOLD  ( (9.8 * 0.9) / ADC_MAX_INPUT_VOLTAGE) * ADC_RESOLUTION // Ensure LATCH high voltage is above 90% of source voltage
#define LATCH_LOW_ADC_THRESHOLD  ( (9.8 * 0.2 ) / ADC_MAX_INPUT_VOLTAGE) * ADC_RESOLUTION // Ensure LATCH low voltage is below 10% of source voltage

	// Latch Test Thresholds
#define LATCH_PULSE_HIGH_THRESHOLD	9.2
#define LATCH_PULSE_LOW_THRESHOLD	1.2
#define V_SOURCE_LOW_THRESHOLD		10.5
#define LATCH_PULSE_WIDTH_MIN		48
#define LATCH_PULSE_WIDTH_MAX		52



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
#define PORT_A_PULSEWIDTH_ERROR 	(1 << 0)
#define PORT_A_VOLTAGE_ERROR 		(1 << 1)
#define PORT_B_PULSEWIDTH_ERROR		(1 << 2)
#define PORT_B_VOLTAGE_ERROR		(1 << 3)
#define INPUT_VOLTAGE_ERROR			(1 << 4)
#define FUSE_VOLTAGE_ERROR			(1 << 5)
#define PORT_A_MOSFET_ERROR			(1 << 6)
#define PORT_B_MOSFET_ERROR			(1 << 7)

uint8 LatchTestErrorRegister;

	// Latch Test Status Registers
#define STABLE_INPUT_VOLTAGE 	(1 << 0)
#define LATCH_ON_COMPLETE 		(1 << 1)
#define LATCH_OFF_COMPLETE		(1 << 2)
#define LATCH_SAMPLING			(1 << 3)
#define LATCH_ON_SAMPLING		(1 << 4)
#define LATCH_OFF_SAMPLING		(1 << 5)

uint8 LatchTestStatusRegister;

	// Voltage Sampling Thresholds
#define VOLTAGE_SAMPLING_ACCURACY 		0.1 		// 5% accuracy check for board voltages
#define SOLAR_CHARGER_THRESHOLD 		13.8
#define INPUT_VOLTAGE_THRESHOLD 		12.0
#define OUTPUT_VOLTAGE_VALUE			12.0
#define TWELVE_VOLT_SAMPLE_VALUE	 	12.0
#define THREE_VOLT_SAMPLE_VALUE			3.0
#define TEN_VOLT_SAMPLE_THRESHOLD		10.5

#define GET_SAMPLE_VOLTAGE_TOLERANCE(VAL) (VAL*VOLTAGE_SAMPLING_ACCURACY); // 10% Accurancy check for board voltages

#endif /* INC_ADC_VARIABLES_H_ */
