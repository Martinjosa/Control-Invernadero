| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

# _Trabajo Práctico: Sistemas Operativos en Tiempo Real_

Este proyecto corresponde al trabajo final de la materia "Arquitecturas Embebidas y Procesamiento en Tiempo Real" perteneciente al posgrado Sistemas Embebidos que dicta el Instituto Universitario Aeronáutico de la ciudad de Córdoba.
El objetivo fundamental de este trabajo práctico es poder aplicar los conecimientos adquiridos a lo largo de la materia, para lo cual, en mi caso, tomo como ejemplo de aplicación el control de algunas variables de un invernadero a traves de sensores y actuadores, ademas las mediciones hechas por los sensores se exponen en una pagina web. En otras palabras se busca hacer una aplicación concreta de lo que se conoce cmo IoT (Internet de la Cosas).
Para llevar adelante este proyecto se utilizó un microcontrolador [ESP32-S3](https://docs.espressif.com/projects/esp-idf/en/stable/esp32s3/hw-reference/index.html) el cual es un dispositivo muy potente, capaz de cubrir los alcances de este proyecto Para desarrollar el firmware se utilizó el editor de texto VScode junto con la extención ESP-IDF de [Espressif](https://www.espressif.com/en), en esta extención se dispone de muchas librerías útiles para este proyecto, las cuales nos permiten aplicar los conceptos de sistema operativo en tiempo real, como freeeRTOS.h, task.h, time.h, etc.

## Sobre el proyecto

Objetivos

    - Desarrollar un dispositivo IoT basado en el microcontrolador RP2040 y el protocolo LoRa. Aprovechando las capacidades del RP2040 y la tecnología LoRa para crear un nodo IoT capaz de recolectar y transmitir datos de manera eficiente, bajo consumo y largas distancias.
    - Implementar una comunicación nodo a nodo: Configurar dos nodos LoRa que permita transmitir datos de sensores de un nodo a otro.
    - Desarrollar un firmware para el microcontrolador ESP32-S3 mediante el uso de RTOS.
    - Implementar una aplicacion web que permita visualizar las mediciones hechas por los sensores.


Alcance

Recursos


