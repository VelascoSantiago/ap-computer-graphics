# CGhci — Entorno de Computación Gráfica (OpenGL en Linux)

Setup reutilizable para prácticas de OpenGL: CMake + GLFW + GLAD + GLM (en vez del stack de Visual Studio con GLEW).

## Estructura

```
CGhci/
├── dependencias/     ← compartido, NO se toca por práctica
│   ├── glad/           funciones de OpenGL
│   ├── glm/            matemáticas (vectores, matrices)
│   └── Shader.h        carga/compila shaders .vs/.frag
├── template/
│   └── CMakeLists.txt  receta genérica, se copia a cada carpeta nueva
├── build/              binarios (NO se sube a git)
├── nueva_practica.sh   crea carpeta + copia CMakeLists + compila
├── fix_glew.py         convierte código GLEW viejo → GLAD
├── CMakeLists.txt      raíz: detecta sola cualquier carpeta con CMakeLists.txt
└── p0/ p1/ proyecto_x/ ...   una carpeta por práctica/proyecto — nombre libre
```

**`dependencias/` en detalle:**
| Carpeta/archivo | Qué es |
|---|---|
| `glad/` | Carga funciones de OpenGL en runtime (reemplazo de GLEW) |
| `glm/` | Solo matemáticas — no dibuja nada |
| `Shader.h` | Lee `.vs`/`.frag`, los compila, expone `.Use()` |

## Setup inicial (una sola vez por máquina)

```bash
sudo apt update
sudo apt install build-essential cmake git libgl1-mesa-dev libx11-dev \
    libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libglfw3-dev
git clone <url-de-tu-repo> CGhci && cd CGhci
```
Si `dependencias/glad/` no viene en el repo: generarlo en https://glad.dav1d.de (C/C++, gl=3.3, Core, GENERATE) y descomprimir en `dependencias/glad/` (queda `include/` y `src/glad.c`).

Extensiones de VS Code: **C/C++** y **CMake Tools** (ambas de Microsoft).

Primera compilación:
```bash
mkdir build && cd build && cmake .. && make -j$(nproc)
```

## Pipeline por práctica/proyecto

| Paso | Comando |
|---|---|
| 1. Crear | `./nueva_practica.sh <nombre> <origen>` — ver modos abajo |
| 2. Arreglar GLEW→GLAD (si aplica) | `python3 fix_glew.py <nombre>` |
| 3. Compilar | `cd build && cmake .. && make -j$(nproc)` |
| 4. Correr | `cd build/<nombre> && ./<nombre>_<archivo>` |

**`<nombre>`** puede ser cualquier cosa: `p4`, `proyecto_final`, etc. No hace falta que empiece con `p`.

**`<origen>` (segundo argumento) admite 4 formas:**
```bash
./nueva_practica.sh p4 practica.zip           # descomprime un zip
./nueva_practica.sh p4 archivo.cpp            # copia un .cpp suelto
./nueva_practica.sh p4 carpeta_ya_descomprimida/  # copia el CONTENIDO de esa carpeta
./nueva_practica.sh p4                        # carpeta vacía, solo CMakeLists
```
En todos los casos se copia el `CMakeLists.txt` genérico al final, pisando cualquiera viejo que haya venido con el origen (evita el error clásico de GLEW).

**Múltiples `.cpp` por carpeta:** el CMakeLists genérico usa GLOB — cada `.cpp` con su propio `main()` genera su propio ejecutable, nombrado `<carpeta>_<archivo>`. Si en cambio querés que varios `.cpp` compilen juntos en un solo binario (solo uno tiene `main()`), avisá y se cambia esa carpeta a modo "un solo target".

**Agregaste un `.cpp` nuevo a una carpeta existente:** no hace falta borrar `build/`, solo:
```bash
cd build && cmake ..
```

**`Shader.h` propio en una práctica:** si la carpeta trae su propio `Shader.h` en la raíz, ese es el que se usa (gana sobre el de `dependencias/`). Por eso `fix_glew.py <carpeta>` es importante correrlo siempre — si ese `Shader.h` local tiene GLEW viejo, hay que arreglarlo también a él.

