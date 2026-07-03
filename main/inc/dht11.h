/*
 * dht11.h
 *
 *  Created on: Jul 2, 2026
 *      Author: Rubin Khadka
 */

#ifndef MAIN_INC_DHT11_H_
#define MAIN_INC_DHT11_H_

#include <driver/gpio.h>
#include <esp_err.h>

typedef struct 
{
	float temperature;
	float humidity;
}dht11_data_t;

// Function prototypes
void DHT11_Init(gpio_num_t gpio_num);
void DHT11_Start(void);
int DHT11_Check_Response(void);
uint8_t DHT11_Read(void);
esp_err_t DHT11_Process(dht11_data_t *data);

#endif /* MAIN_INC_DHT11_H_ */
