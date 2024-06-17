#ifndef SENSOR_H
#define SENSOR_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

void sensor_task(void *pvParameters);

#endif
