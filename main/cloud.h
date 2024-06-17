#ifndef CLOUD_H
#define CLOUD_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_err.h"
#include "mqtt_client.h"
#include "processing.h"

// Inicializa el cliente MQTT y lo conecta al servidor de Adafruit IO
void init_mqtt(void);

void send_to_cloud(SensorData_t *data);

#endif // CLOUD_TASK_H
