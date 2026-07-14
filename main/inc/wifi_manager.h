/*
 * wifi_manager.h
 *
 *  Created on: Jul 14, 2026
 *      Author: Rubin Khadka
 */

#ifndef MAIN_INC_WIFI_MANAGER_H_
#define MAIN_INC_WIFI_MANAGER_H_

#include "esp_err.h"

// WiFi credentials - change these!
#define WIFI_SSID      "Mynoobu"
#define WIFI_PASSWORD  "Sarahmynoobu159!"

// Function prototype
esp_err_t wifi_manager_init(void);
esp_err_t wifi_manager_connect(void);
int wifi_manager_is_connected(void);
esp_err_t wifi_manager_get_ip(char *ip_buf, size_t buf_len);

#endif /* MAIN_INC_WIFI_MANAGER_H_ */
