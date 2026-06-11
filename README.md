# 🤖 Sistema Autónomo de Navegación: Seguidor de Línea Híbrido

[![GitHub Core Micro](https://img.shields.io/badge/Platform-Jetson%20Nano%20%7C%20MegaPi-blue.svg)](https://github.com/)
[![Embedded RTOS](https://img.shields.io/badge/RTOS-FreeRTOS-green.svg)](https://aws.amazon.com/es/freertos/)
[![Computer Vision](https://img.shields.io/badge/Computer%20Vision-OpenCV%204-orange.svg)](https://opencv.org/)
[![Software-Architecture](https://img.shields.io/badge/Architecture-Distributed%20Control-purple.svg)](https://github.com/)

Este repositorio contiene el desarrollo del proyecto final para el sistema de navegación autónoma de un robot móvil con tracción omnidireccional (mBot MegaPi). El sistema destaca por una arquitectura híbrida distribuida: **Procesamiento de Visión Artificial en el Borde (Edge AI)** de alto nivel acoplado a un **Sistema Operativo de Tiempo Real (FreeRTOS)** con optimizaciones Bare-Metal para el control determinista de bajo nivel.

---

## 📊 Arquitectura General del Sistema

El sistema divide estrictamente las responsabilidades de cómputo para garantizar una latencia mínima y una respuesta inmediata ante eventos del entorno:

| Capa de Control | Hardware | Software Principal | Rol en el Sistema |
| :--- | :--- | :--- | :--- |
| **Alto Nivel (Percepción)** | NVIDIA Jetson Nano | Python 3 + OpenCV 4 | Captura de video, binarización inversa, filtrado y cálculo de centroide/error angular (`giro`). |
| **Bajo Nivel (Control)** | Makeblock MegaPi (ATmega2560) | C++ + FreeRTOS + Bare-Metal | Lectura asíncrona por registros, gestión de colas FIFO, cinemática inversa y evasión por hardware. |

---

## ⚡ Características Principales

* **Edge AI & OpenCV Avanzado:** Pipeline de visión optimizado con máscaras de exclusión geométrica (ROI) y cálculo de momentos estadísticos para obtener el centro de masa de la línea sin falsos positivos.
* **Control Determinista Multitarea:** Implementación de FreeRTOS en la MegaPi mediante tareas concurrentes independientes para la gestión serial y el movimiento, eliminando retardos de hilos (*delays*).
* **Comunicación Segura por Colas:** Sincronización inter-tarea mediante colas FIFO de FreeRTOS (`QueueHandle_t`), asegurando el paso de parámetros de control libre de condiciones de carrera.
* **Evasión Reactiva por Hardware:** Rutina de servicio de interrupción nativa del ADC (`ISR(ADC_vect)`) para frenado inmediato ante obstáculos, aislando el chasis de impactos de forma prioritaria.
* **Optimización Bare-Metal:** Configuración directa de registros del microcontrolador ATmega2560 para la interfaz USART0, temporizadores PWM de alta velocidad y conversión continua del hardware ADC.

---

## 🗺️ Documentación Completa (Wiki del Proyecto)

Para conocer a fondo los detalles de implementación, diagramas, modelos matemáticos y código fuente de cada sección, visita las páginas dedicadas en nuestra **Wiki oficial**:

1. [💻 Procesamiento de Visión Artificial: Jetson Nano](https://github.com/TU_USUARIO/TU_REPOSITORIO/wiki/Desarrollo-de-Software)
   * Explicación del pipeline de OpenCV, binarización inversa, aplicación de máscaras de exclusión y cálculo del error angular. Incluye el script de Python completo.
2. [⚙️ Control de Tiempo Real: Firmware mBot](https://github.com/TU_USUARIO/TU_REPOSITORIO/wiki/Firmware-mBot)
   * Estructura de tareas concurrentes en FreeRTOS, comunicación segura, modelo cinemático de motores y rutinas de interrupción (ISR). Incluye el firmware completo en C++.
3. [🛠️ Guía de Instalación y Despliegue](https://github.com/TU_USUARIO/TU_REPOSITORIO/wiki/Instalaci%C3%B3n-y-Configuraci%C3%B3n)
   * Requisitos y dependencias de software, mapeo de puertos y conexiones físicas de los motores, y pasos secuenciales para arrancar el sistema autónomo en pista.
4. [📊 Validación y Resultados de Pruebas](https://github.com/TU_USUARIO/TU_REPOSITORIO/wiki/Pruebas-y-Resultados)
   * Análisis del factor de compensación de tracción (1.175), desglose de los tiempos de latencia del lazo de control (~42.8 ms) y pruebas dinámicas de evasión de colisiones.

---

## 🚀 Vista Rápida del Despliegue

### Requisitos de Software en Jetson
```bash
sudo apt-get update
sudo apt-get install python3-opencv python3-pip
pip3 install pyserial numpy
