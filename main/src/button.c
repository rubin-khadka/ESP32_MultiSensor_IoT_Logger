/*
 * button.c
 *
 *  Created on: Jul 12, 2026
 *      Author: Rubin Khadka
 */

#include "button.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/idf_additions.h"
#include "freertos/task.h"

static const char *TAG = "BUTTON";

#define COOLDOWN_MS 1000
#define DEBOUNCE_MS 20

// Button states
static volatile uint8_t button1_pressed = 0;
static volatile uint8_t button2_pressed = 0;
static volatile uint8_t button3_pressed = 0;

// Cooldown timers
static volatile uint32_t button2_cooldown = 0;
static volatile uint32_t button3_cooldown = 0;

// Debounce timer handler
static TimerHandle_t debounce_timer = NULL;

// Current display mode
static volatile uint8_t current_mode = 0;

// Global flags
volatile uint8_t g_button2_pressed = 0;
volatile uint8_t g_button3_pressed = 0;

// Debounce timer callback
static void debounce_timer_callback(TimerHandle_t xTimer) {
	uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;

	// Button 1: Mode Switch
	if (button1_pressed) {
		if (gpio_get_level(BUTTON1_PIN) == 0) { // Still pressed?
			current_mode = (current_mode + 1) % 4;
			ESP_LOGI(TAG, "Mode: %d", current_mode);
		}
		button1_pressed = 0;
		gpio_intr_enable(BUTTON1_PIN); // Re-enable interrupt
	}

	// Button 2: Save
	if (button2_pressed) {
		if (gpio_get_level(BUTTON2_PIN) == 0) { // Still pressed?
			if (button2_cooldown == 0 || now >= button2_cooldown) {
				button2_cooldown = now + COOLDOWN_MS;
				g_button2_pressed = 1;
				ESP_LOGI(TAG, "Button 2: SAVE triggered!");
			} else {
				ESP_LOGI(TAG, "Button 2: Cooldown active");
			}
		}
		button2_pressed = 0;
		gpio_intr_enable(BUTTON2_PIN);
	}

	// Button 3: Read
	if (button3_pressed) {
		if (gpio_get_level(BUTTON3_PIN) == 0) { // Still pressed?
			if (button3_cooldown == 0 || now >= button3_cooldown) {
				button3_cooldown = now + COOLDOWN_MS;
				g_button3_pressed = 1;
				ESP_LOGI(TAG, "Button 3: READ triggered!");
			} else {
				ESP_LOGI(TAG, "Button 3: Cooldown active");
			}
		}
		button3_pressed = 0;
		gpio_intr_enable(BUTTON3_PIN);
	}
}

// GPIO Interrupt handler
static void IRAM_ATTR button_isr_handler(void *arg) {
	uint32_t gpio_num = (uint32_t)arg;

	// Disable interrupt
	gpio_intr_disable(gpio_num);

	// Set flag
	if (gpio_num == BUTTON1_PIN) {
		button1_pressed = 1;
	} else if (gpio_num == BUTTON2_PIN) {
		button2_pressed = 1;
	} else if (gpio_num == BUTTON3_PIN) {
		button3_pressed = 1;
	}

	// Start debounce timer
	xTimerResetFromISR(debounce_timer, NULL);
}

// Initialize Buttons
void Button_Init(void) {
	ESP_LOGI(TAG, "Initializing buttons...");

	// Configure all 3 buttons
	gpio_config_t btn_conf = {
		.mode = GPIO_MODE_INPUT,
		.pull_up_en = GPIO_PULLUP_ENABLE,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.intr_type = GPIO_INTR_NEGEDGE, // Falling edge
	};

	// Button 1: Mode switch
	btn_conf.pin_bit_mask = (1ULL << BUTTON1_PIN);
	gpio_config(&btn_conf);
	gpio_install_isr_service(0);
	gpio_isr_handler_add(BUTTON1_PIN, button_isr_handler, (void *)BUTTON1_PIN);

	// Button 2: Save
	btn_conf.pin_bit_mask = (1ULL << BUTTON2_PIN);
	gpio_config(&btn_conf);
	gpio_isr_handler_add(BUTTON2_PIN, button_isr_handler, (void *)BUTTON2_PIN);

	// Button 3: Read
	btn_conf.pin_bit_mask = (1ULL << BUTTON3_PIN);
	gpio_config(&btn_conf);
	gpio_isr_handler_add(BUTTON3_PIN, button_isr_handler, (void *)BUTTON3_PIN);

	// Create debounce timer
	debounce_timer = xTimerCreate("debounce",
								  pdMS_TO_TICKS(DEBOUNCE_MS), // 20ms
								  pdFALSE,					  // One-shot
								  NULL, debounce_timer_callback);

	ESP_LOGI(TAG, "Buttons initialized (EXTI + Timer debounce)");
}

uint8_t Button_GetMode(void) { return current_mode; }
