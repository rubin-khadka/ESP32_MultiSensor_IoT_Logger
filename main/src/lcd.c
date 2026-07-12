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
    uint8_t data = nibble | LCD_BACKLIGHT;
    if (rs) data |= LCD_RS;
    
    // Use I2C_BUS_0 or I2C_BUS_1 depending on which bus your LCD is on
    I2C_Write_Byte(I2C_BUS_0, LCD_ADDR, data | LCD_ENABLE);
    esp_rom_delay_us(10);
    
    I2C_Write_Byte(I2C_BUS_0, LCD_ADDR, data & ~LCD_ENABLE);
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
void LCD_Init(void)
{
	ESP_LOGI(TAG, "Initializing LCD ...");
	
	// Power up delay
	vTaskDelay(pdMS_TO_TICKS(100));
	
	// Reset Sequence
	LCD_SendNibble(0x30, 0x00);
	vTaskDelay(pdMS_TO_TICKS(5));
	
	LCD_SendNibble(0x30, 0x00);
	vTaskDelay(pdMS_TO_TICKS(5));
	
	LCD_SendNibble(0x30, 0x00);
	vTaskDelay(pdMS_TO_TICKS(5));
	
	LCD_SendNibble(0x20, 0x00);		// Switch to 4 bit mode
	vTaskDelay(pdMS_TO_TICKS(5));
	
	// Now in 4 bit mode
	LCD_SendCmd(0x28);		// 2 lines, 5x8 font
	LCD_SendCmd(0x08);		// Display off
	LCD_SendCmd(0x01);		// Clear display
	LCD_SendCmd(0x06);		// Entry mode
	LCD_SendCmd(0x0C);		// Display on, Cursor off
	
	ESP_LOGI(TAG, "LCD Initialized Successfully !!!");
}

// Send String
void LCD_SendString(char *str)
{
	while (*str)
	{
		LCD_SendData(*str++);
	}
}

// LCD Clear
void LCD_Clear(void)
{
	LCD_SendCmd(0x01);
}

// Set Cursor
void LCD_SetCursor(uint8_t row, uint8_t col)
{
	uint8_t address;
	
	if (row == 0)
	{
		address = 0x80 + col;
	} else
	{
		address = 0xC0 + col;
	}
	LCD_SendCmd(address);
}
