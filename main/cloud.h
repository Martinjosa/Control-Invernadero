#ifndef CLOUD_H
#define CLOUD_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"

void cloud_communication_task(void *pvParameters);

#endif // CLOUD_TASK_H
