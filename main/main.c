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
#include "i2c.helper.h"
#include "mpu6050.h"

static const char *TAG = "MAIN";

void dht11_task(void *pvParameters);
void mpu6050_task(void *pvParameters);

sensor_data_t g_sensor_data;

void app_main(void)
{
	ESP_LOGI(TAG, "ESP32 Multi-Sensor IoT Logger");
	
	// Create Mutex
	g_sensor_data.mutex = xSemaphoreCreateMutex();
	ESP_LOGI(TAG, "Mutex Created !!");
	
	// Initialize I2C Bus
	I2C_Init(GPIO_NUM_22, GPIO_NUM_21);
	ESP_LOGI(TAG, "I2C Initialized !!!");
	
	// Initialize Sensors
	MPU6050_Init();
	DHT11_Init(GPIO_NUM_4);
	ESP_LOGI(TAG, "DHT11 and MPU6050 Initialized !!!");
	
	// Create tasks
	xTaskCreate(dht11_task, "dht11_task", 4096, NULL, 2, NULL);
	xTaskCreate(mpu6050_task, "mpu6050_task", 4096, NULL, 3, NULL);
	
	ESP_LOGI(TAG, "DHT11 and MPU6050 Tasks created successfully !!!");
	ESP_LOGI(TAG, "Scheduler Running !!!");
	
	while(1)
	{
		// Read sensor data
		if (xSemaphoreTake(g_sensor_data.mutex, pdMS_TO_TICKS(100)) == pdTRUE)
		{
			float temp = g_sensor_data.temperature;
			float humd = g_sensor_data.humidity;
			float ax = g_sensor_data.accel_x;
			float ay = g_sensor_data.accel_y;
			float az = g_sensor_data.accel_z;
			
			xSemaphoreGive(g_sensor_data.mutex);
		}
		
		vTaskDelay(pdMS_TO_TICKS(5000));
	}
}
