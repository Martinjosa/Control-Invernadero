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
#include "driver/adc.h"
#include "driver/gpio.h"
#include "DHT.h"

// Definición de umbrales y configuraciones
#define TEMP_THRESHOLD 25.0
#define LUMINOSITY_THRESHOLD 500
#define SOIL_MOISTURE_THRESHOLD 2300

#define NUM_READINGS 5

#define LUMINOSITY_SENSOR ADC1_CHANNEL_6    //Para el potenciometro
#define SOIL_SENSOR ADC1_CHANNEL_5    //Para el potenciometro
#define DHT_GPIO GPIO_NUM_4               //para el sensor DHT22
#define LED_PIN GPIO_NUM_48                 //para el led
#define LED_IRRIG_PIN GPIO_NUM_47           //para el riego
#define LED_FAN_PIN GPIO_NUM_46             //para la ventilación

int S_Ang_lumy = 0;
int S_Ang_soil = 0;
DHT dhtSensor;

// Estructura para datos de sensores
typedef struct {
    float temperature;
    float humidity;
    float luminosity;
    float soil_moisture;
} SensorData_t;

typedef struct {
    float temperatureReadings[NUM_READINGS];
    float humidityReadings[NUM_READINGS];
    float luminosityReadings[NUM_READINGS];
    float soilMoistureReadings[NUM_READINGS];
    int index;
} SensorHistory_t;

// Inicializar estructura SensorHistory_t
SensorHistory_t sensorHistory = { .temperatureReadings = {0}, 
                                   .humidityReadings = {0}, 
                                   .luminosityReadings = {0}, 
                                   .soilMoistureReadings = {0},
                                   .index = 0 };

// Función auxiliar para calcular el promedio
float calculate_average(float* readings, int numReadings) {
    float sum = 0.0;
    for (int i = 0; i < numReadings; i++) {
        sum += readings[i];
    }
    return sum / numReadings;
}

// Declaración de colas y semáforos
QueueHandle_t sensorQueue;
QueueHandle_t processedDataQueue;
SemaphoreHandle_t i2cSemaphore;
SemaphoreHandle_t actuatorSemaphore;

void CONFIG_Puertos();

// Funciones de lectura de sensores y control de actuadores
float read_luminosity() {
    S_Ang_lumy = adc1_get_raw(LUMINOSITY_SENSOR);
    return S_Ang_lumy;
}

float read_soil_moisture() {
    // Simulación de lectura de humedad del suelo
    S_Ang_soil = adc1_get_raw(SOIL_SENSOR);
    return S_Ang_soil;
}

void process_sensor_data(SensorData_t* data) {
    // Almacenar las lecturas actuales en el historial
    sensorHistory.temperatureReadings[sensorHistory.index] = data->temperature;
    sensorHistory.humidityReadings[sensorHistory.index] = data->humidity;
    sensorHistory.luminosityReadings[sensorHistory.index] = data->luminosity;
    sensorHistory.soilMoistureReadings[sensorHistory.index] = data->soil_moisture;
    
    // Incrementar el índice circular
    sensorHistory.index = (sensorHistory.index + 1) % NUM_READINGS;
    
    // Calcular los promedios móviles
    data->temperature = calculate_average(sensorHistory.temperatureReadings, NUM_READINGS);
    data->humidity = calculate_average(sensorHistory.humidityReadings, NUM_READINGS);
    data->luminosity = calculate_average(sensorHistory.luminosityReadings, NUM_READINGS);
    data->soil_moisture = calculate_average(sensorHistory.soilMoistureReadings, NUM_READINGS);

    // Lógica adicional para detectar anomalías o condiciones específicas
    if (data->temperature > TEMP_THRESHOLD) {
        ESP_LOGW("Processing", "Temperature exceeds threshold: %.2f", data->temperature);
    }
}

void send_to_cloud(SensorData_t* data) {
    // Simulación de envío de datos a la nube
    ESP_LOGI("Cloud", "Sending data to cloud: Temp=%.2f, Humidity=%.2f, Luminosity=%.2f, Soil Moisture=%.2f",
             data->temperature, data->humidity, data->luminosity, data->soil_moisture);
}

void turn_on_fan() {
    ESP_LOGI("Actuator", "Fan turned ON");
    gpio_set_level(LED_FAN_PIN, 1);
}

void turn_off_fan() {
    ESP_LOGI("Actuator", "Fan turned OFF");
    gpio_set_level(LED_FAN_PIN, 0);
}

void turn_on_light() {
    ESP_LOGI("Actuator", "Light turned ON");
    gpio_set_level(LED_PIN, 1);
}

void turn_off_light() {
    ESP_LOGI("Actuator", "Light turned OFF");
    gpio_set_level(LED_PIN, 0);
}

void turn_on_irrigation() {
    ESP_LOGI("Actuator", "Irrigation system turned ON");
    gpio_set_level(LED_IRRIG_PIN, 1);
}

