#include "freertos/FreeRTOS.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "dht11.h"
#include "lcd.h"
#include "portmacro.h"
#include "sensor_data.h"
#include "soc/gpio_num.h"
#include "i2c.helper.h"
#include "mpu6050.h"
#include "ds3231.h"

static const char *TAG = "MAIN";

void dht11_task(void *pvParameters);
void mpu6050_task(void *pvParameters);
void ds3231_task(void *pvParameters);
void lcd_task(void *pvParameters);

sensor_data_t g_sensor_data;

void app_main(void)
{
	ESP_LOGI(TAG, "ESP32 Multi-Sensor IoT Logger");
	
	// Create Mutex
	g_sensor_data.mutex = xSemaphoreCreateMutex();
	ESP_LOGI(TAG, "Mutex Created !!");
	
	// Initialize I2C Bus
	I2C_Bus_Init(I2C_BUS_0, GPIO_NUM_22, GPIO_NUM_21);  // MPU6050 + LCD
	I2C_Bus_Init(I2C_BUS_1, GPIO_NUM_19, GPIO_NUM_18);  // DS3231
	ESP_LOGI(TAG, "I2C Initialized !!!");
	
	// Initialize Sensors
	MPU6050_Init();
	DHT11_Init(GPIO_NUM_4);
	DS3231_Init();
	LCD_Init();
	ESP_LOGI(TAG, "Sensors Initialized !!!");
	
	// Create tasks
	xTaskCreate(dht11_task, "dht11_task", 4096, NULL, 2, NULL);
	xTaskCreate(mpu6050_task, "mpu6050_task", 4096, NULL, 3, NULL);
	xTaskCreate(ds3231_task, "ds3231_task", 4096, NULL, 1, NULL);
	xTaskCreate(lcd_task, "lcd_task", 4096, NULL, 1, NULL);
	ESP_LOGI(TAG, "All tasks created! Scheduler Running !!!");
	
	while(1)
	{
		if (xSemaphoreTake(g_sensor_data.mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
		    ESP_LOGI(TAG, "T:%.1f°C H:%.1f%% | AccZ:%.2f | %02d:%02d:%02d | Heap:%lu",
		             g_sensor_data.temperature,
		             g_sensor_data.humidity,
		             g_sensor_data.accel_z,
		             g_sensor_data.hours,
		             g_sensor_data.minutes,
		             g_sensor_data.seconds,
		             esp_get_free_heap_size());
		    xSemaphoreGive(g_sensor_data.mutex);
		}

		vTaskDelay(pdMS_TO_TICKS(5000));
	}
}
