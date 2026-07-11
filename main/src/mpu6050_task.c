/*
 * mpu6050_task.c
 *
 *  Created on: Jul 10, 2026
 *      Author: Rubin Khadka
 */
 
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "portmacro.h"
#include "sensor_data.h"
#include "mpu6050.h"
#include "esp_log.h"

static const char *TAG = "MPU6050_TASK";
 
void mpu6050_task(void *pvParameters)
{
	MPU6050_Data_t mpu_data;
	TickType_t last_wake_time = xTaskGetTickCount();
	
	ESP_LOGI(TAG, "MPU6050 Task Started !!!");
	
	while (1)
	{
		if (MPU6050_Read(&mpu_data) == ESP_OK)
		{
			// Lock Shared data
			if (xSemaphoreTake(g_sensor_data.mutex, pdMS_TO_TICKS(100)) == pdTRUE)
			{
				// Update global data
				g_sensor_data.accel_x = mpu_data.accel_x;
				g_sensor_data.accel_x = mpu_data.accel_y;
				g_sensor_data.accel_x = mpu_data.accel_z;
				g_sensor_data.gyro_x = mpu_data.gyro_x;
				g_sensor_data.gyro_y = mpu_data.gyro_y;
				g_sensor_data.gyro_z = mpu_data.gyro_z;
				
				// Unlock
				xSemaphoreGive(g_sensor_data.mutex);
				
				ESP_LOGI(TAG, "Accel: X=%.2f Y=%.2f Z=%.2f | Gyro: X=%.1f Y=%.1f Z=%.1f",
				         mpu_data.accel_x, mpu_data.accel_y, mpu_data.accel_z,
				         mpu_data.gyro_x, mpu_data.gyro_y, mpu_data.gyro_z);
			}
		} else {
			ESP_LOGW(TAG, "MPU6050 Read Failed !!!");
		}
		
		// Run every 50ms
		vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(50));
	}
}
 