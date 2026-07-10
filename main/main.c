#include "freertos/FreeRTOS.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "dht11.h"
#include "portmacro.h"
#include "sensor_data.h"
#include "soc/gpio_num.h"

static const char *TAG = "MAIN";

void dht11_task(void *pvParameters);

sensor_data_t g_sensor_data;

void app_main(void)
{
	ESP_LOGI(TAG, "ESP32 Multi-Sensor IoT Logger");
	
	// Create Mutex
	g_sensor_data.mutex = xSemaphoreCreateMutex();
	if (g_sensor_data.mutex == NULL)
	{
		ESP_LOGE(TAG, "Failed to create Mutex !!");
		return;
	}
	ESP_LOGI(TAG, "Mutex Created !!");
	
	// Initialize DHT11
	DHT11_Init(GPIO_NUM_4);
	ESP_LOGI(TAG, "DHT11 Initialized");
	
	// Create task
	BaseType_t result = xTaskCreate(dht11_task, "dht11_task", 4096, NULL, 2, NULL);
	
	if(result != pdPASS)
	{
		ESP_LOGE(TAG, "Failed to create DHT11 task !!");
	}
	ESP_LOGI(TAG, "DHT11 Task created successfully !!!");
	ESP_LOGI(TAG, "Scheduler Running !!!");
	
	while(1)
	{
		// Read sensor data
		if (xSemaphoreTake(g_sensor_data.mutex, pdMS_TO_TICKS(100)) == pdTRUE)
		{
			float temp = g_sensor_data.temperature;
			float humd = g_sensor_data.humidity;
			
			xSemaphoreGive(g_sensor_data.mutex);
			
			ESP_LOGI(TAG, "Temp: %.1f°C | Hum: %.1f%% | Free heap: %lu", temp, humd, esp_get_free_heap_size());
		}
		
		vTaskDelay(pdMS_TO_TICKS(5000));
	}
}
