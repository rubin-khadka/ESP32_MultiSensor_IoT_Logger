/*
 * dht11_task.c
 *
 *  Created on: Jul 10, 2026
 *      Author: Rubin Khadka
 */

#include "esp_err.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "portmacro.h"
#include "sensor_data.h"
#include "dht11.h"
#include "esp_log.h"

static const char *TAG = "DHT11_TASK";

// DHT11 Task function
void dht11_task(void *pvParameters)
{
	dht11_data_t dht_data;
	TickType_t last_wake_time = xTaskGetTickCount();
	
	ESP_LOGI(TAG, "DHT11 Task Started !!!");
	
	while (1)
	{
		if (DHT11_Process(&dht_data) == ESP_OK)
		{
			// Lock shared data
			if (xSemaphoreTake(g_sensor_data.mutex, pdMS_TO_TICKS(100)) == pdTRUE)
			{
				// Update global data
				g_sensor_data.temperature = dht_data.temperature;
				g_sensor_data.humidity = dht_data.humidity;
				
				// Unlock
				xSemaphoreGive(g_sensor_data.mutex);
				
				ESP_LOGI(TAG, "Temp: %.1f°C, Hum: %.1f%%", dht_data.temperature, dht_data.humidity);
			}
		} else 
		{
			ESP_LOGW(TAG, "DHT11 Reading Failed, skipping ...");
		}
		
		// Wait 2000 ms 
		vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(2000));
	}
}
