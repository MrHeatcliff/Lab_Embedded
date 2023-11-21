/*
 * scheduler.c
 *
 *  Created on: Nov 15, 2023
 *      Author: Heathcliff
 */

#include "scheduler.h"

stack task_stack ;
uint32_t new_task_id = 0;
uint8_t current_task_index = 0;
static uint32_t Get_New_Task_ID(void);

void SCH_Init(void){
	task_stack.top = 0;
}

uint32_t SCH_Add_Task (void(*pFunction)(), uint32_t DELAY, uint32_t PERIOD, uint32_t ID){
	uint32_t sum_delay = 0;
	uint32_t new_delay = 0;
	sTask *curr_task = task_stack.top;
	sTask *pre_task = curr_task;
	sTask *add_task = (sTask*) malloc(sizeof(sTask));
	add_task->Delay = DELAY;
	add_task->Period = PERIOD;
	add_task->pTask = pFunction;
	if(DELAY == 0){
		add_task->RunMe = 1;
	} else add_task->RunMe = 0;
	if(ID == 0) add_task->TaskID = Get_New_Task_ID();
	add_task->nextTask = 0;
	//CHECK IF THE STACK WAS EMPTY OR NOT
	if(!curr_task){
		task_stack.top = add_task;
		return task_stack.top->TaskID;
	}
	// FIND POSITION TO ADD TASK
	while(curr_task){
		sum_delay += curr_task->Delay;
		if(add_task->Delay < sum_delay) break;
		pre_task = curr_task;
		curr_task = curr_task->nextTask;
	}
	// IF ADD TO TAIL
	if(curr_task == 0){
		add_task->Delay -= sum_delay;
		if(add_task->Delay == 0){
			add_task->RunMe = 1;
		}
		pre_task->nextTask = add_task;
		return add_task->TaskID;
	}
	// IF ADD TO MIDDLE
	if(pre_task != curr_task){
		new_delay = add_task->Delay - (sum_delay - curr_task->Delay);
		curr_task->Delay = sum_delay - add_task->Delay;
		add_task->Delay = new_delay;
		if(curr_task->Delay == 0){
			curr_task->RunMe = 1;
		}
		if(new_delay == 0){
			add_task->RunMe = 1;
		}
		pre_task->nextTask = add_task;
		add_task->nextTask = curr_task;
		return add_task->TaskID;
	}
	// IF ADD TO HEAD
	else{
		curr_task->Delay = curr_task->Delay - add_task->Delay;
		if(curr_task->Delay == 0){
			curr_task->RunMe = 1;
		}
		add_task->nextTask = curr_task;
		task_stack.top = add_task;
		return add_task->TaskID;
	}
}

void SCH_Update(void){
	// CHECK IF HAVE A TASK AND IT'S NOT RUN YET
	if(task_stack.top && task_stack.top->RunMe == 0){
		if(task_stack.top->Delay > 0){
			task_stack.top->Delay --;
		}
		if(task_stack.top->Delay == 0) task_stack.top->RunMe = 1;
	}
}

void SCH_Dispatch_Tasks(void){
	if(task_stack.top->RunMe == 1){
		(task_stack.top->pTask)();
		task_stack.top->RunMe--;
		sTask temp = *(task_stack.top);
		if(task_stack.top->RunMe == 0){
			SCH_Delete(task_stack.top->TaskID);
			if(temp.Period !=0){
				SCH_Add_Task(temp.pTask, temp.Period, temp.Period, temp.TaskID);
			}
		}
	}
}

uint8_t SCH_Delete(uint32_t ID){
	sTask* curr_temp = task_stack.top;
	sTask* pre_temp = curr_temp;
	// FIND THE TASK TO DELETE
	while(curr_temp && curr_temp->TaskID != ID){
		pre_temp = curr_temp;
		curr_temp = curr_temp->nextTask;
	}
	// IF CANT FIND THE TASK, THEN RETURN 0
	if(!curr_temp) return 0;
	// IF THE TASK WAS AT THE TOP
	else if(curr_temp == pre_temp){
		task_stack.top = curr_temp->nextTask;
		curr_temp->nextTask = 0;
		free(curr_temp);
		return 1;
	}
	// IF THE TASK IS NOT AT THE TOP
	else{
		pre_temp->nextTask = curr_temp->nextTask;
		free(curr_temp);
		return 1;
	}
	return 0;
}

static uint32_t Get_New_Task_ID(void){
	new_task_id++;
	if(new_task_id == NO_TASK_ID){
		new_task_id++;
	}
	return new_task_id;
}
