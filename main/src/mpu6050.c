/*
 * mpu6050.c
 *
 *  Created on: Jul 3, 2026
 *      Author: Rubin Khadka
 */

#include "mpu6050.h"
#include "i2c.helper.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"   
#include "freertos/task.h"       
#include <stdint.h>

static const char *TAG = "MPU6050";

esp_err_t MPU6050_Init(void)
{
    ESP_LOGI(TAG, "Initializing MPU6050 ...");
    uint8_t who_am_i = 0;
    
    // Read who_am_i register to verify chip
    if (I2C_Read_Reg(MPU6050_DEV_ADDR, MPU6050_WHO_AM_I, &who_am_i) != ESP_OK) 
    {
        ESP_LOGE(TAG, "Failed to read WHO_AM_I !!!");
        return ESP_FAIL;
    }   
    
    ESP_LOGI(TAG, "Device ID: 0x%02X", who_am_i);
    
    if (who_am_i != MPU6050_DEV_ADDR) 
    {
        ESP_LOGE(TAG, "Wrong Device ID !!!");
        return ESP_FAIL;
    }
    
    // Wake up MPU6050
    if (I2C_Write_Reg(MPU6050_DEV_ADDR, MPU6050_PWR_MGMT1, 0x00) != ESP_OK)  
    {
        ESP_LOGE(TAG, "Failed to wake up MPU6050 !!!");
        return ESP_FAIL;
    }
    
    // Configure Accelerometer
    if (I2C_Write_Reg(MPU6050_DEV_ADDR, MPU6050_ACCEL_CONFIG, 0x00) != ESP_OK) 
    {
        ESP_LOGE(TAG, "Failed to configure Accelerometer !!!");
        return ESP_FAIL;
    }
    
    // Configure Gyroscope
    if (I2C_Write_Reg(MPU6050_DEV_ADDR, MPU6050_GYRO_CONFIG, 0x00) != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to configure Gyroscope !!!");
        return ESP_FAIL;
    }
    
    ESP_LOGI(TAG, "MPU6050 initialized successfully !!!");
    return ESP_OK;
}

esp_err_t MPU6050_Read(MPU6050_Data_t *data)
{
    uint8_t raw_data[14];
    
    // Burst read 14 byte starting from accel_x
    if (I2C_Read_Brust(MPU6050_DEV_ADDR, MPU6050_ACCEL_XOUT, raw_data, 14) != ESP_OK) 
    {
        ESP_LOGE(TAG, "Failed to read sensor data !!!");
        return ESP_FAIL;
    }
    
    // Raw accelerometer data
    int16_t accel_x_raw = (raw_data[0] << 8) | raw_data[1];
    int16_t accel_y_raw = (raw_data[2] << 8) | raw_data[3];
    int16_t accel_z_raw = (raw_data[4] << 8) | raw_data[5];
    
    // Raw gyroscope data
    int16_t gyro_x_raw = (raw_data[8] << 8) | raw_data[9];
    int16_t gyro_y_raw = (raw_data[10] << 8) | raw_data[11];
    int16_t gyro_z_raw = (raw_data[12] << 8) | raw_data[13];
    
    data->accel_x = accel_x_raw / 16384.0f;
    data->accel_y = accel_y_raw / 16384.0f;
    data->accel_z = accel_z_raw / 16384.0f;
    
    data->gyro_x = gyro_x_raw / 131.0f;
    data->gyro_y = gyro_y_raw / 131.0f;
    data->gyro_z = gyro_z_raw / 131.0f;
    
    ESP_LOGI(TAG, "Accel: X=%.2f Y=%.2f Z=%.2f | Gyro: X=%.1f Y=%.1f Z=%.1f",
             data->accel_x, data->accel_y, data->accel_z,
             data->gyro_x, data->gyro_y, data->gyro_z);

    return ESP_OK;
}
