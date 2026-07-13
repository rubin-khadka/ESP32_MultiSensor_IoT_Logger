/*
 * button.h
 *
 *  Created on: Jul 12, 2026
 *      Author: Rubin Khadka
 */

#ifndef MAIN_INC_BUTTON_H_
#define MAIN_INC_BUTTON_H_

#include <stdint.h>

#define BUTTON1_PIN GPIO_NUM_0
#define BUTTON2_PIN GPIO_NUM_12
#define BUTTON3_PIN GPIO_NUM_13

extern volatile uint8_t g_button2_pressed;
extern volatile uint8_t g_button3_pressed;

void Button_Init(void);
uint8_t Button_GetMode(void);

#endif /* MAIN_INC_BUTTON_H_ */
