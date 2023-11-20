/*
 * scheduler.c
 *
 *  Created on: Nov 15, 2023
 *      Author: Heathcliff
 */

#include "scheduler.h"

sTask SCH_tasks_G[SCH_MAX_TASKS];
uint32_t new_task_id = 0;
uint8_t current_task_index = 0;
static uint32_t Get_New_Task_ID(void);

void SCH_Init(void){
	current_task_index = 0;
	for(unsigned i = 0; i < SCH_MAX_TASKS; i++){
		SCH_tasks_G[i].pTask = 0x0000;
		SCH_tasks_G[i].Delay = 0;
		SCH_tasks_G[i].Period = 0;
		SCH_tasks_G[i].RunMe = 0;
		SCH_tasks_G[i].TaskID = 0;
	}
}

uint32_t SCH_Add_Task (void(*pFunction)(), uint32_t DELAY, uint32_t PERIOD, uint32_t ID){
	uint32_t sum_delay = 0;
	uint32_t new_sum_delay = 0;
	unsigned i = 0;
	// IF IT'S NOT FULL TASK
	if(current_task_index != SCH_MAX_TASKS){
		for(i = 0; i< current_task_index+1; i++){
				// if added task has delay time < sum from 0 to i delay time
				sum_delay +=SCH_tasks_G[i].Delay;
				if(DELAY < sum_delay){
					new_sum_delay = sum_delay;
					new_sum_delay -= SCH_tasks_G[i].Delay;
					new_sum_delay = DELAY - new_sum_delay;
					SCH_tasks_G[i].Delay = sum_delay - DELAY;
					for(unsigned j = current_task_index+1; j > i; j--){
						// shift task array to the right
						SCH_tasks_G[j].Delay = SCH_tasks_G[j-1].Delay;
						SCH_tasks_G[j].Period = SCH_tasks_G[j-1].Period;
						SCH_tasks_G[j].TaskID = SCH_tasks_G[j-1].TaskID;
						SCH_tasks_G[j].pTask = SCH_tasks_G[j-1].pTask;
						SCH_tasks_G[j].RunMe = SCH_tasks_G[j-1].RunMe;
					}
					// replace task
					SCH_tasks_G[i].Delay = new_sum_delay;
					SCH_tasks_G[i].Period = PERIOD;
					SCH_tasks_G[i].pTask = pFunction;
					if(SCH_tasks_G[i].Delay == 0){
						SCH_tasks_G[i].RunMe = 1;
					} else{
						SCH_tasks_G[i].RunMe = 0;
					}
					if(ID == 0){
						SCH_tasks_G[i].TaskID = Get_New_Task_ID();
					} else SCH_tasks_G[i].TaskID = ID;
					current_task_index++;
					return SCH_tasks_G[i].TaskID;
				} else {
					// CHECK IF IT IS THE LAST TASK TO EXCUTE THAN ADD TO TAIL OF ARRAY
					if(SCH_tasks_G[i].pTask == 0x0000){
						SCH_tasks_G[i].Delay = DELAY - sum_delay;
						SCH_tasks_G[i].Period = PERIOD;
						if(ID == 0){
							SCH_tasks_G[i].TaskID = Get_New_Task_ID();
						} else SCH_tasks_G[i].TaskID = ID;
						SCH_tasks_G[i].pTask = pFunction;
						if(SCH_tasks_G[i].Delay == 0){
							SCH_tasks_G[i].RunMe = 1;
						} else{
							SCH_tasks_G[i].RunMe = 0;
						}
						current_task_index++;
						return SCH_tasks_G[i].TaskID;
					}
				}
			}
	}
	return SCH_tasks_G[i].TaskID;
}

void SCH_Update(void){
	if (SCH_tasks_G[0].pTask && SCH_tasks_G[0].RunMe == 0) {
			if(SCH_tasks_G[0].Delay > 0){
				SCH_tasks_G[0].Delay = SCH_tasks_G[0].Delay - 1;
			}
			if (SCH_tasks_G[0].Delay == 0) {
				SCH_tasks_G[0].RunMe = 1;
			}
		}
}

void SCH_Dispatch_Tasks(void){
	sTask tempTask = SCH_tasks_G[0];
		if(SCH_tasks_G[0].RunMe > 0){
			SCH_tasks_G[0].RunMe--;
			(*SCH_tasks_G[0].pTask)();
			if(SCH_tasks_G[0].RunMe == 0){
				SCH_Delete(SCH_tasks_G[0].TaskID);
			}
			if(tempTask.Period !=0){
				SCH_Add_Task(tempTask.pTask, tempTask.Period, tempTask.Period , tempTask.TaskID);
			}
		}
}

uint8_t SCH_Delete(uint32_t ID){
	unsigned return_code = 0;
	unsigned taskIndex;
	unsigned j;
	if(ID != NO_TASK_ID){
		// FIND THE INDEX OF TASK TO DELETE
		for(taskIndex = 0; taskIndex < current_task_index; taskIndex++){
			if(SCH_tasks_G[taskIndex].TaskID == ID){
				// return if the task was find
				return_code = 1;
				// if deleted task it in the middle then check
				// the task has a function to executed or not
				// if it has, then change delay of next task by plus for its delay
				if(taskIndex != 0 && taskIndex < current_task_index - 1){
					if(SCH_tasks_G[taskIndex+1].pTask != 0x0000){
						SCH_tasks_G[taskIndex+1].Delay +=SCH_tasks_G[taskIndex].Delay;
					}
				}
				// shift all task to the left
				for(j = taskIndex; j < current_task_index-1; j++){
					SCH_tasks_G[j].pTask = SCH_tasks_G[j+1].pTask;
					SCH_tasks_G[j].Period = SCH_tasks_G[j+1].Period;
					SCH_tasks_G[j].Delay = SCH_tasks_G[j+1].Delay;
					SCH_tasks_G[j].RunMe = SCH_tasks_G[j+1].RunMe;
					SCH_tasks_G[j].TaskID = SCH_tasks_G[j+1].TaskID;
				}
				// delete the tail
				SCH_tasks_G[j].pTask = 0x0000;
				SCH_tasks_G[j].Period = 0;
				SCH_tasks_G[j].Delay = 0;
				SCH_tasks_G[j].RunMe = 0;
				SCH_tasks_G[j].TaskID = 0;
				current_task_index--;
				return return_code;
			}
		}
	}
	return return_code;
}

static uint32_t Get_New_Task_ID(void){
	new_task_id++;
	if(new_task_id == NO_TASK_ID){
		new_task_id++;
	}
	return new_task_id;
}
