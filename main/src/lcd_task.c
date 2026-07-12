/*
 * lcd_task.c
 *
 *  Created on: Jul 11, 2026
 *      Author: Rubin Khadka
 */

#include "button.h"
#include "esp_log.h"
#include "lcd.h"
#include "sensor_data.h"

static const char *TAG = "LCD_TASK";

void lcd_task(void *pvParameters) 
{
	TickType_t last_wake_time = xTaskGetTickCount();

	ESP_LOGI(TAG, "LCD Task started!");

	while (1) 
	{
		uint8_t display_mode = Button_GetMode(); // Get mode from button module	

		if (xSemaphoreTake(g_sensor_data.mutex, pdMS_TO_TICKS(50)) == pdTRUE) {

			switch (display_mode) {
			case 0: // Temperature and Humidity
				LCD_SetCursor(0, 0);
				char buf[17];
				sprintf(buf, "TEMP: %.1f C", g_sensor_data.temperature);
				LCD_SendString(buf);
				LCD_SetCursor(1, 0);
				sprintf(buf, "HUMD: %.1f %%", g_sensor_data.humidity);
				LCD_SendString(buf);
				break;

			case 1: // Accelerometer
				LCD_SetCursor(0, 0);
				char buf2[17];
				sprintf(buf2, "AX:%.2f AY:%.2f", g_sensor_data.accel_x,
						g_sensor_data.accel_y);
				LCD_SendString(buf2);
				LCD_SetCursor(1, 0);
				sprintf(buf2, "AZ:%.2f [g]", g_sensor_data.accel_z);
				LCD_SendString(buf2);
				break;

			case 2:	// Gyroscope
				LCD_SetCursor(0, 0);
				char buf3[17];
				sprintf(buf3, "GX:%.1f GY:%.1f", g_sensor_data.gyro_x,
						g_sensor_data.gyro_y);
				LCD_SendString(buf3);
				LCD_SetCursor(1, 0);
				sprintf(buf3, "GZ:%.1f [dps]", g_sensor_data.gyro_z);
				LCD_SendString(buf3);
				break;
	
			case 3:	// Real time clock
				LCD_SetCursor(0, 0);
				char buf4[17];
				sprintf(buf4, "%02d/%02d/20%02d", g_sensor_data.day,
						g_sensor_data.month, g_sensor_data.year);
				LCD_SendString(buf4);
				LCD_SendString(" ");
				LCD_SendString(" ");
				LCD_SendString(" ");
				LCD_SendString(" ");
				LCD_SendString(" ");
				LCD_SendString(" ");
				LCD_SetCursor(1, 0);
				sprintf(buf4, "%02d:%02d:%02d", g_sensor_data.hours,
						g_sensor_data.minutes, g_sensor_data.seconds);
				LCD_SendString(buf4);
				LCD_SendString(" ");
				LCD_SendString(" ");
				LCD_SendString(" ");
				LCD_SendString(" ");
				LCD_SendString(" ");
				LCD_SendString(" ");
				LCD_SendString(" ");
				LCD_SendString(" ");
				break;
			}

			xSemaphoreGive(g_sensor_data.mutex);
		}

		vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(100));
	}
}
