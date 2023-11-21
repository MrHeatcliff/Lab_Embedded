/*
 * scheduler.h
 *
 *  Created on: Nov 15, 2023
 *      Author: Heathcliff
 */

#ifndef INC_SCHEDULER_H_
#define INC_SCHEDULER_H_

#include "main.h"
#include "stdint.h"


#define	NO_TASK_ID				0
typedef struct{
	void (*pTask)(void);
	uint32_t 	Delay;
	uint32_t 	Period;
	uint8_t		RunMe;
	struct sTask* 		nextTask;
	uint32_t 	TaskID;
} sTask;

typedef struct{
	sTask * top;
} stack;


void SCH_Init(void);

uint32_t SCH_Add_Task (void (*pFunction)(),
						uint32_t	DELAY,
						uint32_t	PERIOD,
						uint32_t ID);
void SCH_Update(void);

void SCH_Dispatch_Tasks(void);

uint8_t SCH_Delete(uint32_t ID);

#endif /* INC_SCHEDULER_H_ */
