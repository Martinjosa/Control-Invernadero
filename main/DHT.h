/* DHT library


*/


#ifndef DHT_H
#define DHT_H

#include "driver/gpio.h"
#include "esp_log.h"

#define DHT_OK 0
#define DHT_CHECKSUM_ERROR -1
#define DHT_TIMEOUT_ERROR -2

class DHT {
public:
    DHT();
    void setDHTgpio(gpio_num_t gpio);
    int readDHT();
    float getHumidity();
    float getTemperature();
    void errorHandler(int response);

private:
    gpio_num_t DHTgpio;
    float humidity = 0.0;
    float temperature = 0.0;
    int getSignalLevel(int usTimeOut, bool state);
};

#endif