**En VS Code**, como hay varios targets por carpeta:
- `Ctrl+Shift+P` → `CMake: Set Build Target` (cuál compilar)
- `Ctrl+Shift+P` → `CMake: Select Launch Target` (cuál correr/debuggear)

## Troubleshooting rápido

| Síntoma | Solución |
|---|---|
| CMake compila un archivo que ya no existe / renombraste algo | `rm -rf build && mkdir build && cd build && cmake .. && make -j$(nproc)` (o `CMake: Delete Cache and Reconfigure` en VS Code) |
| Agregué un `.cpp` y no aparece como target | `cd build && cmake ..` |
| `Target links to GLEW::GLEW but the target was not found` | `cp template/CMakeLists.txt <carpeta>/CMakeLists.txt` |
| `undefined reference to __glewXxx` / `glewInit` | `python3 fix_glew.py <carpeta>`, o a mano: `sed -i 's/#include <GL\/glew.h>/#include <glad\/glad.h>/' <carpeta>/*.cpp <carpeta>/*.h` |
| `#error gl.h included before glew.h` | Hay un `Shader.h` duplicado con GLEW en la carpeta: `find . -name "Shader.h"` y arreglalo igual que arriba |
| Corre pero no dibuja nada / ventana vacía | Corré desde `build/<carpeta>/`, no desde la raíz — los shaders están ahí. Verificá con `ls build/<carpeta>/Shader/` |
| `./nueva_practica.sh: Permission denied` | `chmod +x nueva_practica.sh` |
| Quiero ver la línea exacta de un error de compilación | `sed -n '<línea-5>,<línea+5>p' archivo.cpp` |
| Confirmar que GLFW/OpenGL están instalados | `pkg-config --modversion glfw3` / `ldconfig -p \| grep libGL` |
| Ver versión de OpenGL de la GPU | Corré cualquier práctica (la imprime), o `glxinfo \| grep "OpenGL version"` (`sudo apt install mesa-utils` si falta) |
| No sé qué carpetas ya compilaron | `ls build/*/ -d 2>/dev/null \| grep -v CMakeFiles` |

## Código de referencia

<details>
<summary><code>template/CMakeLists.txt</code></summary>

```cmake
get_filename_component(TARGET_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
foreach(SOURCE ${SOURCES})
    get_filename_component(SOURCE_NAME ${SOURCE} NAME_WE)
    add_executable(${TARGET_NAME}_${SOURCE_NAME} ${SOURCE})
    target_include_directories(${TARGET_NAME}_${SOURCE_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/dependencias
        ${CMAKE_SOURCE_DIR}/dependencias/glm
    )
    target_link_libraries(${TARGET_NAME}_${SOURCE_NAME} PRIVATE OpenGL::GL glfw glad)
endforeach()
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Shader DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
```
Si la carpeta no tiene subcarpeta `Shader/`, borrá la última línea.
</details>

<details>
<summary><code>CMakeLists.txt</code> raíz</summary>

```cmake
cmake_minimum_required(VERSION 3.10)
project(CGhci)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
add_library(glad STATIC dependencias/glad/src/glad.c)
target_include_directories(glad PUBLIC dependencias/glad/include)
file(GLOB TODAS RELATIVE ${CMAKE_SOURCE_DIR} *)
foreach(carpeta ${TODAS})
    if(EXISTS ${CMAKE_SOURCE_DIR}/${carpeta}/CMakeLists.txt AND NOT carpeta STREQUAL "template")
        add_subdirectory(${carpeta})
    endif()
endforeach()
```
</details>

<details>
<summary><code>nueva_practica.sh</code></summary>

