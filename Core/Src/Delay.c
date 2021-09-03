#include <main.h>
#include "Delay.h"
#include "Global_Variables.h"

void delay_us(int DelayTime) {
	TIM13->ARR = DelayTime;
	HAL_TIM_Base_Start(&htim13);
	TIM13->CNT = 0;
	while (TIM13->CNT < DelayTime) {
	}
	HAL_TIM_Base_Stop(&htim13);
}
