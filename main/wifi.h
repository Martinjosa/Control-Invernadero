#ifndef WIFI_H
#define WIFI_H

#include "esp_wifi.h"
#include <wifi.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

void wifi_init(void);

#endif // WIFI_H
