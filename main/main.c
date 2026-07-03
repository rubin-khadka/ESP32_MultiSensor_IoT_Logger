
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "dht11.h"
#include "soc/gpio_num.h"

static const char *TAG = "MAIN";

void app_main(void)
{
	ESP_LOGI(TAG, "DHT11 Test Starting ...");
	
	dht11_data_t sensor_data;
	DHT11_Init(GPIO_NUM_4);
	
	while(1)
	{
		if (DHT11_Process(&sensor_data) == ESP_OK)
		{
			ESP_LOGI(TAG, "Temp: %.1f C, Hum: %.1f %%", sensor_data.temperature, sensor_data.humidity);
		}
		vTaskDelay(pdMS_TO_TICKS(2000));
	}
}
