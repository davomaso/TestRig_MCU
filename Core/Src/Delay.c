#include <main.h>
#include "interogate_project.h"

void delay_us(int DelayTime)
{
	if(DelayTime > 3)
		DelayTime -= 3;
	else
		DelayTime = 1;
	TIM13->ARR = DelayTime;
	HAL_TIM_Base_Start(&htim13);
	while(TIM13->CNT < DelayTime);
}
