/*
 * i2c.helper.h
 *
 *  Created on: Jul 4, 2026
 *      Author: Rubin Khadka
 */

#ifndef MAIN_INC_I2C_HELPER_H_
#define MAIN_INC_I2C_HELPER_H_

#include "esp_err.h"

#define I2C_BUS		I2C_NUM_0
#define I2C_FREQ 	100000

esp_err_t I2C_Init(int scl_pin, int sda_pin);
esp_err_t I2C_Read_Reg(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data);
esp_err_t I2C_Write_Reg(uint8_t dev_addr, uint8_t reg_addr, uint8_t data);
esp_err_t I2C_Read_Brust(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, size_t len);

#endif /* MAIN_INC_I2C_HELPER_H_ */
