#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "esp_system.h"
#include "DHT.h"
#include "config.h"
#include "actuator.h"
#include "sensor.h"
#include "cloud.h"
#include "processing.h"

// Inicializacion señales analògicas
int S_Ang_lumy = 0;
int S_Ang_soil = 0;

DHT dhtSensor; //Variable global para el objeto densor DHT22


// Declaración de colas y semáforos
QueueHandle_t sensorQueue;
QueueHandle_t processedDataQueue;
SemaphoreHandle_t i2cSemaphore;
SemaphoreHandle_t actuatorSemaphore;


// Declaración de las tareas
void sensor_task(void *pvParameters);
void processing_task(void *pvParameters);
void actuator_task(void *pvParameters);
void cloud_communication_task(void *pvParameters);

extern "C" void app_main()
{
    // Inicializar NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    CONFIG_Puertos();

    // Crear colas
    sensorQueue = xQueueCreate(10, sizeof(SensorData_t));
    processedDataQueue = xQueueCreate(10, sizeof(SensorData_t));

    // Comprobar que las colas se han creado correctamente
    if (sensorQueue == NULL || processedDataQueue == NULL)
    {
        ESP_LOGE("Queue", "Error creating the queues");
        return;
    }

    // Crear semáforos
    i2cSemaphore = xSemaphoreCreateMutex();
    actuatorSemaphore = xSemaphoreCreateMutex();

    // Comprobar que los semáforos se han creado correctamente
    if (i2cSemaphore == NULL || actuatorSemaphore == NULL)
    {
        ESP_LOGE("Semaphore", "Error creating the semaphores");
        return;
    }

    // Configurar el sensor DHT
    dhtSensor.setDHTgpio(DHT_GPIO);

   // Crear tareas FreeRTOS y asignarlas a los núcleos
    xTaskCreatePinnedToCore(sensor_task, "Sensor Task", 4096, NULL, 6, NULL, 0);
    xTaskCreatePinnedToCore(processing_task, "Processing Task", 4096, NULL, 5, NULL, 0);
    xTaskCreatePinnedToCore(actuator_task, "Actuator Task", 4096, NULL, 4, NULL, 0);
    xTaskCreatePinnedToCore(cloud_communication_task, "Cloud Communication Task", 4096, NULL, 5, NULL, 1);
}
