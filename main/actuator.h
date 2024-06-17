#ifndef ACTUATOR_H
#define ACTUATOR_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

void actuator_task(void *pvParameters);
void turn_on_fan();
void turn_off_fan();
void turn_on_light();
void turn_off_light();
void turn_on_irrigation();
void turn_off_irrigation();

#endif
