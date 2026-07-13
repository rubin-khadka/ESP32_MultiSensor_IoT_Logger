/*
 * sd_card.c
 *
 *  Created on: Jul 13, 2026
 *      Author: Rubin Khadka
 */

#include "sd_card.h"
#include "driver/spi_common.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"

static const char *TAG = "SD_CARD";

#define PIN_MOSI GPIO_NUM_23
#define PIN_MISO GPIO_NUM_19
#define PIN_CLK GPIO_NUM_18
#define PIN_CS GPIO_NUM_5

esp_err_t SD_Card_Init(void)
{
    ESP_LOGI(TAG, "Initializing SD card (slow speed)...");
    
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = PIN_MOSI,
        .miso_io_num = PIN_MISO,
        .sclk_io_num = PIN_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };
    
    esp_err_t ret = spi_bus_initialize(SPI2_HOST, &bus_cfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPI init failed");
        return ret;
    }
    
    sdspi_device_config_t slot_cfg = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_cfg.gpio_cs = PIN_CS;
    slot_cfg.host_id = SPI2_HOST;
    
    esp_vfs_fat_sdmmc_mount_config_t mount_cfg = {
        .format_if_mount_failed = true,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };
    
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    host.slot = SPI2_HOST;
    host.max_freq_khz = 9000;  // Reliable communication test
    
    sdmmc_card_t *card;
    ret = esp_vfs_fat_sdspi_mount("/sdcard", &host, &slot_cfg, &mount_cfg, &card);
    
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Mount failed at 200kHz: %s", esp_err_to_name(ret));
        return ret;
    }
    
    sdmmc_card_print_info(stdout, card);
    ESP_LOGI(TAG, "SD card ready! (running at reduced speed)");
    return ESP_OK;
}

esp_err_t SD_Card_Log_Data(const char *datetime, float temp, float hum,
						   float ax, float ay, float az, float gx, float gy,
						   float gz) {
	FILE *f = fopen("/sdcard/sensor_data.csv", "a");
	if (f == NULL) {
		ESP_LOGE(TAG, "Failed to open file");
		return ESP_FAIL;
	}

	fseek(f, 0, SEEK_END);
	if (ftell(f) == 0) {
		fprintf(f, "DateTime,Temperature,Humidity,AccelX,AccelY,AccelZ,GyroX,"
				   "GyroY,GyroZ\n");
	}

	fprintf(f, "%s,%.1f,%.1f,%.3f,%.3f,%.3f,%.2f,%.2f,%.2f\n", datetime, temp,
			hum, ax, ay, az, gx, gy, gz);

	fclose(f);
	ESP_LOGI(TAG, "Data logged");
	return ESP_OK;
}

esp_err_t SD_Card_Read_All(void) {
	FILE *f = fopen("/sdcard/sensor_data.csv", "r");
	if (f == NULL) {
		printf("\n--- No data file ---\n");
		return ESP_FAIL;
	}

	printf("\n=== SD CARD DATA ===\n");
	char line[256];
	while (fgets(line, sizeof(line), f)) {
		printf("%s", line);
	}
	fclose(f);
	printf("====================\n\n");
	return ESP_OK;
}
