#include "sensor.h"
#include "config.h"
#include "DHT.h"
#include "processing.h"

extern QueueHandle_t sensorQueue;
extern SemaphoreHandle_t i2cSemaphore;
extern DHT dhtSensor;

float read_luminosity()
{
    int S_Ang_lumy = adc1_get_raw(LUMINOSITY_SENSOR);
    return S_Ang_lumy;
}

float read_soil_moisture()
{
    int S_Ang_soil = adc1_get_raw(SOIL_SENSOR);
    return S_Ang_soil;
}

void sensor_task(void *pvParameters)
{
    SensorData_t sensorData;
    while (1)
    {
        if (xSemaphoreTake(i2cSemaphore, portMAX_DELAY) == pdTRUE)
        {
            int ret = dhtSensor.readDHT();
            dhtSensor.errorHandler(ret);

            if (ret == DHT_OK)
            {
                sensorData.temperature = dhtSensor.getTemperature();
                sensorData.humidity = dhtSensor.getHumidity();
            }
            else
            {
                sensorData.temperature = 0.0;
                sensorData.humidity = 0.0;
            }

            sensorData.luminosity = read_luminosity();
            sensorData.soil_moisture = read_soil_moisture();

            xSemaphoreGive(i2cSemaphore);

            xQueueSend(sensorQueue, &sensorData, portMAX_DELAY);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
