#ifndef CONFIG_H
#define CONFIG_H

#include "driver/adc.h"
#include "driver/gpio.h"

#define TEMP_THRESHOLD 30.0
#define LUMINOSITY_THRESHOLD 500
#define SOIL_MOISTURE_THRESHOLD 3300

#define LUMINOSITY_SENSOR ADC1_CHANNEL_6 // Para el potenciometro
#define SOIL_SENSOR ADC1_CHANNEL_5       // Para el sensor de humedad del suelo
#define DHT_GPIO GPIO_NUM_4              // para el sensor DHT22
#define LED_PIN GPIO_NUM_48              // para el led
#define LED_IRRIG_PIN GPIO_NUM_47        // para el riego
#define LED_FAN_PIN GPIO_NUM_46          // para la ventilación

void CONFIG_Puertos();

#endif