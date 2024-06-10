#ifndef SENSOR_H
#define SENSOR_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"


// Inicializacion señales analògicas
//int S_Ang_lumy = 0;
//int S_Ang_soil = 0;

void sensor_task(void *pvParameters);

#endif
