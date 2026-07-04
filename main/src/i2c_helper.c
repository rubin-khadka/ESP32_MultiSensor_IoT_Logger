/*
 * i2c_helper.c
 *
 *  Created on: Jul 4, 2026
 *      Author: Rubin Khadka
 */

#include "driver/i2c.h"
#include "driver/i2c_types_legacy.h"
#include "esp_err.h"
#include "freertos/projdefs.h"
#include "hal/gpio_types.h"
#include "hal/i2c_types.h"
#include "i2c.helper.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stddef.h>
#include <stdint.h>

static const char *TAG = "I2C_HELPER";

esp_err_t I2C_Init(int scl_pin, int sda_pin)
{
	ESP_LOGI(TAG, "Initializing I2C bus");
	ESP_LOGI(TAG, "SCL: %d, SDA: %d", scl_pin, sda_pin);
	
	i2c_config_t i2c_conf =
	{
		.mode = I2C_MODE_MASTER,
		.sda_io_num	= sda_pin,
		.scl_io_num	= scl_pin,
		.sda_pullup_en	= GPIO_PULLUP_ENABLE,
		.scl_pullup_en	= GPIO_PULLUP_ENABLE,
		.master.clk_speed = I2C_FREQ
	};
	
	i2c_param_config(I2C_BUS, &i2c_conf);
	return i2c_driver_install(I2C_BUS, I2C_MODE_MASTER, 0, 0, 0);
}

esp_err_t I2C_Read_Reg(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data)
{
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	
	// Send device address and register to read
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_WRITE, true);
	i2c_master_write_byte(cmd, reg_addr, true);
	
	// Restart and read
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_READ, true);
	i2c_master_read_byte(cmd, data, I2C_MASTER_NACK);
	i2c_master_stop(cmd);
	
	esp_err_t ret = i2c_master_cmd_begin(I2C_BUS, cmd, pdMS_TO_TICKS(100));
	i2c_cmd_link_delete(cmd);
	return ret;
}

esp_err_t I2C_Write_Reg(uint8_t dev_addr, uint8_t reg_addr, uint8_t data)
{
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_WRITE, true);
	i2c_master_write_byte(cmd, reg_addr, true);
	i2c_master_write_byte(cmd, data, true);
	i2c_master_stop(cmd);
	
	esp_err_t ret = i2c_master_cmd_begin(I2C_BUS, cmd, pdMS_TO_TICKS(100));
	i2c_cmd_link_delete(cmd);
	return ret;
}

esp_err_t I2C_Read_Brust(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, size_t len)
{
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	
	// send device id and starting register
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_WRITE, true);
	i2c_master_write_byte(cmd, reg_addr, true);
	
	// Restart and read multiple bytes
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_READ, true);
	i2c_master_read(cmd, data, len, I2C_MASTER_LAST_NACK);
	i2c_master_stop(cmd);

	esp_err_t ret = i2c_master_cmd_begin(I2C_BUS, cmd, pdMS_TO_TICKS(100));
	i2c_cmd_link_delete(cmd);
	return ret;
}