void turn_off_irrigation() {
    ESP_LOGI("Actuator", "Irrigation system turned OFF");
    gpio_set_level(LED_IRRIG_PIN, 0);
}

// Declaración de las tareas
void sensor_task(void *pvParameters);
void processing_task(void *pvParameters);
void actuator_task(void *pvParameters);

extern "C" void app_main() {
    // Inicializar NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    CONFIG_Puertos();
    
    // Crear colas
    sensorQueue = xQueueCreate(10, sizeof(SensorData_t));
    processedDataQueue = xQueueCreate(10, sizeof(SensorData_t));

    // Comprobar que las colas se han creado correctamente
    if (sensorQueue == NULL || processedDataQueue == NULL) {
        ESP_LOGE("Queue", "Error creating the queues");
        return;
    }
    
    // Crear semáforos
    i2cSemaphore = xSemaphoreCreateMutex();
    actuatorSemaphore = xSemaphoreCreateMutex();

    // Comprobar que los semáforos se han creado correctamente
    if (i2cSemaphore == NULL || actuatorSemaphore == NULL) {
        ESP_LOGE("Semaphore", "Error creating the semaphores");
        return;
    }

    // Configurar el sensor DHT
    dhtSensor.setDHTgpio(DHT_GPIO);
    
    // Crear tareas
    xTaskCreate(sensor_task, "Sensor Task", 4096, NULL, 5, NULL);
    xTaskCreate(processing_task, "Processing Task", 4096, NULL, 5, NULL);
    xTaskCreate(actuator_task, "Actuator Task", 4096, NULL, 5, NULL);
}

void sensor_task(void *pvParameters) {
    SensorData_t sensorData;
    while (1) {
        // Intentar tomar el semáforo del I2C
        if (xSemaphoreTake(i2cSemaphore, portMAX_DELAY) == pdTRUE) {
            // Leer temperatura y humedad del sensor DHT
            int ret = dhtSensor.readDHT();
            dhtSensor.errorHandler(ret);

            if (ret == DHT_OK) {
                sensorData.temperature = dhtSensor.getTemperature();
                sensorData.humidity = dhtSensor.getHumidity();
            } else {
                sensorData.temperature = 0.0;
                sensorData.humidity = 0.0;
            }
            
            // Leer luminosidad
            sensorData.luminosity = read_luminosity();
            
            // Leer humedad del suelo
            sensorData.soil_moisture = read_soil_moisture();
            
            // Liberar el semáforo del I2C
            xSemaphoreGive(i2cSemaphore);
            
            // Enviar datos a la cola
            xQueueSend(sensorQueue, &sensorData, portMAX_DELAY);
        }
        
        // Esperar antes de la siguiente lectura
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void processing_task(void *pvParameters) {
    SensorData_t sensorData;
    while (1) {
        // Recibir datos de la cola
        if (xQueueReceive(sensorQueue, &sensorData, portMAX_DELAY)) {
            // Procesar datos (por ejemplo, calcular promedios)
            process_sensor_data(&sensorData);
            
            // Enviar datos procesados a la cola de actuadores
            xQueueSend(processedDataQueue, &sensorData, portMAX_DELAY);
            
            // Enviar datos a la nube
            send_to_cloud(&sensorData);
        }
    }
}

void actuator_task(void *pvParameters) {
    SensorData_t sensorData;
    while (1) {
        // Recibir datos procesados
        if (xQueueReceive(processedDataQueue, &sensorData, portMAX_DELAY)) {
            // Intentar tomar el semáforo del actuador
            if (xSemaphoreTake(actuatorSemaphore, portMAX_DELAY) == pdTRUE) {
                // Controlar ventilador
                if (sensorData.temperature > TEMP_THRESHOLD) {  //Agregar humedad
                    turn_on_fan();
                } else {
                    turn_off_fan();
                }
                
                // Controlar luces
                if (sensorData.luminosity < LUMINOSITY_THRESHOLD) {
                    turn_on_light();
                } else {
                    turn_off_light();
                }
                
                // Controlar sistema de riego
                if (sensorData.soil_moisture > SOIL_MOISTURE_THRESHOLD) {
                    turn_on_irrigation();
                } else {
                    turn_off_irrigation();
                }
                
                // Liberar el semáforo del actuador
                xSemaphoreGive(actuatorSemaphore);
            }
        }
    }
}

// Funcion para la configuracion de puertos
void CONFIG_Puertos() {
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    gpio_reset_pin(LED_IRRIG_PIN);
    gpio_set_direction(LED_IRRIG_PIN, GPIO_MODE_OUTPUT);

    gpio_reset_pin(LED_FAN_PIN);
    gpio_set_direction(LED_FAN_PIN, GPIO_MODE_OUTPUT);
    
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(LUMINOSITY_SENSOR, ADC_ATTEN_DB_0);

    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(SOIL_SENSOR, ADC_ATTEN_DB_0);

}
