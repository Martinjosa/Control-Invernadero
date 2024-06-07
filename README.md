| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

# _Trabajo Práctico: Sistemas Operativos en Tiempo Real_

Este proyecto corresponde al trabajo final de la materia "Arquitecturas Embebidas y Procesamiento en Tiempo Real" perteneciente al posgrado Sistemas Embebidos que dicta el Instituto Universitario Aeronáutico de la ciudad de Córdoba.
El objetivo fundamental de este trabajo práctico es poder aplicar los conecimientos adquiridos a lo largo de la materia, para lo cual, en mi caso, tomo como ejemplo de aplicación el control de algunas variables de un invernadero a traves de sensores y actuadores, ademas las mediciones hechas por los sensores se exponen en una pagina web. En otras palabras se busca hacer una aplicación concreta de lo que se conoce cmo IoT (Internet de la Cosas).
Para llevar adelante este proyecto se utilizó un microcontrolador [ESP32-S3](https://docs.espressif.com/projects/esp-idf/en/stable/esp32s3/hw-reference/index.html) el cual es un dispositivo muy potente, capaz de cubrir los alcances de este proyecto Para desarrollar el firmware se utilizó el editor de texto VScode junto con la extención ESP-IDF de [Espressif](https://www.espressif.com/en), en esta extención se dispone de muchas librerías útiles para este proyecto, las cuales nos permiten aplicar los conceptos de sistema operativo en tiempo real, como freeeRTOS.h, task.h, time.h, etc.

## Sobre el proyecto
Para este proyecto se tomaran muestras de temperatura, humedad, luminocidad y humedad del suelo y en funcion de estas variables se activaran actuadores como el riego, la luminaria o la ventilación para mantener los parametros mensionados dentro de margenes establecidos. Los datos recolectados por los sensores se suben a la nuve, a manera de que el proceso sea mas eficiente no se transmite a la nube cada vez que se toma una muestra de alguna variable, sino que se toman varias muestras, se calcula un promedio y ese dato es el que se envia a la nube. De esta forma se busca hacer uana comunicacion mas eficiente.

Objetivos

    - Desarrollar un sistema de IoT basado en el microcontrolador ESP32-S3. Que a partir de sensores recolecte datos y active los correspondientes actuadores para hacer control de las diferentes variables dentro de un invernadero.
    - Implementar el protocolo de comunicaciones MQTT para subir los datos de las mediciones a la nube.
    - Desarrollar un firmware para el microcontrolador ESP32-S3 mediante el uso de RTOS.
    - Implementar una aplicacion web que permita visualizar las mediciones hechas por los sensores.


Alcance

Recursos