```bash
#!/bin/bash
# uso:
#   ./nueva_practica.sh <nombre> <ruta.zip>       -> descomprime el zip
#   ./nueva_practica.sh <nombre> <ruta.cpp>        -> copia solo ese .cpp
#   ./nueva_practica.sh <nombre> <ruta_a_carpeta>  -> copia el CONTENIDO de esa carpeta
#   ./nueva_practica.sh <nombre>                   -> carpeta vacía, solo CMakeLists
set -e

DIR=$1
ORIGEN=$2

if [ -z "$DIR" ]; then
    echo "uso: ./nueva_practica.sh <nombre_carpeta> [ruta_al_zip_o_cpp_o_carpeta]"
    exit 1
fi

mkdir -p "$DIR"

if [ -z "$ORIGEN" ]; then
    echo "Carpeta '$DIR' creada con CMakeLists.txt genérico (sin .cpp todavía)."
elif [[ "$ORIGEN" == *.zip ]]; then
    unzip -o "$ORIGEN" -d "$DIR"
    echo "Zip descomprimido en '$DIR'."
elif [[ "$ORIGEN" == *.cpp ]]; then
    cp "$ORIGEN" "$DIR/"
    echo "Archivo '$ORIGEN' copiado a '$DIR/'."
elif [ -d "$ORIGEN" ]; then
    cp -r "$ORIGEN"/. "$DIR"/
    echo "Contenido de '$ORIGEN' copiado a '$DIR/'."
else
    echo "No reconozco el origen '$ORIGEN' (debe ser .zip, .cpp, o una carpeta existente)."
    exit 1
fi

# Siempre imponemos el CMakeLists.txt genérico al final, aunque el origen
# haya traído uno propio (típicamente viejo, con GLEW).
cp template/CMakeLists.txt "$DIR/CMakeLists.txt"

if [ ! -d build ]; then
    echo ""
    echo "No existe build/ todavía. Corre primero:"
    echo "  mkdir build && cd build && cmake .. && make -j\$(nproc)"
    exit 0
fi

cd build
cmake ..
make -j"$(nproc)"

echo ""
echo "Listo -> ejecutables en build/$DIR/:"
ls "$DIR" 2>/dev/null | grep -v CMakeFiles || echo "(todavía no hay .cpp para compilar)"
```
</details>

<details>
<summary><code>fix_glew.py</code></summary>

```python
#!/usr/bin/env python3
import re
import sys
import glob

if len(sys.argv) != 2:
    print("uso: python3 fix_glew.py <carpeta>   (ej: python3 fix_glew.py p1)")
    sys.exit(1)

carpeta = sys.argv[1]
archivos = glob.glob(f"{carpeta}/*.cpp") + glob.glob(f"{carpeta}/*.h")

if not archivos:
    print(f"No se encontraron .cpp/.h en {carpeta}")
    sys.exit(1)

patron_include = re.compile(r'#include\s*<GL/glew\.h>')
patron_init = re.compile(
    r'glewExperimental = GL_TRUE;.*?return EXIT_FAILURE;\n\t\}',
    re.DOTALL
)
reemplazo_init = (
    'if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {\n'
    '\t\tstd::cout << "Failed to initialise GLAD" << std::endl;\n'
    '\t\treturn EXIT_FAILURE;\n'
    '\t}'
)

for ruta in archivos:
    with open(ruta, "r", encoding="utf-8", errors="replace") as f:
        contenido = f.read()

    contenido, n_include = patron_include.subn('#include <glad/glad.h>', contenido)
    contenido, n_init = patron_init.subn(reemplazo_init, contenido)

    if n_include or n_init:
        with open(ruta, "w", encoding="utf-8") as f:
            f.write(contenido)
        print(f"OK  - {ruta} (include: {n_include}, init: {n_init})")
    else:
        print(f"sin cambios - {ruta}")
```
Si dice "sin cambios" pero sabés que sí usa GLEW, el patrón de `glewInit()` puede venir escrito distinto ese año — arreglalo a mano.
</details>

## `.gitignore`

```
build/
*.o
*.a
*.exe
.vscode/
```
`dependencias/` **sí** se sube al repo — sin eso nadie más puede compilar.