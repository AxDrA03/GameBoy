# Versión mínima de CMake requerida
cmake_minimum_required(VERSION 3.15)

# Nombre de tu proyecto y lenguaje
project(GameBoy_project VERSION 1.0 LANGUAGES CXX)

# Especificar el estándar de C++ (ej. C++17 o C++20)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Crear el ejecutable a partir de los archivos fuente (.cpp)
add_executable(GameBoy
    main.cpp
    # Agrega aquí más archivos .cpp si tu proyecto crece:
    # src/mi_clase.cpp
    src/cpu.cpp
    src/MemoryBus.cpp
)

# Opcional: Especificar carpetas de encabezados (.h / .hpp)
target_include_directories(GameBoy PRIVATE include)