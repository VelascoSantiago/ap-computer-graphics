cmake_minimum_required(VERSION 3.10)
project(CGhci)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# GLAD se compila una sola vez y lo reusan todas las prácticas
add_library(glad STATIC dependencias/glad/src/glad.c)
target_include_directories(glad PUBLIC dependencias/glad/include)

# Auto-descubre cualquier carpeta pN que tenga su propio CMakeLists.txt
file(GLOB PRACTICAS RELATIVE ${CMAKE_SOURCE_DIR} p*)
foreach(practica ${PRACTICAS})
    if(EXISTS ${CMAKE_SOURCE_DIR}/${practica}/CMakeLists.txt)
        add_subdirectory(${practica})
    endif()
endforeach()