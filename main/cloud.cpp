#include "cloud.h"
#include "processing.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "freertos/event_groups.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_task_wdt.h"

// Credenciales de Adafruit IO
#define AIO_USERNAME "mjosa"
#define AIO_KEY "aio_hOPg208MXD9f5oJw6N5oWIlQtlHn"
#define AIO_FEED_TEMPERATURE "mjosa/feeds/Temp"
#define AIO_FEED_HUMIDITY "mjosa/feeds/Hum"
#define AIO_FEED_LUMINOSITY "mjosa/feeds/luminosity"
#define AIO_FEED_SOIL_MOISTURE "mjosa/feeds/soil-moisture"

// Variable global para el cliente MQTT
static esp_mqtt_client_handle_t mqtt_client;

// Declaración externa de la cola
extern QueueHandle_t processedDataQueue;

// Manejador de eventos MQTT
void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;
    switch (event->event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI("MQTT", "MQTT_EVENT_CONNECTED");
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI("MQTT", "MQTT_EVENT_DISCONNECTED");
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI("MQTT", "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI("MQTT", "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI("MQTT", "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI("MQTT", "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI("MQTT", "MQTT_EVENT_ERROR");
        break;
    default:
        ESP_LOGI("MQTT", "Other event id:%d", event->event_id);
        break;
    }
}

void init_mqtt(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {};
    mqtt_cfg.uri = "mqtt://io.adafruit.com";
    mqtt_cfg.username = AIO_USERNAME;
    mqtt_cfg.password = AIO_KEY;

    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(mqtt_client, MQTT_EVENT_ANY, mqtt_event_handler, NULL);
    esp_mqtt_client_start(mqtt_client);
}

void send_to_cloud(SensorData_t *data)
{
    ESP_LOGI("Cloud", "Sending data to cloud: Temp=%.2f, Humidity=%.2f, Luminosity=%.2f, Soil Moisture=%.2f",
             data->temperature, data->humidity, data->luminosity, data->soil_moisture);

    char payload[64];

    // Publicar la temperatura
    snprintf(payload, sizeof(payload), "%.2f", data->temperature);
    esp_mqtt_client_publish(mqtt_client, AIO_FEED_TEMPERATURE, payload, 0, 1, 0);

    // Publicar la humedad
    snprintf(payload, sizeof(payload), "%.2f", data->humidity);
    esp_mqtt_client_publish(mqtt_client, AIO_FEED_HUMIDITY, payload, 0, 1, 0);

    // Publicar la luminosidad
    snprintf(payload, sizeof(payload), "%.2f", data->luminosity);
    esp_mqtt_client_publish(mqtt_client, AIO_FEED_LUMINOSITY, payload, 0, 1, 0);

    // Publicar la humedad del suelo
    snprintf(payload, sizeof(payload), "%.2f", data->soil_moisture);
    esp_mqtt_client_publish(mqtt_client, AIO_FEED_SOIL_MOISTURE, payload, 0, 1, 0);
}

void cloud_communication_task(void *pvParameters)
{
    SensorData_t sensorData;
    ESP_ERROR_CHECK(esp_task_wdt_add(NULL));
    while (1)
    {
        // Alimentar el Watchdog Timer
        ESP_ERROR_CHECK(esp_task_wdt_reset());

        // Recibir datos procesados
        if (xQueueReceive(processedDataQueue, &sensorData, portMAX_DELAY))
        {
            // Enviar datos a la nube
            send_to_cloud(&sensorData);
        }

        ESP_ERROR_CHECK(esp_task_wdt_reset());
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}