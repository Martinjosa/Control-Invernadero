
#include "actuator.h"
#include "config.h"
#include "processing.h"
#include "esp_task_wdt.h"

extern QueueHandle_t processedDataQueue;
extern SemaphoreHandle_t actuatorSemaphore;

void turn_on_fan()
{
    ESP_LOGI("Actuator", "Fan turned ON");
    gpio_set_level(LED_FAN_PIN, 1);
}

void turn_off_fan()
{
    ESP_LOGI("Actuator", "Fan turned OFF");
    gpio_set_level(LED_FAN_PIN, 0);
}

void turn_on_light()
{
    ESP_LOGI("Actuator", "Light turned ON");
    gpio_set_level(LED_PIN, 1);
}

void turn_off_light()
{
    ESP_LOGI("Actuator", "Light turned OFF");
    gpio_set_level(LED_PIN, 0);
}

void turn_on_irrigation()
{
    ESP_LOGI("Actuator", "Irrigation system turned ON");
    gpio_set_level(LED_IRRIG_PIN, 1);
}

void turn_off_irrigation()
{
    ESP_LOGI("Actuator", "Irrigation system turned OFF");
    gpio_set_level(LED_IRRIG_PIN, 0);
}

// Tarea de Control de Actuadores
void actuator_task(void *pvParameters)
{
    SensorData_t sensorData;
    ESP_ERROR_CHECK(esp_task_wdt_add(NULL));

    while (1)
    {
        // Recibir datos procesados
        if (xQueueReceive(processedDataQueue, &sensorData, portMAX_DELAY))
        {
            // Intentar tomar el semáforo del actuador
            if (xSemaphoreTake(actuatorSemaphore, portMAX_DELAY) == pdTRUE)
            {
                // Controlar ventilador
                if (sensorData.temperature > TEMP_THRESHOLD)
                { // Agregar humedad
                    turn_on_fan();
                }
                else
                {
                    turn_off_fan();
                }

                // Controlar luces
                if (sensorData.luminosity < LUMINOSITY_THRESHOLD)
                {
                    turn_on_light();
                }
                else
                {
                    turn_off_light();
                }

                // Controlar sistema de riego
                if (sensorData.soil_moisture > SOIL_MOISTURE_THRESHOLD)
                {
                    turn_on_irrigation();
                }
                else
                {
                    turn_off_irrigation();
                }

                // Liberar el semáforo del actuador
                xSemaphoreGive(actuatorSemaphore);

                ESP_ERROR_CHECK(esp_task_wdt_reset());
            }
        }
    }
}