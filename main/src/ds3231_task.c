/*
 * ds3231_task.c
 *
 *  Created on: Jul 10, 2026
 *      Author: Rubin Khadka
 */

#include "ds3231.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "i2c.helper.h"
#include "esp_log.h"
#include "portmacro.h"
#include "sensor_data.h"

static const char *TAG = "DS3231_TASK";

void ds3231_task(void *pvParameters)
{
	DS3231_Time_t rtc_time;
	TickType_t last_wake_time = xTaskGetTickCount();
	
	ESP_LOGI(TAG, "DS3231 Task Started !!!");
	
	while (1)
	{
		// Read DS3231 time
		if (DS3231_GetTime(&rtc_time) == ESP_OK) {
			// Lock shared data
			if (xSemaphoreTake(g_sensor_data.mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
				// Update Global data
				g_sensor_data.hours = rtc_time.hours;
				g_sensor_data.minutes = rtc_time.minutes;
				g_sensor_data.seconds = rtc_time.seconds;
				g_sensor_data.day = rtc_time.dayofmonth;
				g_sensor_data.month = rtc_time.month;
				g_sensor_data.year = rtc_time.year;
				
				// Unlock
				xSemaphoreGive(g_sensor_data.mutex);\
				
				ESP_LOGI(TAG, "%02d/%02d/20%02d %02d:%02d:%02d",
				         rtc_time.dayofmonth, rtc_time.month, rtc_time.year,
				         rtc_time.hours, rtc_time.minutes, rtc_time.seconds);
			}
		} else {
			ESP_LOGW(TAG, "DS3231 read failed!");
		}
		
		// Run at 1 Hz
		vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(1000));
	}
}
