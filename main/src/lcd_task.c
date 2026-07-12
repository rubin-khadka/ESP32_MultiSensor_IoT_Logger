/*
 * lcd_task.c
 *
 *  Created on: Jul 11, 2026
 *      Author: Rubin Khadka
 */

#include "driver/gpio.h"
#include "sensor_data.h"
#include "lcd.h"
#include "esp_log.h"

static const char *TAG = "LCD_TASK";

// Button pin for mode switching (GPIO 0 = BOOT button on most ESP32)
#define BUTTON_PIN  GPIO_NUM_0

void lcd_task(void *pvParameters)
{
    uint8_t display_mode = 0;
    TickType_t last_wake_time = xTaskGetTickCount();
    static int last_button_state = 1;  // Pulled up, so default HIGH
    
    ESP_LOGI(TAG, "LCD Task started!");
    
    // Configure button with internal pull-up
    gpio_config_t btn_conf = {
        .pin_bit_mask = (1ULL << BUTTON_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&btn_conf);
    
    while (1) {
        // ── Button Check (with software debounce) ──
        int button_state = gpio_get_level(BUTTON_PIN);
        
        if (last_button_state == 1 && button_state == 0) {
            // Button pressed! Change mode
            display_mode = (display_mode + 1) % 4;
            LCD_Clear();
            ESP_LOGI(TAG, "Mode switched to: %d", display_mode);
            vTaskDelay(pdMS_TO_TICKS(200));  // Debounce delay
        }
        last_button_state = button_state;
        
        // ── Update Display ──
        if (xSemaphoreTake(g_sensor_data.mutex, pdMS_TO_TICKS(50)) == pdTRUE) {
            
            switch (display_mode) {
                case 0:  // DHT11 - Temperature & Humidity
					LCD_SetCursor(0, 0);
					char buf[17];
					sprintf(buf, "Date:%02d/%02d/20%02d",
					        g_sensor_data.day,
					        g_sensor_data.month,
					        g_sensor_data.year);
					LCD_SendString(buf);
	
					LCD_SetCursor(1, 0);
					sprintf(buf, "Time: %02d:%02d:%02d",
					        g_sensor_data.hours,
					        g_sensor_data.minutes,
					        g_sensor_data.seconds);
					LCD_SendString(buf);
                    break;
                    
                case 1:  // MPU6050 - Accelerometer
                    break;
                    
                case 2:  // MPU6050 - Gyroscope
                    break;
                    
                case 3:  // DS3231 - Date & Time
                    break;
            }
            
            xSemaphoreGive(g_sensor_data.mutex);
        }
        
        // Update at 10 Hz
        vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(100));
    }
}