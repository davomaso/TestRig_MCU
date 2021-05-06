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

#define ADC_R1 5000
#define ADC_R2 5000
#define ADC_R3 1000
#define ADC_Rcurrent 3.3

#define V_SOURCE 12.0
#define ADC_VREF 2.5
#define ADC_RESOLUTION 4096
#define ADC_MAX_INPUT_VOLTAGE ( ADC_VREF * ( (ADC_R2+ADC_R3) / ADC_R3 ) )

#define LATCH_HIGH_VOLTAGE_THRESHOLD ( (V_SOURCE * 0.9) / ADC_MAX_INPUT_VOLTAGE) * ADC_RESOLUTION // Ensure LATCH high voltage is above 90% of source voltage
#define LATCH_LOW_VOLTAGE_THRESHOLD ( (V_SOURCE * 0.2 ) / ADC_MAX_INPUT_VOLTAGE) * ADC_RESOLUTION // Ensure LATCH low voltage is below 10% of source voltage


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

#define STABLE_INPUT_VOLTAGE 	1
#define LATCH_ON_COMPLETE 		2
#define LATCH_OFF_COMPLETE		4
#define LATCH_SAMPLING			8

uint8 LatchTestControlRegister;

#endif /* INC_ADC_VARIABLES_H_ */
