#include "processing.h"
#include "config.h"

extern QueueHandle_t sensorQueue;
extern QueueHandle_t processedDataQueue;

SensorHistory_t sensorHistory = {.temperatureReadings = {0},
                                 .humidityReadings = {0},
                                 .luminosityReadings = {0},
                                 .soilMoistureReadings = {0},
                                 .index = 0};

float calculate_average(float *readings, int numReadings)
{
    float sum = 0.0;
    for (int i = 0; i < numReadings; i++)
    {
        sum += readings[i];
    }
    return numReadings > 0 ? sum / numReadings : 0.0;
}

void process_sensor_data(SensorData_t *data)
{
    sensorHistory.temperatureReadings[sensorHistory.index] = data->temperature;
    sensorHistory.humidityReadings[sensorHistory.index] = data->humidity;
    sensorHistory.luminosityReadings[sensorHistory.index] = data->luminosity;
    sensorHistory.soilMoistureReadings[sensorHistory.index] = data->soil_moisture;

    sensorHistory.index = (sensorHistory.index + 1) % NUM_READINGS;

    data->temperature = calculate_average(sensorHistory.temperatureReadings, NUM_READINGS);
    data->humidity = calculate_average(sensorHistory.humidityReadings, NUM_READINGS);
    data->luminosity = calculate_average(sensorHistory.luminosityReadings, NUM_READINGS);
    data->soil_moisture = calculate_average(sensorHistory.soilMoistureReadings, NUM_READINGS);

    if (data->temperature > TEMP_THRESHOLD)
    {
        ESP_LOGW("Processing", "Temperature exceeds threshold: %.2f", data->temperature);
    }
}


void processing_task(void *pvParameters)
{
    SensorData_t sensorData;
    while (1)
    {
        if (xQueueReceive(sensorQueue, &sensorData, portMAX_DELAY))
        {
            process_sensor_data(&sensorData);

            xQueueSend(processedDataQueue, &sensorData, portMAX_DELAY);

            //send_to_cloud(&sensorData);
        }
    }
}
