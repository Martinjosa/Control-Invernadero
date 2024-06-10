#ifndef PROCESSING_H
#define PROCESSING_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_log.h"

#define NUM_READINGS 5

// Estructura para datos de sensores
typedef struct
{
    float temperature;
    float humidity;
    float luminosity;
    float soil_moisture;
} SensorData_t;

typedef struct
{
    float temperatureReadings[NUM_READINGS];
    float humidityReadings[NUM_READINGS];
    float luminosityReadings[NUM_READINGS];
    float soilMoistureReadings[NUM_READINGS];
    int index;
} SensorHistory_t;

void processing_task(void *pvParameters);
float calculate_average(float *readings, int numReadings);

#endif
