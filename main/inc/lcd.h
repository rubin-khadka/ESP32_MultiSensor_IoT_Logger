/*
 * lcd.h
 *
 *  Created on: Jul 7, 2026
 *      Author: Rubin Khadka
 */

#ifndef MAIN_INC_LCD_H_
#define MAIN_INC_LCD_H_

#include "esp_err.h"

// PCF8574 I2C Address
#define LCD_ADDR 		0x27

// PCF8574 Controls
#define LCD_BACKLIGHT	0x08
#define LCD_ENABLE		0x04
#define LCD_RW			0x02
#define LCD_RS			0x01

#endif /* MAIN_INC_LCD_H_ */
