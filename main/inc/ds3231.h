/*
 * ds3231.h
 *
 *  Created on: Jul 7, 2026
 *      Author: Rubin Khadka
 */

#ifndef MAIN_INC_DS3231_H_
#define MAIN_INC_DS3231_H_

#include "esp_err.h"
#include <stdint.h>

// DS3231 I2C address
#define DS3231_ADDR 		0x68

// Register Address
#define DS3231_REG_SECONDS      0x00
#define DS3231_REG_CONTROL      0x0E
#define DS3231_REG_STATUS       0x0F
#define DS3231_REG_TEMP_MSB     0x11
#define DS3231_REG_TEMP_LSB     0x12

// Time Structure
typedef struct 
{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t dayofweek;
	uint8_t dayofmonth;
	uint8_t month;
	uint8_t year;
}DS3231_Time_t;

esp_err_t DS3231_Init(void);
esp_err_t DS3231_GetTime(DS3231_Time_t *time);
esp_err_t DS3231_SetTime(DS3231_Time_t *time);
float DS3231_GetTemperature(void);

#endif /* MAIN_INC_DS3231_H_ */
