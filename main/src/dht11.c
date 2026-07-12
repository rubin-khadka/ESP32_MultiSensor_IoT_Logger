/*
 * dht11.c
 *
 *  Created on: Jul 2, 2026
 *      Author: Rubin Khadka
 */

#include "dht11.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_rom_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "hal/gpio_types.h"
#include <stdint.h>

static const char *TAG = "DHT11";
static gpio_num_t dht11_pin;

// Pin Macros
#define DHT11_HIGH() 		gpio_set_level(dht11_pin, 1)
#define DHT11_LOW() 		gpio_set_level(dht11_pin, 0)
#define DHT11_READ_PIN() 	gpio_get_level(dht11_pin)

// Initialize DHT11 sensor
void DHT11_Init(gpio_num_t gpio_num)
{
	dht11_pin = gpio_num;
	
	// Configure as output, start HIGH
	gpio_config_t io_conf = 
	{
		.pin_bit_mask = (1UL << dht11_pin),
		.mode = GPIO_MODE_INPUT_OUTPUT_OD,
		.pull_up_en = GPIO_PULLUP_ENABLE,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.intr_type = GPIO_INTR_DISABLE
	};
	gpio_config(&io_conf);
	
	DHT11_HIGH();
	ESP_LOGI(TAG, "Initialized on GPIO %d", dht11_pin);
}

// Start DHT11 sensor
void DHT11_Start(void)
{
	// Set as output
	gpio_set_direction(dht11_pin, GPIO_MODE_OUTPUT);
	
	// Pull low for 18ms
	DHT11_LOW();
	esp_rom_delay_us(18000);
	
	// Pull high for 20 microsecond
	DHT11_HIGH();
	esp_rom_delay_us(20);
	
	// Set as input
	gpio_set_direction(dht11_pin, GPIO_MODE_INPUT);
}

// Check response of the sensor
int DHT11_Check_Response(void)
{
	esp_rom_delay_us(40);
	
	// Check if sensor is pulled low
	if (!DHT11_READ_PIN()) 
	{
		esp_rom_delay_us(80); // wait 80 microsecond , low pulse
		
		if (DHT11_READ_PIN()) // Should be high now
		{
			// Wait for high to end
			uint32_t timeout = 500;
			while (DHT11_READ_PIN()) 
			{
				if (--timeout == 0) 
				{
					return 0;
				}
			}
			return 1;	// Response OK
		}
	}
	return 0;	// No response
}

// Read the sensor data
uint8_t DHT11_Read(void)
{
	uint8_t data = 0;
	uint32_t timeout = 0;
	
	for (int bit =7; bit >= 0; bit--) 
	{
		// Wait for pin to go HIGH
		timeout = 500;
		while (!DHT11_READ_PIN())
		{
			if(--timeout == 0)
			{
				return 0;
			}
		}
		
		// Wait 40 microsecond into the high pulse
		esp_rom_delay_us(40);
		
		// Check if pin is still high
		if (DHT11_READ_PIN()) 
		{
			// Still high: bit is 1
			data |= (1 << bit);
			
			// Wait for high pulse to end
			timeout = 500;
			while (DHT11_READ_PIN()) 
			{
				if (--timeout == 0) 
				{
					return 0;
				}
			}
		}
		
		// If pin is low: bit is 0
	}
	
	return data;
}

// Process the read data
esp_err_t DHT11_Process(dht11_data_t *data)
{
	uint8_t humidity_int = 0;
	uint8_t humidity_dec = 0;
	uint8_t temperature_int = 0;
	uint8_t temperature_dec = 0;
	uint8_t checksum = 0;
	
	// Critical section
	vTaskSuspendAll();
	
	// Start the sensor signal
	DHT11_Start();
	
	// Check Response from the sensor
	if (!DHT11_Check_Response()) 
	{
		xTaskResumeAll();
		ESP_LOGE(TAG, "Sensor not responding !!!");
		return ESP_ERR_TIMEOUT;
	}
	
	// Read 5 Bytes
	humidity_int 	= DHT11_Read();
	humidity_dec 	= DHT11_Read();
	temperature_int = DHT11_Read();
	temperature_dec = DHT11_Read();
	checksum 		= DHT11_Read();
	
	// Re-enable all task 
	xTaskResumeAll();
	
	// Verify Checksum
	uint16_t sum = humidity_int + humidity_dec + temperature_int + temperature_dec;
	if (sum != checksum)
	{
		ESP_LOGE(TAG, "Checksum error: calc=%d, recv=%d", checksum, sum);	
		return ESP_ERR_INVALID_CRC;
	}
	
	// Fill data
	data->humidity = (float)humidity_int + ((float)humidity_dec / 10.0f);
	
	if (temperature_int & 0x80) {
		temperature_int &= 0x7F;
		data->temperature = -((float)temperature_int + ((float)temperature_dec / 10.0f));
	} else {
		data->temperature = (float)temperature_int + ((float)temperature_dec / 10.0f);
	}
	
	ESP_LOGI(TAG, "Temp: %.1f C, Hum: %.1f %%", data->temperature, data->humidity);
	
	return ESP_OK;
}

