/*------------------------------------------------------------------------------
	DHT22 CPP temperature & humidity sensor AM2302 (DHT22) driver for ESP32
	Jun 2017:	Ricardo Timmermann, new for DHT22

	This example code is in the Public Domain (or CC0 licensed, at your option.)
	Unless required by applicable law or agreed to in writing, this
	software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
	CONDITIONS OF ANY KIND, either express or implied.
	PLEASE KEEP THIS CODE IN LESS THAN 0XFF LINES. EACH LINE MAY CONTAIN ONE BUG !!!
---------------------------------------------------------------------------------*/

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_rom_sys.h"
#include "DHT.h"


#define DHT_TAG "DHT"

DHT::DHT() {}

void DHT::setDHTgpio(gpio_num_t gpio) {
    DHTgpio = gpio;
    gpio_set_direction(DHTgpio, GPIO_MODE_INPUT_OUTPUT);
}

int DHT::getSignalLevel(int usTimeOut, bool state) {
    int uSec = 0;
    while (gpio_get_level(DHTgpio) == state) {
        if (uSec > usTimeOut) return -1;
        ++uSec;
        esp_rom_delay_us(1);
    }
    return uSec;
}

void DHT::errorHandler(int response) {
    switch (response) {
        case DHT_TIMEOUT_ERROR:
            ESP_LOGE(DHT_TAG, "Sensor Timeout\n");
            break;
        case DHT_CHECKSUM_ERROR:
            ESP_LOGE(DHT_TAG, "CheckSum error\n");
            break;
        case DHT_OK:
            break;
        default:
            ESP_LOGE(DHT_TAG, "Unknown error\n");
    }
}

int DHT::readDHT() {
    int uSec = 0;
    uint8_t data[5] = {0, 0, 0, 0, 0};

    gpio_set_direction(DHTgpio, GPIO_MODE_OUTPUT);
    gpio_set_level(DHTgpio, 0);
    //vTaskDelay(pdMS_TO_TICKS(20));
    vTaskDelay(20 / portTICK_PERIOD_MS);
    gpio_set_level(DHTgpio, 1);
    esp_rom_delay_us(40);
    gpio_set_direction(DHTgpio, GPIO_MODE_INPUT);

    uSec = getSignalLevel(85, 0);
    if (uSec < 0) return DHT_TIMEOUT_ERROR;

    uSec = getSignalLevel(85, 1);
    if (uSec < 0) return DHT_TIMEOUT_ERROR;

    for (int k = 0; k < 40; k++) {
        uSec = getSignalLevel(56, 0);
        if (uSec < 0) return DHT_TIMEOUT_ERROR;

        uSec = getSignalLevel(75, 1);
        if (uSec < 0) return DHT_TIMEOUT_ERROR;

        if (uSec > 40) {
            data[k / 8] |= (1 << (7 - (k % 8)));
        }
    }

    if (data[4] != ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) {
        return DHT_CHECKSUM_ERROR;
    }

    humidity = data[0] + data[1] / 10.0;
    temperature = data[2] + data[3] / 10.0;

    return DHT_OK;
}

float DHT::getHumidity() {
    return humidity;
}

float DHT::getTemperature() {
    return temperature;
}
