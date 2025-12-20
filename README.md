# Gestor de Tutorías

Bienvenido al repositorio oficial del proyecto **GestorTutorias**. Este sistema ha sido desarrollado como parte de la asignatura de Ingeniería del Software (Curso 2025/26), siguiendo la metodología Scrum.

## 📋 Descripción del Proyecto
El objetivo de este software es gestionar el sistema de tutorías, permitiendo.

El proyecto está dividido en dos componentes principales:
1. **GestorTutorias:** La aplicación principal (Consola/Interfaz).
2. **PruebasUnitarias:** Módulo de testing para validar la lógica del negocio.

## 🚀 Instalación y Configuración

### Requisitos Previos
* **IDE Recomendado:** Visual Studio 2022 (con soporte para C++).
* **Sistema de Control de Versiones:** Git.
* **Importante** Ubicar el repositorio en la ruta del gestor de archivos más corta posible, para no dar lugar a problemas con Visual Studio.

### Descarga del Repositorio
Para obtener el código fuente, clona el repositorio utilizando el siguiente comando en tu terminal:

```bash
git clone https://github.com/i42chfua/Trabajo-Final-IS
``` 

#### 🛠️ Instrucciones de Ejecución (Importante)
Al abrir la solución (.slnx o .sln) en Visual Studio, es posible que el proyecto de pruebas esté marcado por defecto. Para ejecutar correctamente el programa, siga estos pasos:

##### Opción A: Ejecutar el Programa Principal

En el Explorador de Soluciones, haga clic derecho sobre el proyecto GestorTutorias.
Seleccione la opción "Establecer como proyecto de inicio" (Set as Startup Project).
Pulse F5 o el botón de "Iniciar".

##### Opción B: Ejecutar las Pruebas Unitarias

Para verificar la integridad del código mediante los tests diseñados:
Haga clic derecho sobre el proyecto PruebasUnitarias.
Seleccione "Establecer como proyecto de inicio".
Pulse F5.
Nota: Las pruebas validan funcionalidades críticas como la conexión a la base de datos y la lógica de clases.

##### Opción C: Ejecución Simultánea

Si desea arrancar ambos componentes a la vez:
Haga clic derecho en la Solución 'GestorTutorias' (parte superior del explorador).
Elija Propiedades -> Proyecto de inicio.
Seleccione Proyectos de inicio múltiples y configure ambos en "Iniciar".

#### 📂 Estructura del Proyecto

La estructura de carpetas sigue el estándar organizado para C++:
/GestorTutorias: Código fuente de la aplicación (.cpp, .h) y base de datos (.db).
/PruebasUnitarias: Tests unitarios.
/src: Archivos fuente de implementación.
/include: Archivos de cabecera.

#### ⚙️ Metodología de Trabajo

Este proyecto ha sido desarrollado utilizando Scrum con Sprints semanales.
Gestión de tareas: YouTrack / Trello.
Control de versiones: GitHub (Flujo de trabajo con Ramas y Pull Requests).
