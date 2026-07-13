/*
 * sd_card.h
 *
 *  Created on: Jul 13, 2026
 *      Author: Rubin Khadka
 */

#ifndef MAIN_INC_SD_CARD_H_
#define MAIN_INC_SD_CARD_H_

#include "esp_err.h"

esp_err_t SD_Card_Init(void);
esp_err_t SD_Card_Log_Data(const char *datetime, float temp, float hum,
						   float ax, float ay, float az, float gx, float gy,
						   float gz);
esp_err_t SD_Card_Read_All(void);

#endif /* MAIN_INC_SD_CARD_H_ */
