# Entorno de Computación Gráfica (OpenGL en Linux)

Setup reutilizable para las prácticas semanales del curso, usando CMake + GLFW + GLAD + GLM en vez del stack de Visual Studio (GLEW).

## Setup inicial (solo una vez, en una máquina nueva)

Esto es lo que hay que hacer **antes** de poder usar el pipeline de abajo — ya sea porque acabas de formatear, tienes una laptop nueva, o vas a clonar este repo en otra máquina.

### 1. Herramientas de compilación
```bash
sudo apt update
sudo apt install build-essential cmake git
```

### 2. Librerías de desarrollo de OpenGL/X11
```bash
sudo apt install libgl1-mesa-dev libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev
```

### 3. GLFW (crea la ventana/contexto)
```bash
sudo apt install libglfw3-dev
```

### 4. Clona este repo (si es máquina nueva)
```bash
git clone <url-de-tu-repo> CGhci
cd CGhci
```
Como `dependencias/glm` y `dependencias/Shader.h` ya viven en el repo, no hay que rearmarlos. Solo falta GLAD (paso 5), porque son archivos generados que no se versionan igual en todos los setups — revisa primero si `dependencias/glad/` ya viene en el repo; si es así, sáltate el paso 5.

### 5. GLAD (solo si `dependencias/glad/` no existe todavía)
Ve a https://glad.dav1d.de, elige Language=C/C++, API gl=Version 3.3, Profile=Core, dale "GENERATE" y descarga el zip. Descomprímelo dentro de `dependencias/glad`, de modo que quede `dependencias/glad/include` y `dependencias/glad/src/glad.c`.

### 6. VS Code y extensiones
Instala las extensiones "C/C++" (Microsoft) y "CMake Tools" (Microsoft) desde el marketplace de VS Code. Con CMake Tools puedes compilar (botón `Build` o `F7`) y correr (▶ junto a `Build`, en la barra inferior) sin tocar la terminal.

### 7. Primera compilación
```bash
mkdir -p build
cd build
cmake ..
make -j$(nproc)
```

Con esto, el entorno queda listo para usar el pipeline semanal de abajo.

## Estructura del proyecto

```
CGhci/
├── dependencias/        ← compartido por TODAS las prácticas, no se toca
│   ├── glad/              (funciones de OpenGL)
│   ├── glm/                (matemáticas)
│   └── Shader.h             (carga/compila shaders)
├── template/
│   └── CMakeLists.txt    ← receta genérica, se copia igual a cada carpeta de práctica
├── build/                 ← binarios compilados (NO se sube a git)
├── nueva_practica.sh      ← automatiza: descomprime zip + copia CMakeLists + compila
├── CMakeLists.txt         ← raíz: auto-descubre CUALQUIER carpeta que tenga su propio CMakeLists.txt
└── p0/ p1/ p2/ proyecto_final/ ...   ← una carpeta por semana o proyecto — el nombre es libre
```

## Conceptos clave

- **OpenGL**: la API gráfica. La implementa el driver de tu GPU (Mesa en Linux).
- **GLFW**: crea la ventana del sistema y el contexto OpenGL. Sin esto, OpenGL no tiene dónde dibujar.
- **GLAD**: carga en tiempo de ejecución las funciones de OpenGL que el sistema no expone directo. Es el reemplazo de GLEW (que usan en Windows/Visual Studio).
- **GLM**: solo matemáticas (vectores, matrices, proyecciones) al estilo GLSL. No dibuja nada.
- **Shader.h**: clase C++ que lee tus archivos `.vs`/`.frag`, los compila en la GPU y los enlaza en un programa usable con `.Use()`.
- **`.vs` (vertex shader)**: corre una vez por cada vértice. Decide su posición final en pantalla (aquí entran las matrices de GLM).
- **`.frag` (fragment shader)**: corre una vez por cada píxel. Decide su color final (texturas, iluminación, etc).

## Qué cambia por práctica vs. qué no

| No cambia (`dependencias/`) | Sí cambia (carpeta de la práctica) |
|---|---|
| GLAD, GLM, `Shader.h` | uno o más `.cpp` — lógica de la práctica |
| — | `Shader/core.vs` y `core.frag` — qué y cómo se dibuja |
| — | `CMakeLists.txt` de esa carpeta (copia del `template/`, pero vive ahí) |

Lo de `dependencias/` se arma **una sola vez** al inicio del semestre.

