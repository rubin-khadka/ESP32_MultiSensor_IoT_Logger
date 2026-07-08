/*
 * ds3231.c
 *
 *  Created on: Jul 7, 2026
 *      Author: Rubin Khadka
 */

#include "ds3231.h"
#include "esp_err.h"
#include "i2c.helper.h"
#include "esp_log.h"
#include "unity_internals.h"
#include <stdint.h>

static const char *TAG = "DS3231";

// BCD to Decimal
static uint8_t DS3231_DecToBcd(uint8_t val)
{
	return ((val / 10) << 4) | (val % 10);	
}

// Decimal to BCD
static uint8_t DS3231_BcdToDec(uint8_t val)
{
	return ((val >> 4) *10) + (val & 0x0F);
}

// Initialize DS3231
esp_err_t DS3231_Init(void)
{
	ESP_LOGI(TAG, "Initializing DS3231 ...");
	
	uint8_t status = 0;
	uint8_t is_first_boot = 0;
	
	// Read Status Register
	if (I2C_Read_Reg(DS3231_ADDR, DS3231_REG_STATUS, &status) != ESP_OK)
	{
		ESP_LOGE(TAG, "Failed to read DS3231 !!!");
		return ESP_FAIL;
	}
	
	// Check Oscillator stop flag
	if (status & 0x80)
	{
		is_first_boot = 1;
		
		// Clear the flag 
		status &= ~0x80;
		I2C_Write_Reg(DS3231_ADDR, DS3231_REG_STATUS, status);
		ESP_LOGW(TAG, "Oscillator Stopped !!!");
	}
	
	if (is_first_boot)
	{
		ESP_LOGI(TAG, "Setting Initial Time");
		
		uint8_t time_data[7];
		time_data[0] = DS3231_DecToBcd(0);		// Seconds
		time_data[1] = DS3231_DecToBcd(39);	// Minutes
		time_data[2] = DS3231_DecToBcd(0);		// Hours
		time_data[3] = DS3231_DecToBcd(0);		// Day of Week
		time_data[4] = DS3231_DecToBcd(0);		// Day of Month
		time_data[5] = DS3231_DecToBcd(0);		// Month
		time_data[6] = DS3231_DecToBcd(0);		// Year
		
		// Write all 7 registers
		for (int i = 0; i < 7; i++) 
		{
			I2C_Write_Reg(DS3231_ADDR, DS3231_REG_SECONDS + i, time_data[i]);
		}
	}
	
	// Disable 32KHZ output
	I2C_Write_Reg(DS3231_ADDR, DS3231_REG_CONTROL, 0x00);
	
	ESP_LOGI(TAG, "DS3231 Initialized Successfully");
	return ESP_OK;
}

esp_err_t DS3231_GetTime(DS3231_Time_t *time)
{
	uint8_t data[7];
	
	// Brust read 7 bytes
	if (I2C_Read_Brust(DS3231_ADDR, DS3231_REG_SECONDS, data, 7) != ESP_OK)
	{
		ESP_LOGE(TAG, "Failed to read time !!!");
		return ESP_FAIL;
	}
	
	// Convert BCD to Decimal
	time->seconds 		= DS3231_BcdToDec(data[0] & 0x7F);
	time->minutes 		= DS3231_BcdToDec(data[1] & 0x7F);
	time->hours 		= DS3231_BcdToDec(data[2] & 0x3F);
	time->dayofweek 	= DS3231_BcdToDec(data[3] & 0x0F);
	time->dayofmonth 	= DS3231_BcdToDec(data[4] & 0x3F);
	time->month 		= DS3231_BcdToDec(data[5] & 0x1F);
	time->year 			= DS3231_BcdToDec(data[6]);
	
	ESP_LOGI(TAG, "%02d/%02d/20%02d %02d:%02d:%02d",
	         time->dayofmonth, time->month, time->year,
	         time->hours, time->minutes, time->seconds);

	return ESP_OK;
}

esp_err_t DS3231_SetTime(DS3231_Time_t *time)
{
	uint8_t data[7];
	
	// Convert to BCD
	data[0] = DS3231_DecToBcd(time->seconds);
	data[1] = DS3231_DecToBcd(time->minutes);
	data[2] = DS3231_DecToBcd(time->hours);
	data[3] = DS3231_DecToBcd(time->dayofweek);
	data[4] = DS3231_DecToBcd(time->dayofmonth);
	data[5] = DS3231_DecToBcd(time->month);
	data[6] = DS3231_DecToBcd(time->year);
	
	// Write all 7 register
	for (int i = 0; i < 7; i++) 
	{
		I2C_Write_Reg(DS3231_ADDR, DS3231_REG_SECONDS + i, data[i]);
	}
	
	ESP_LOGI(TAG, "Time set to: %02d/%02d/20%02d %02d:%02d:%02d",
	         time->dayofmonth, time->month, time->year,
	         time->hours, time->minutes, time->seconds);
	return ESP_OK;
}

float DS3231_GetTemperature(void)
{
	uint8_t temp_msb = 0;
	uint8_t temp_lsb = 0;
	
	if (I2C_Read_Reg(DS3231_ADDR, DS3231_REG_TEMP_MSB, &temp_msb) != ESP_OK)
	{
		return 0.0f;
	}
	
	if (I2C_Read_Reg(DS3231_ADDR, DS3231_REG_TEMP_MSB, &temp_msb) != ESP_OK)
	{
		return 0.0f;
	}
	
	float temperature = (float)temp_msb;
	 temperature += ((float)(temp_lsb >> 6)) * 0.25f;
	
	return temperature;
}
