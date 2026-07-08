/*
 * lcd.c
 *
 *  Created on: Jul 7, 2026
 *      Author: Rubin Khadka
 */

#include "lcd.h"
#include "esp_rom_sys.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "i2c.helper.h"
#include "esp_log.h"
#include <stdint.h>
 
static const char *TAG = "LCD";
 
// Send nibble
static void LCD_SendNibble(uint8_t nibble, uint8_t rs)
{
	uint8_t data;
	
	data = nibble | LCD_BACKLIGHT;	// Backlight always on
	
	if (rs)
	{
		data |= LCD_RS;		// RS = 1 for data
	}
	
	// Enable High
	I2C_Write_Reg(LCD_ADDR, data | LCD_ENABLE, 0x00);		// Dummy Write
	esp_rom_delay_us(1); 	// Small delay
	
	// Enable Low
	I2C_Write_Reg(LCD_ADDR, data & ~LCD_ENABLE, 0x00);
	esp_rom_delay_us(50);
}
 
// Send Command
void LCD_SendCmd(uint8_t cmd)
{
	// Send high nibble first
	LCD_SendNibble(cmd & 0xF0, 0);
	
	// Send low nibble
	LCD_SendNibble((cmd << 4) & 0xF0, 0);
	
	// Commands need execution time
	if (cmd == 0x01 || cmd == 0x02)
	{
		vTaskDelay(pdMS_TO_TICKS(20));	// 20 ms
	} else {
		vTaskDelay(pdMS_TO_TICKS(1));	// 1 ms
	}
}

// Send data
void LCD_SendData(uint8_t data)
{
	// Send high nibble first
	LCD_SendNibble(data & 0xF0, 1);
	
	// Send low nibble
	LCD_SendNibble((data << 4) & 0xF0, 1);
	
	vTaskDelay(pdMS_TO_TICKS(1));
}

// Initialize LCD