## Múltiples `.cpp` por práctica — y nombres de carpeta libres

El `CMakeLists.txt` de `template/` (el que se copia a cada carpeta nueva) usa `file(GLOB ...)` para detectar **todos** los `.cpp` que haya en esa carpeta y generar **un ejecutable por cada uno**, sin que tengas que declarar nada a mano:

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

**Cómo queda el nombre de cada ejecutable:** `<nombre_de_la_carpeta>_<nombre_del_cpp_sin_extensión>`.
Ejemplo: en la carpeta `p2` con `mainp2.cpp` y `practica_ortogonal.cpp`, se generan los targets `p2_mainp2` y `p2_practica_ortogonal`.

**Requisito importante:** cada `.cpp` de la carpeta debe tener su propio `main()` — es decir, cada archivo es un programa independiente, no partes de un mismo programa repartidas en varios archivos. Si en cambio querés varios `.cpp` que se compilen **juntos en un solo ejecutable** (por ejemplo `main.cpp` + `utilidades.cpp` sin `main()` propio), usa esta variante en vez del `foreach`:
```cmake
get_filename_component(TARGET_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_executable(${TARGET_NAME} ${SOURCES})
target_include_directories(${TARGET_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/dependencias
    ${CMAKE_SOURCE_DIR}/dependencias/glm
)
target_link_libraries(${TARGET_NAME} PRIVATE OpenGL::GL glfw glad)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Shader DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
```

**El nombre de la carpeta ya no tiene que ser `pN`.** El `CMakeLists.txt` raíz auto-descubre *cualquier* subcarpeta que tenga su propio `CMakeLists.txt` adentro (excepto `template/`), sin importar cómo se llame:
```cmake
file(GLOB TODAS RELATIVE ${CMAKE_SOURCE_DIR} *)
foreach(carpeta ${TODAS})
    if(EXISTS ${CMAKE_SOURCE_DIR}/${carpeta}/CMakeLists.txt AND NOT carpeta STREQUAL "template")
        add_subdirectory(${carpeta})
    endif()
endforeach()
```
Podés tener `p0`, `p1`, `proyecto_final`, `tarea_extra`, lo que sea — mientras tenga un `CMakeLists.txt` copiado del `template/`, se agrega solo.

Después de agregar un `.cpp` nuevo a una carpeta existente, CMake necesita re-escanear esa carpeta una vez para detectarlo:
```bash
cd build && cmake ..
```
(no hace falta borrar `build/`, solo volver a correr `cmake ..`; VS Code + CMake Tools suele hacer esto solo al guardar cambios en un `CMakeLists.txt`)

## Pipeline semanal

| Paso | Comando | Qué hace | Ejemplo |
|---|---|---|---|
| 1. Nueva práctica | `./nueva_practica.sh <nombre> ruta_al_zip` | Descomprime en `<nombre>/`, copia el CMakeLists genérico, compila | `./nueva_practica.sh p4 ~/Downloads/practica4.zip` |
| 2. Adaptar GLEW → GLAD | `python3 fix_glew.py <nombre>` | Reemplaza el `#include <GL/glew.h>` por `<glad/glad.h>` y el bloque `glewInit()` por `gladLoadGLLoader(...)` en todos los `.cpp`/`.h` de esa carpeta. Si no encuentra nada que cambiar, lo dice y no rompe nada | `python3 fix_glew.py p4` |
| 3. Compilar | `cd build && cmake .. && make -jN` | Regenera el proyecto y compila todo | `cd build && cmake .. && make -j$(nproc)` |
| 4. Ejecutar | `cd build/<nombre> && ./<nombre>_<archivo>` | Corre desde su propia carpeta (ahí están los shaders copiados) | `cd build/p4 && ./p4_main` |
| 5. Iterar código | editar → `make -jN` desde `build/` | Recompila solo lo que cambió, sin repetir `cmake ..` (salvo que hayas agregado un `.cpp` nuevo) | editar `p4/main.cpp` → `cd build && make -j$(nproc)` → `cd p4 && ./p4_main` |

El nombre `<nombre>` puede ser lo que quieras (`p4`, `proyecto_final`, etc.) — ya no tiene que empezar con `p` ni ser un número.

El paso 2 solo hace falta si el zip trae código viejo de GLEW (la mayoría de las prácticas del curso). Correrlo siempre es seguro — si no hay nada que cambiar, `fix_glew.py` no toca los archivos y lo confirma en pantalla. Si ya viene pensado para GLAD, saltas directo del paso 1 al 3.

