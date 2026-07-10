/*
 * sensor_data.h
 *
 *  Created on: Jul 10, 2026
 *      Author: Rubin Khadka
 */

#ifndef MAIN_INC_SENSOR_DATA_H_
#define MAIN_INC_SENSOR_DATA_H_

#include "freertos/idf_additions.h"

typedef struct
{
	// DHT11
	float temperature;
	float humidity;
	
	// MPU6050
	float accel_x, accel_y, accel_z;
	float gyro_x, gyro_y, gyro_z;
	
	// DS3231
	int hours, minutes, seconds;
	int day, month, year;
	
	// Protection
	SemaphoreHandle_t mutex;
} sensor_data_t;

extern sensor_data_t g_sensor_data;

#endif /* MAIN_INC_SENSOR_DATA_H_ */
