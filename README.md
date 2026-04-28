# Analizador Kanban - Proyecto LFP 

Este proyecto es un analizador léxico y sintáctico construido en **C++** con una Interfaz Gráfica de Usuario (GUI) desarrollada en **Qt6**. Su propósito es leer archivos con extensión `.task` que contienen la estructura de un tablero Kanban, validar su sintaxis, y generar reportes automatizados.

##  Características Principales
* **Analizador Léxico (Scanner):** Identifica palabras reservadas, cadenas, fechas, prioridades y atrapa errores léxicos sin colapsar.
* **Analizador Sintáctico (Parser):** Construye la estructura del tablero, valida la gramática y cuenta con "Modo Pánico" y sincronización local para evitar cascadas de errores.
* **Generación de Reportes:** * Árbol de derivación visual utilizando Graphviz (`.dot`).
  * Tableros HTML dinámicos con CSS integrado.
  * Reportes de carga por responsable con barras de progreso.
  * Tabla de Errores detallada.
* **Interfaz Gráfica:** Pestañas para visualizar el código fuente, tabla de tokens válidos, tabla de errores, y accesos directos para abrir los reportes en el navegador.

---

##  Requisitos Previos

Para compilar y ejecutar este proyecto desde cero, necesitas tener instalado:
* **Compilador C++:** MinGW (v13.1.0 64-bit o equivalente) con soporte para C++17.
* **CMake:** Versión 3.16 o superior.
* **Qt6:** Instalado con el componente de `Widgets`.
* **Graphviz:** (Opcional, pero recomendado) Para renderizar el archivo `arbol.dot` generado.

---

## 💻 Instrucciones de Compilación (VS Code / Terminal Windows)

El proyecto está configurado para compilarse fácilmente utilizando CMake y MinGW Makefiles directamente desde la terminal. Sigue estos pasos:
