/*
 * wifi_task.c
 *
 *  Created on: Jul 14, 2026
 *      Author: Rubin Khadka
 */

#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "wifi_manager.h"

static const char *TAG = "WIFI_TASK";

void wifi_task(void *pvParameters) {
	char ip_address[16] = "0.0.0.0";

	ESP_LOGI(TAG, "WiFi Task started!");

	// Initialize WiFi
	wifi_manager_init();

	// Connect
	if (wifi_manager_connect() == ESP_OK) {
		wifi_manager_get_ip(ip_address, sizeof(ip_address));
		ESP_LOGI(TAG, "Connected! IP: %s", ip_address);
	} else {
		ESP_LOGE(TAG, "WiFi connection failed!");
	}

	// Monitor connection
	while (1) {
		if (wifi_manager_is_connected()) {
			wifi_manager_get_ip(ip_address, sizeof(ip_address));
			ESP_LOGI(TAG, "WiFi OK - IP: %s", ip_address);
		} else {
			ESP_LOGW(TAG, "WiFi disconnected!");
		}

		vTaskDelay(pdMS_TO_TICKS(30000)); // Check every 30 seconds
	}
}
