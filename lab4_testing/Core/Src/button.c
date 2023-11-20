#include "button.h"
#define NUMBER_OF_BUTTON 	1

// long pressed time is for auto increasing when hold button
#define LONG_PRESSED_TIME	 5// 250ms
/*Struct for reading button*/
struct {
	int reg[4];
	int is_pressed;
	int is_long_pressed;
	unsigned int timer;
	uint16_t pin;
} button[NUMBER_OF_BUTTON];
/*
 * button[0]: transitioning-mode button
 * button[1]: increasing-value button
 * button[2]: setting-value button*/
void initButton(void){
	for(int i=0; i<NUMBER_OF_BUTTON; i++){
		button[i].reg[0] = button[i].reg[1] = button[i].reg[2] = button[i].reg[3] = NORMAL_STATE;
		button[i].is_long_pressed = 0;
		button[i].is_pressed = 0;
		button[i].timer = 0;
	}
	button[0].pin = GPIO_PIN_7;
//	button[1].pin = GPIO_PIN_8;
//	button[2].pin = GPIO_PIN_9;
}
void buttonRead(void){
	for (unsigned i = 0; i < NUMBER_OF_BUTTON; i++) {
		button[i].reg[0] = button[i].reg[1];
		button[i].reg[1] = button[i].reg[2];
		button[i].reg[2] = HAL_GPIO_ReadPin(GPIOA, button[i].pin);
		if (button[i].reg[0] == button[i].reg[1]
				&& button[i].reg[1] == button[i].reg[2]) {
			//stable state, not bouncing
			// if state was change, then processing
			if(button[i].reg[3] != button[i].reg[2]){
				button[i].reg[3] = button[i].reg[2];
				// if button was not press to press
				if(button[i].reg[2] == PRESSED_STATE){
					//TODO
					button[i].is_pressed = 1;
					button[i].timer = LONG_PRESSED_TIME;
				}
				// if button was pressed to not pressed
				else{
					button[i].is_pressed = 0;
					button[i].is_long_pressed = 0;
					button[i].timer = LONG_PRESSED_TIME;
				}
			// if button was not change, so execute when it hold
			} else{
				button[i].timer--;
				if(button[i].timer == 0){
					// TODO
					if(button[i].reg[2] == PRESSED_STATE){
						button[i].is_long_pressed = 1;
					}
					button[i].timer = LONG_PRESSED_TIME;
				}
			}
		}
	}
}
// funtion to check if button was pressed
int buttonPressed(unsigned i){
	if(button[i].is_pressed == 1){
		button[i].is_pressed = 0;
		return 1;
	}
	else return 0;
}
// function to check if button was hold
int buttonHold(unsigned i){
	if(button[i].is_long_pressed == 1){
		button[i].is_long_pressed = 0;
		 return 1;
	}
	else return 0;
}
