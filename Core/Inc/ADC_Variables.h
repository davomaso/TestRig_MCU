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

#define ADC_VREF 2.5
#define ADC_RESOLUTION 4096
#define ADC_MAX_INPUT_VOLTAGE ( ADC_VREF * ( (ADC_R2+ADC_R3) / ADC_R3 ) )
#define LATCH_HIGH_VOLTAGE_THRESHOLD 2400
#define LATCH_LOW_VOLTAGE_THRESHOLD 500

#endif /* INC_ADC_VARIABLES_H_ */
