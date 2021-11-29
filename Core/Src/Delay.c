#include <main.h>
#include "Delay.h"
#include "Global_Variables.h"

	// us Delay function, uses timer 13 in one pulse mode, sets the Count (CNT) register to zero and increments to the DelayTime passed to the function
void delay_us(int DelayTime) {
	TIM13->ARR = DelayTime;
	HAL_TIM_Base_Start(&htim13);
	TIM13->CNT = 0;
	while (TIM13->CNT < DelayTime) {
	}
	HAL_TIM_Base_Stop(&htim13);
}
