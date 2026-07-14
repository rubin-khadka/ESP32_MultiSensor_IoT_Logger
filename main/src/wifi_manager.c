/*
 * wifi_manager.c
 *
 *  Created on: Jul 14, 2026
 *      Author: Rubin Khadka
*/

#include "wifi_manager.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "nvs_flash.h"

static const char *TAG = "WIFI";

// FreeRTOS event group for WiFi status
static EventGroupHandle_t wifi_event_group;
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

static int s_retry_count = 0;
#define MAX_RETRY 5

// WiFi Event Handler
static void wifi_event_handler(void *arg, esp_event_base_t event_base,
							   int32_t event_id, void *event_data) {
	if (event_base == WIFI_EVENT) {
		switch (event_id) {
		case WIFI_EVENT_STA_START:
			ESP_LOGI(TAG, "WiFi started, connecting...");
			esp_wifi_connect();
			break;

		case WIFI_EVENT_STA_DISCONNECTED:
			if (s_retry_count < MAX_RETRY) {
				ESP_LOGW(TAG, "Disconnected, retrying (%d/%d)...",
						 s_retry_count + 1, MAX_RETRY);
				esp_wifi_connect();
				s_retry_count++;
			} else {
				ESP_LOGE(TAG, "Max retries reached!");
				xEventGroupSetBits(wifi_event_group, WIFI_FAIL_BIT);
			}
			break;
		}
	} else if (event_base == IP_EVENT) {
		switch (event_id) {
		case IP_EVENT_STA_GOT_IP: {
			ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
			ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
			s_retry_count = 0;
			xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
			break;
		}
		}
	}
}

// Initialize WiFi
esp_err_t wifi_manager_init(void) {
	ESP_LOGI(TAG, "Initializing WiFi...");

	// Create event group
	wifi_event_group = xEventGroupCreate();

	// Initialize NVS (needed for WiFi)
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
		ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		nvs_flash_erase();
		nvs_flash_init();
	}

	// Initialize TCP/IP stack
	esp_netif_init();

	// Create event loop
	esp_event_loop_create_default();

	// Create WiFi station
	esp_netif_create_default_wifi_sta();

	// Configure WiFi
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	esp_wifi_init(&cfg);

	// Register event handlers
	esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
							   &wifi_event_handler, NULL);
	esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
							   &wifi_event_handler, NULL);

	// Configure WiFi connection
	wifi_config_t wifi_config = {
		.sta =
			{
				.ssid = WIFI_SSID,
				.password = WIFI_PASSWORD,
			},
	};

	esp_wifi_set_mode(WIFI_MODE_STA);
	esp_wifi_set_config(WIFI_IF_STA, &wifi_config);

	ESP_LOGI(TAG, "WiFi initialized (SSID: %s)", WIFI_SSID);
	return ESP_OK;
}

// Connect to WiFi
esp_err_t wifi_manager_connect(void) {
	ESP_LOGI(TAG, "Starting WiFi connection...");

	esp_wifi_start();

	// Wait for connection or failure
	EventBits_t bits = xEventGroupWaitBits(
		wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdFALSE, pdFALSE,
		pdMS_TO_TICKS(30000)); // 30 sec timeout

	if (bits & WIFI_CONNECTED_BIT) {
		ESP_LOGI(TAG, "WiFi connected successfully!");
		return ESP_OK;
	} else if (bits & WIFI_FAIL_BIT) {
		ESP_LOGE(TAG, "WiFi connection failed!");
		return ESP_FAIL;
	} else {
		ESP_LOGE(TAG, "WiFi connection timed out!");
		return ESP_ERR_TIMEOUT;
	}
}

// Check Connection Status
int wifi_manager_is_connected(void) {
	EventBits_t bits = xEventGroupGetBits(wifi_event_group);
	return (bits & WIFI_CONNECTED_BIT) ? 1 : 0;
}

// Get IP Address
esp_err_t wifi_manager_get_ip(char *ip_buf, size_t buf_len) {
	esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
	esp_netif_ip_info_t ip_info;

	if (esp_netif_get_ip_info(netif, &ip_info) == ESP_OK) {
		snprintf(ip_buf, buf_len, IPSTR, IP2STR(&ip_info.ip));
		return ESP_OK;
	}
	return ESP_FAIL;
}
