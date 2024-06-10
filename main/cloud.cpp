#include "cloud.h"
#include "processing.h"

extern QueueHandle_t processedDataQueue;
//extern SensorData_t sensorData;

void send_to_cloud(SensorData_t *data)
{
    // Simulación de envío de datos a la nube
    ESP_LOGI("Cloud", "Sending data to cloud: Temp=%.2f, Humidity=%.2f, Luminosity=%.2f, Soil Moisture=%.2f",
             data->temperature, data->humidity, data->luminosity, data->soil_moisture);
}

void cloud_communication_task(void *pvParameters)
{
    SensorData_t sensorData;
    while (1)
    {
        // Recibir datos procesados
        if (xQueueReceive(processedDataQueue, &sensorData, portMAX_DELAY))
        {
            // Enviar datos a la nube
            send_to_cloud(&sensorData);
            
            // Simulación de retardo de comunicación
            vTaskDelay(pdMS_TO_TICKS(5000));
        }
    }
}
