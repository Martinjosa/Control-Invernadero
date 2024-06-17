#include "config.h"

void CONFIG_Puertos()
{
    // Configurar pines GPIO para actuadores
    gpio_pad_select_gpio(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    gpio_pad_select_gpio(LED_IRRIG_PIN);
    gpio_set_direction(LED_IRRIG_PIN, GPIO_MODE_OUTPUT);

    gpio_pad_select_gpio(LED_FAN_PIN);
    gpio_set_direction(LED_FAN_PIN, GPIO_MODE_OUTPUT);

    // Configurar pines ADC para sensores
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(LUMINOSITY_SENSOR, ADC_ATTEN_DB_0);
    adc1_config_channel_atten(SOIL_SENSOR, ADC_ATTEN_DB_0);
}
