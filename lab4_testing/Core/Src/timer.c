/*
 * timer.c
 *
 *  Created on: Nov 15, 2023
 *      Author: Heathcliff
 */

#include "timer.h"

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	SCH_Update();
}
