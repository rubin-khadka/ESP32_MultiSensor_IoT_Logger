/*
 * i2c.helper.h
 *
 *  Created on: Jul 4, 2026
 *      Author: Rubin Khadka
 */

#ifndef MAIN_INC_I2C_HELPER_H_
#define MAIN_INC_I2C_HELPER_H_

#include "esp_err.h"
#include "hal/i2c_types.h"

#define I2C_BUS_0	I2C_NUM_0
#define I2C_BUS_1	I2C_NUM_1

#define I2C_FREQ 	100000

// Initialize a specific I2C bus
esp_err_t I2C_Bus_Init(i2c_port_t bus_num, int scl_pin, int sda_pin);

// Read/write on a specific bus
esp_err_t I2C_Read_Reg(i2c_port_t bus, uint8_t dev_addr, uint8_t reg_addr, uint8_t *data);
esp_err_t I2C_Write_Reg(i2c_port_t bus, uint8_t dev_addr, uint8_t reg_addr, uint8_t data);
esp_err_t I2C_Read_Brust(i2c_port_t bus, uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, size_t len);

#endif /* MAIN_INC_I2C_HELPER_H_ */
