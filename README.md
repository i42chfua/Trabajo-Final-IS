# Gestor de Tutorías

Bienvenido al repositorio oficial del proyecto **GestorTutorias**. [cite_start]Este sistema ha sido desarrollado como parte de la asignatura de Ingeniería del Software (Curso 2025/26) [cite: 5][cite_start], siguiendo la metodología Scrum[cite: 18].

## 📋 Descripción del Proyecto
El objetivo de este software es gestionar el sistema de tutorías, permitiendo [Añade aquí una frase corta sobre qué hace tu programa, ej: reservar citas, gestionar alumnos, etc.].

El proyecto está dividido en dos componentes principales:
1. **GestorTutorias:** La aplicación principal (Consola/Interfaz).
2. [cite_start]**PruebasUnitarias:** Módulo de testing para validar la lógica del negocio[cite: 396].

## 🚀 Instalación y Configuración

### Requisitos Previos
* **IDE Recomendado:** Visual Studio 2022 (con soporte para C++).
* [cite_start]**Sistema de Control de Versiones:** Git[cite: 807].

### Descarga del Repositorio
[cite_start]Para obtener el código fuente, clona el repositorio utilizando el siguiente comando en tu terminal[cite: 1469]:

```bash
git clone https://github.com/i42chfua/Trabajo-Final-IS
```

🛠️ Instrucciones de Ejecución (Importante)
Al abrir la solución (.slnx o .sln) en Visual Studio, es posible que el proyecto de pruebas esté marcado por defecto. Para ejecutar correctamente el programa, siga estos pasos:

Opción A: Ejecutar el Programa Principal

En el Explorador de Soluciones, haga clic derecho sobre el proyecto GestorTutorias.
Seleccione la opción "Establecer como proyecto de inicio" (Set as Startup Project).
Pulse F5 o el botón de "Iniciar".

Opción B: Ejecutar las Pruebas Unitarias

Para verificar la integridad del código mediante los tests diseñados:
Haga clic derecho sobre el proyecto PruebasUnitarias.
Seleccione "Establecer como proyecto de inicio".
Pulse F5.
Nota: Las pruebas validan funcionalidades críticas como la conexión a la base de datos y la lógica de clases.

Opción C: Ejecución Simultánea

Si desea arrancar ambos componentes a la vez:
Haga clic derecho en la Solución 'GestorTutorias' (parte superior del explorador).
Elija Propiedades -> Proyecto de inicio.
Seleccione Proyectos de inicio múltiples y configure ambos en "Iniciar".

📂 Estructura del Proyecto

La estructura de carpetas sigue el estándar organizado para C++:
/GestorTutorias: Código fuente de la aplicación (.cpp, .h) y base de datos (.db).
/PruebasUnitarias: Tests unitarios.
/src: Archivos fuente de implementación.
/include: Archivos de cabecera.

⚙️ Metodología de Trabajo

Este proyecto ha sido desarrollado utilizando Scrum con Sprints semanales.
Gestión de tareas: YouTrack / Trello.
Control de versiones: GitHub (Flujo de trabajo con Ramas y Pull Requests).
