/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_log.h"

#include "WavPlayer.h"
#include "AudioFiles.h"
#include "BtAudio.h"
#include "AudioHandler.h"

#include "Config.h"

extern "C" {

#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "spiffs_vfs.h"

const char *MAIN_TAG = "MainLoop";

WavPlayer wavPlayer(PIN_I2S_BCLK, PIN_I2S_LRC, PIN_I2S_DATA, I2S_NUM_0);
uint8_t uartBuffer[256];

void initUart();

void app_main()
{
	ESP_LOGI(MAIN_TAG, "Init");

    // Initialise NVS.
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	ESP_LOGI(MAIN_TAG, "Init SPIFFS");
	vfs_spiffs_register();

	ESP_LOGI(MAIN_TAG, "Init BT");
	initBtAudio("Warp Core");

	ESP_LOGI(MAIN_TAG, "Init audio");
	wavPlayer.init();

	ESP_LOGI(MAIN_TAG, "Init UART");
	initUart();

    ESP_LOGI(MAIN_TAG, "Init complete");

    while (true)
	{
    	int len = uart_read_bytes(UART_NUM_1, uartBuffer, 1, portMAX_DELAY);
    	if (len <= 0)
    	{
    		ESP_LOGW(MAIN_TAG, "No data received from UART");
    		continue;
    	}

    	executeCommand(uartBuffer[0], &wavPlayer);
	}
}

void initUart()
{
	uart_config_t uart_config = {};
	uart_config.baud_rate = 115200,
	uart_config.data_bits = UART_DATA_8_BITS;
	uart_config.parity    = UART_PARITY_DISABLE;
	uart_config.stop_bits = UART_STOP_BITS_1;
	uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
	uart_param_config(UART_NUM_1, &uart_config);
	uart_set_pin(UART_NUM_1, PIN_UART_TX, PIN_UART_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
	uart_driver_install(UART_NUM_1, 256 * 2, 0, 0, NULL, 0);
}

}
