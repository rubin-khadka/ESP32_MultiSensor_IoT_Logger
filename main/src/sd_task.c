/*
 * sd_task.c
 *
 *  Created on: Jul 13, 2026
 *      Author: Rubin Khadka
 */

#include "esp_log.h"
#include "sd_card.h"
#include "sensor_data.h"

static const char *TAG = "SD_TASK";

void sd_logger_task(void *pvParameters) {
	TickType_t last_wake = xTaskGetTickCount();
	char timestamp[30];

	ESP_LOGI(TAG, "SD Logger Task started!");

	while (1) {
		if (xSemaphoreTake(g_sensor_data.mutex, pdMS_TO_TICKS(100)) == pdTRUE) {

			sprintf(timestamp, "20%02d-%02d-%02d %02d:%02d:%02d",
					g_sensor_data.year, g_sensor_data.month, g_sensor_data.day,
					g_sensor_data.hours, g_sensor_data.minutes,
					g_sensor_data.seconds);

			SD_Card_Log_Data(timestamp, g_sensor_data.temperature,
							 g_sensor_data.humidity, g_sensor_data.accel_x,
							 g_sensor_data.accel_y, g_sensor_data.accel_z,
							 g_sensor_data.gyro_x, g_sensor_data.gyro_y,
							 g_sensor_data.gyro_z);

			xSemaphoreGive(g_sensor_data.mutex);
		}

		vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(5000));
	}
}
