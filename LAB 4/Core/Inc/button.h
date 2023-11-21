#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_
#include "main.h"

#define NORMAL_STATE	GPIO_PIN_SET
#define PRESSED_STATE	GPIO_PIN_RESET

void buttonRead(void);
void initButton(void);
int buttonPressed(unsigned i);
int buttonHold(unsigned i);

#endif /* INC_BUTTON_H_ */