### `nueva_practica.sh` — código completo

```bash
#!/bin/bash
# uso: ./nueva_practica.sh <nombre_carpeta> <ruta_al_zip>
# ejemplos:
#   ./nueva_practica.sh p4 ~/Downloads/practica4.zip
#   ./nueva_practica.sh proyecto_final ~/Downloads/final.zip
set -e
DIR=$1
ZIP=$2

if [ -z "$DIR" ] || [ -z "$ZIP" ]; then
    echo "uso: ./nueva_practica.sh <nombre_carpeta> <ruta_al_zip>"
    exit 1
fi

mkdir -p "$DIR"
unzip -o "$ZIP" -d "$DIR"
cp template/CMakeLists.txt "$DIR/CMakeLists.txt"

cd build
cmake ..
make -j"$(nproc)"

echo ""
echo "Listo -> ejecutables generados en build/$DIR/:"
ls "$DIR" 2>/dev/null | grep -v CMakeFiles
```

El nombre de la carpeta ahora es un argumento libre — no arma `pN` a partir de un número, así que sirve tanto para prácticas semanales como para proyectos con nombre propio.

### `fix_glew.py` — código completo

Vive en la raíz del proyecto, junto a `nueva_practica.sh`. Se corre una sola vez por práctica (el nombre de la carpeta es libre, no tiene que ser `pN`):
```bash
python3 fix_glew.py <nombre_carpeta>
```

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

Si algún archivo dice "sin cambios" pero tú sabes que sí usa GLEW (por ejemplo el patrón de `glewInit()` viene escrito distinto ese año), tocará arreglarlo a mano esa vez — no todos los profes escriben el bloque idéntico.

### `template/CMakeLists.txt` — código completo (el que se copia a cada carpeta nueva)

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

Si una carpeta no tiene subcarpeta `Shader/`, quita la última línea (`file(COPY ...)`) de su copia — si no, `cmake ..` va a fallar buscando una carpeta que no existe.

### `CMakeLists.txt` raíz — código completo

```cmake
cmake_minimum_required(VERSION 3.10)
project(CGhci)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
# GLAD se compila una sola vez y lo reusan todas las prácticas
add_library(glad STATIC dependencias/glad/src/glad.c)
target_include_directories(glad PUBLIC dependencias/glad/include)
# Auto-descubre CUALQUIER carpeta que tenga su propio CMakeLists.txt (menos template/)
file(GLOB TODAS RELATIVE ${CMAKE_SOURCE_DIR} *)
foreach(carpeta ${TODAS})
    if(EXISTS ${CMAKE_SOURCE_DIR}/${carpeta}/CMakeLists.txt AND NOT carpeta STREQUAL "template")
        add_subdirectory(${carpeta})
    endif()
endforeach()
```

---

## Troubleshooting — sets de comandos

### "No such file or directory" al entrar a una carpeta
```bash
pwd                     # confirma dónde estás parado
ls                      # confirma que la carpeta que buscas existe aquí
```

### CMake sigue intentando compilar un archivo que ya no existe / renombraste
Pasa cuando la caché de `build/` quedó con una configuración vieja (por ejemplo, de cuando la carpeta tenía un solo `.cpp` con otro nombre). Hay que forzar una reconfiguración completa:
```bash
cd ~/Documents/CGhci
rm -rf build
mkdir build && cd build
cmake ..
make -j$(nproc)
```
En VS Code: `Ctrl+Shift+P` → `CMake: Delete Cache and Reconfigure`.

### Agregué un `.cpp` nuevo a una carpeta y no aparece como target
El `CMakeLists.txt` usa `file(GLOB ...)`, que se evalúa cuando corre `cmake`, no en cada `make`. Basta con volver a configurar (no hace falta borrar `build/`):
```bash
cd build && cmake ..
```
En VS Code: `Ctrl+Shift+P` → `CMake: Configure` (o simplemente guardar el `CMakeLists.txt` de esa carpeta, CMake Tools suele detectarlo solo).

### No sé qué target elegir para compilar/correr en VS Code
Cada `.cpp` genera su propio ejecutable llamado `<carpeta>_<archivo>` (ver sección "Múltiples `.cpp` por práctica" arriba).
- `Ctrl+Shift+P` → `CMake: Set Build Target` → elegís cuál compilar.
- `Ctrl+Shift+P` → `CMake: Select Launch Target` → elegís cuál correr/debuggear con ▶.

