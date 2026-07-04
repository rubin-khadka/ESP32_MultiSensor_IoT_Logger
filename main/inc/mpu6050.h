/*
 * mpu6050.h
 *
 *  Created on: Jul 3, 2026
 *      Author: Rubin Khadka
 */

#ifndef MAIN_INC_MPU6050_H_
#define MAIN_INC_MPU6050_H_

#include "esp_err.h"

#define MPU6050_DEV_ADDR		0x68

#define MPU6050_WHO_AM_I		0x75
#define MPU6050_PWR_MGMT1		0x6B
#define MPU6050_ACCEL_XOUT		0x3B
#define MPU6050_ACCEL_CONFIG	0x1C
#define MPU6050_GYRO_CONFIG		0x1B

// Data Structure
typedef struct
{
	float accel_x;
	float accel_y;
	float accel_z;
	float gyro_x;
	float gyro_y;
	float gyro_z;
} MPU6050_Data_t;

// Function Prototype
esp_err_t MPU6050_Init(void);
esp_err_t MPU6050_Read(MPU6050_Data_t *data);

#endif /* MAIN_INC_MPU6050_H_ */
