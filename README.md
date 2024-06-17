# Monitoreo y Control de un Vivero/Invernadero con ESP32 Y FreeRTOS

## Descripción General
Este proyecto consiste en la monitorización y control automático de un vivero utilizando un [ESP32-S3](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/get-started/index.html) . El sistema adquiere datos de diversos sensores (como temperatura y humedad, luminocidad y humedad del suelo), los procesa y los envía a la nube mediante el protocolo MQTT para ser expuestos. En este caso he utilizado la plataforma [Adafruit IO](https://io.adafruit.com/) para graficar estos datos, proporcionando una visualización en tiempo real de las métricas como así también un historial. Ademas, con los datos procesados el sistema toma deciciones sobre actuadores (como luminaria, ventiladores o motores) para poder mantener las metricas dentro de los umbrales definidos. 

Se utiliza FreeRTOS para la gestión de tareas en tiempo real y el ESP32 para la ejecución del código. La conectividad WiFi del ESP32 permite la comunicación eficiente de datos para su monitoreo y análisis. En cuanto a los sensores, para medir humedad y temperatura se usó el sensor [DHT22](https://www.sparkfun.com/datasheets/Sensors/Temperature/DHT22.pdf), para medir la humedad del suelo se utilizó el sensor [YL-69](https://www.electronicoscaldas.com/datasheet/YL-69-HL-69.pdf) y para la luminocidad, a falta de sensor, se simuló con un potenciómetro. Para la implementación de los actuadores también se optó por simularlos, en este caso se usó un led de 7 segmentos en el cual un segmento representa la ventilación, otro la activación del sistema de riego y otro la iluminación.


## Objetivo
Desarrollar un sistema IoT para el monitoreo y control de un vivero basado en el microcontrolador ESP32. Este sistema debe ser capaz de recolectar datos ambientales, procesarlos y enviarlos a un servidor remoto para su visualización y análisis. Ademas, debe ser capaz de controlar actuadores para mantener las variables dentro de los umbrales establecidos.
### Metas específicas
* Implementar un sistema de adquisición de datos utilizando sensores de temperatura y humedad, luminocidad y humedad del suelo.
* Procesar los datos adquiridos y enviarlos a un servidor remoto mediante MQTT, como así también controlar actuadores.
* Desarrollar un firmware para el ESP32 utilizando FreeRTOS para la gestión de tareas concurrentes.
* Implementar una aplicación de visualización de datos utilizando Adafruit IO.

### Alcance
El proyecto abarca desde la selección y configuración del hardware necesario (ESP32, sensores, actuadores) hasta el desarrollo del software con FreeRTOS y la implementación de la comunicación de datos con MQTT. Se considerarán aspectos críticos como la gestión energética, la confiabilidad de la transmisión de datos y la integración con plataformas de análisis y visualización de datos.


## Archivos del Proyecto

El proyecto está compuesto por varios archivos fuente y de encabezado que contienen las funciones y estructuras necesarias para la adquisición, procesamiento y envío de datos, así como para la gestión de conectividad y sincronización temporal.

## Requerimientos Funcionales

- **Adquisición de Datos**: El sistema debe adquirir datos de temperatura y humedad, luminocidad y humedad del suelo cada 2 segundos utilizando los sensores correspondientes.
- **Procesamiento de Datos**: El sistema debe ser capaz de controlar las diferentes variables a travez de actuadores.
- **Envío de Datos**: Los datos adquiridos deben ser procesados (se promedian 5 muestras) y enviados a un servidor remoto cada 10 segundos.
- **Gestión de Conectividad**: El sistema debe poder conectarse a una red WiFi y manejar la reconexión en caso de pérdida de conectividad.

## Requerimientos No Funcionales

- **Eficiencia**: El sistema debe ser eficiente en términos de uso de memoria y recursos del microcontrolador.
- **Confiabilidad**: El sistema debe ser capaz de manejar errores y condiciones de carrera, asegurando la integridad de los datos.
- **Seguridad**: Las credenciales de WiFi y del servidor remoto deben ser protegidas utilizando técnicas seguras.
- **Escalabilidad**: El sistema debe ser fácilmente extensible para soportar nuevos sensores y funcionalidades adicionales.

### Archivos Principales

#### main.cpp
- **Descripción**: Inicializa el sistema configurando las conexiones WiFi, creando las colas de mensajes, los semaforos para la sincronizaciòn y lanzando las tareas FreeRTOS para el procesamiento de datos de los sensores y actuadores, el envío de datos vía MQTT.
- **Funciones Clave**:
  - Configuración de conexiones WiFi
  - Creación de colas de mensajes
  - Creación de semàforos para la sincronizació
  - Lanzamiento de tareas FreeRTOS, las cuales tienen diferentes prioridades y son manejadas por diferentes nucleos
  - Configuración del objeto dhtSensor que se usa para adquirir los datos del sensor DHT22

#### processing.cpp y processing.h
- **Descripción**: Define las funciones necesarias para procesar los datos de los sensores. Implementa la función `calculate_average` para calcular el promedio de las lecturas ya que la informacion que se manda a la nube es un promedio de una cirta cantidad de muestras, no se manda muestra a muestra. Esto aporta a la eficiencia del sistema, de tal manera que se consume mucho menos ancho de banda en la transmisión.
Define la estructura de los datos y la estructura donde se almacenan las muestras que serán promediadas.
- **Funciones Clave**:
  - calculate_average`: Calcula el promedio de los datos proporcionados.
  - void process_sensor_data(SensorData_t *data): procesalos datos y los acomoda en la estructura definida. De esta manera se acomodan los datos para enviarlos a la nube.

#### wifi.cpp y wifi.h
- **Descripción**: Define las funciones necesarias para establecer la conexión a internet y en el caso de que se caiga la conexión se reconectará.
- **Funciones Clave**:
  - void wifi_init(void): establece y asegura la reconexión a internet.
 
#### sensor.cpp y sensor.h
- **Descripción**: Toma datos de los sensores del vivero, incluyendo la adquisición de datos de temperatura y humedad, luminocidad y humedad del suelo.
- **Funciones Clave**:
  - void sensor_task(void *pvParameters) La función de la tarea que adquiere datos de los sensores

#### DHT.cpp y DHT.h
- **Descripción**: Toma los datos del sensor de temperatura y humedad y los acomoda para que puedan ser procesados. Se define el objeto DHT
- **Funciones Clave**:
  - void DHT::setDHTgpio(gpio_num_t gpio)
 
#### config.cpp y config.h
- **Descripción**: Declara las variables necesarias, define los umbrales y configura los puertos GPIO
- **Funciones Clave**:
  - void CONFIG_Puertos()

#### cloud.cpp y cloud.h
- **Descripción**: Inicia el cliente MQTT y envia los datos al broker. Ademas dispone de una función para manejar los diferentes eventos entre el servidor y el cliente.
- **Funciones Clave**:
  - void init_mqtt(void): Inicializa el cliente MQTT y alimenta al manejador de eventos.
  - void send_to_cloud(SensorData_t *data): Envía los datos al broker.
 
#### actuator.cpp y actuator.h
 - **Descripción**: Lleva a delante el accionar de los actuadores e indicadores en función de los umbrales
- **Funciones Clave**:
  - void actuator_task(void *pvParameters)
    
## Dependencias

El proyecto depende de varias bibliotecas y componentes:

- **FreeRTOS**: Sistema operativo en tiempo real que permite la gestión eficiente de tareas concurrentes.
- **ESP-IDF**: Biblioteca de soporte para el ESP32, proporcionando funciones esenciales de hardware y software.
- **lwIP**: Implementación ligera de la pila TCP/IP, utilizada para la comunicación de red.
- **DHT Library**: Biblioteca específica para sensores de temperatura y humedad, facilitando la adquisición de datos.
- **MQTT Library**: Biblioteca para la implementación del protocolo MQTT, facilitando la comunicación de datos con el servidor remoto.

![Captura desde 2024-06-17 13-46-29](https://github.com/Martinjosa/Control-Invernadero/assets/91288096/20a757f0-12eb-48f7-88af-c6efc7601c98)


![PINOUT_ESP32_V3](https://github.com/Martinjosa/Control-Invernadero/assets/91288096/24b1d0f1-0fab-4805-a832-23c58bca6684)