### Quiero un proyecto con nombre libre (no `pN`)
Ya no hace falta que empiece con `p`. Simplemente:
```bash
mkdir nombre_que_quieras
cp template/CMakeLists.txt nombre_que_quieras/CMakeLists.txt
# metés tu(s) .cpp ahí
cd build && cmake .. && make -j$(nproc)
```
El `CMakeLists.txt` raíz detecta cualquier carpeta con su propio `CMakeLists.txt` adentro, sin importar el nombre (ver código completo arriba).

### Error de CMake: `Target "pN" links to GLEW::GLEW but the target was not found`
Una carpeta tiene un `CMakeLists.txt` viejo (de antes de este setup, o de antes del cambio a GLOB). Cámbialo por el genérico:
```bash
cp template/CMakeLists.txt <carpeta>/CMakeLists.txt
cd build && cmake .. && make -j$(nproc)
```

### Error de compilación: `undefined reference to __glewXxx` / `glewInit`
El `.cpp` o `Shader.h` de esa práctica todavía incluye GLEW. Revisa cuáles:
```bash
grep -rl "glew" <carpeta>/*.cpp <carpeta>/*.h
```
Arregla el include:
```bash
sed -i 's/#include <GL\/glew.h>/#include <glad\/glad.h>/' <carpeta>/*.cpp <carpeta>/*.h
```
Si el error persiste, busca el bloque de inicialización y reemplázalo a mano (o con el script de Python) por:
```cpp
if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialise GLAD" << std::endl;
    return EXIT_FAILURE;
}
```
(este bloque va después de `glfwMakeContextCurrent(window)`)

### Error de compilación: `#error gl.h included before glew.h`
Hay un `Shader.h` duplicado dentro de la carpeta de la práctica (`<carpeta>/Shader.h`) que se está usando en vez del de `dependencias/`. Revisa cuál toma prioridad y arréglalo también:
```bash
find . -name "Shader.h"
sed -i 's/#include <GL\/glew.h>/#include <glad\/glad.h>/' <carpeta>/Shader.h
```

### Corre pero no aparece ninguna ventana / no dibuja nada
Casi siempre es el directorio de trabajo — el binario busca `Shader/core.vs` relativo a donde estás parado, no donde está el `.cpp`:
```bash
cd build/<carpeta>     # NO ./build/<carpeta>/<ejecutable> desde la raíz
./<carpeta>_<archivo>
```
Verifica que los shaders sí se copiaron ahí:
```bash
ls build/<carpeta>/Shader/
```
Si está vacío, revisa que el `CMakeLists.txt` de esa carpeta tenga la línea `file(COPY ... Shader DESTINATION ...)` (viene en el `template/`) y que la carpeta `Shader/` realmente exista dentro de `<carpeta>/`.

### Quiero recompilar desde cero (por si algo quedó en mal estado)
```bash
cd ~/Documents/CGhci
rm -rf build
mkdir build
cd build
cmake ..
make -j$(nproc)
```

### No sé qué carpetas de práctica existen ni si compilaron
```bash
ls build/*/  -d 2>/dev/null | grep -v CMakeFiles
```

### El script `nueva_practica.sh` da "Permission denied"
```bash
chmod +x nueva_practica.sh
```

### Ver exactamente qué línea de código está fallando
El compilador (g++) siempre imprime `archivo:línea:columna: error: ...`. Para ver el contexto de esa línea:
```bash
sed -n '<línea-5>,<línea+5>p' ruta/al/archivo.cpp
```
Ejemplo, si el error dice `p4/main.cpp:48:`:
```bash
sed -n '43,53p' p4/main.cpp
```

### Confirmar que GLFW y OpenGL están instalados
```bash
pkg-config --modversion glfw3
ldconfig -p | grep libGL
```

### Ver qué versión de OpenGL corre tu GPU realmente
Corre cualquier práctica compilada — casi todas imprimen esto al iniciar (vía `glGetString`). Si no, prueba:
```bash
glxinfo | grep "OpenGL version"
```
(si `glxinfo` no existe: `sudo apt install mesa-utils`)

### Quiero iterar sobre el mismo código
```bash
cd build
make -j$(nproc)
cd <carpeta>
./<carpeta>_<archivo>
```

---

## `.gitignore`

```
build/
*.o
*.a
*.exe
.vscode/
```
`dependencias/` sí se sube al repo — sin eso nadie más puede compilar el proyecto.