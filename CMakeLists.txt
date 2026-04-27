cmake_minimum_required(VERSION 3.16)
project(ProyectoLFP VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Habilitar el manejo automático de Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Buscar Qt6 (asegúrate de que CMAKE_PREFIX_PATH apunte a tu instalación de Qt si falla)
find_package(Qt6 COMPONENTS Widgets REQUIRED)

# Agregar todos tus archivos fuente
add_executable(ProyectoLFP
    src/main.cpp
    src/LexicalAnalyzer.cpp
    src/SyntaxAnalyzer.cpp
    src/ReportGenerator.cpp
    src/MainWindow.cpp # <-- ¡Tu nueva ventana!
)

# Vincular las librerías de Qt a tu ejecutable
target_link_libraries(ProyectoLFP PRIVATE Qt6::Widgets)