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
│   └── CMakeLists.txt    ← receta genérica, se copia igual a cada pN
├── build/                 ← binarios compilados (NO se sube a git)
├── nueva_practica.sh      ← automatiza: descomprime zip + copia CMakeLists + compila
├── CMakeLists.txt         ← raíz: auto-descubre cualquier carpeta pN
└── p0/ p1/ p2/ p3/ ...    ← una carpeta por semana
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

| No cambia (`dependencias/`) | Sí cambia (`pN/`) |
|---|---|
| GLAD, GLM, `Shader.h` | `Main_X.cpp` — lógica de la práctica |
| — | `Shader/core.vs` y `core.frag` — qué y cómo se dibuja |
| — | `CMakeLists.txt` de esa carpeta (copia del `template/`, pero vive ahí) |

Lo de `dependencias/` se arma **una sola vez** al inicio del semestre.

## Pipeline semanal

| Paso | Comando | Qué hace | Ejemplo |
|---|---|---|---|
| 1. Nueva práctica | `./nueva_practica.sh N ruta_al_zip` | Descomprime en `pN/`, copia el CMakeLists genérico, compila | `./nueva_practica.sh 4 ~/Downloads/practica4.zip` |
| 2. Revisar si usa GLEW | `grep -l "glew" pN/*.cpp pN/*.h` | Si imprime archivos, el código viejo usa GLEW y hay que adaptarlo | `grep -l "glew" p4/*.cpp p4/*.h` |
| 3. Cambiar el include | `sed -i 's/PATRON_VIEJO/PATRON_NUEVO/' archivo` | Reemplaza el header de GLEW por el de GLAD | `sed -i 's/#include <GL\/glew.h>/#include <glad\/glad.h>/' p4/*.cpp p4/*.h` |
| 4. Cambiar la inicialización | script de Python con lista `archivos = [...]` | Reemplaza el bloque `glewInit()` por `gladLoadGLLoader(...)` | editar `archivos = ["p4/Main_P4.cpp"]` y correr `python3 fix_glew.py` |
| 5. Compilar | `cd build && cmake .. && make -jN` | Regenera el proyecto y compila todo | `cd build && cmake .. && make -j$(nproc)` |
| 6. Ejecutar | `cd carpeta_binario && ./binario` | Corre desde su propia carpeta (ahí están los shaders copiados) | `cd p4 && ./p4` |
| 7. Iterar código | editar → `make -jN` desde `build/` | Recompila solo lo que cambió, sin repetir `cmake ..` | editar `p4/Main_P4.cpp` → `cd build && make -j$(nproc)` → `cd p4 && ./p4` |

Los pasos 2-4 solo aplican si el zip trae código viejo de GLEW. Si ya viene pensado para GLAD, saltas del paso 1 al 5.

---

## Troubleshooting — sets de comandos

### "No such file or directory" al entrar a una carpeta
```bash
pwd                     # confirma dónde estás parado
ls                      # confirma que la carpeta que buscas existe aquí
```

### Error de CMake: `Target "pN" links to GLEW::GLEW but the target was not found`
Una carpeta `pN` tiene un `CMakeLists.txt` viejo (de antes de este setup). Cámbialo por el genérico:
```bash
cp template/CMakeLists.txt pN/CMakeLists.txt
cd build && cmake .. && make -j$(nproc)
```

### Error de compilación: `undefined reference to __glewXxx` / `glewInit`
El `.cpp` o `Shader.h` de esa práctica todavía incluye GLEW. Revisa cuáles:
```bash
grep -rl "glew" pN/*.cpp pN/*.h
```
Arregla el include:
```bash
sed -i 's/#include <GL\/glew.h>/#include <glad\/glad.h>/' pN/*.cpp pN/*.h
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
Hay un `Shader.h` duplicado dentro de la carpeta de la práctica (`pN/Shader.h`) que se está usando en vez del de `dependencias/`. Revisa cuál toma prioridad y arréglalo también:
```bash
find . -name "Shader.h"
sed -i 's/#include <GL\/glew.h>/#include <glad\/glad.h>/' pN/Shader.h
```

### Corre pero no aparece ninguna ventana / no dibuja nada
Casi siempre es el directorio de trabajo — el binario busca `Shader/core.vs` relativo a donde estás parado, no donde está el `.cpp`:
```bash
cd build/pN     # NO ./build/pN/pN desde la raíz
./pN
```
Verifica que los shaders sí se copiaron ahí:
```bash
ls build/pN/Shader/
```
Si está vacío, revisa que el `CMakeLists.txt` de esa carpeta tenga la línea `file(COPY ... Shader DESTINATION ...)` (viene en el `template/`).

### Quiero recompilar desde cero (por si algo quedó en mal estado)
```bash
cd ~/Documents/CGhci
rm -rf build
mkdir build
cd build
cmake ..
make -j$(nproc)
```

### No sé qué carpetas pN existen ni si compilaron
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
Ejemplo, si el error dice `p4/Main_P4.cpp:48:`:
```bash
sed -n '43,53p' p4/Main_P4.cpp
```

### Confirmar que GLFW y OpenGL están instalados
```bash
pkg-config --modversion glfw3
ldconfig -p | grep libGL
```

### Ver qué versión de OpenGL corre tu GPU realmente
Corre cualquier práctica compilada — casi todas imprimen esto al iniciar (via `glGetString`). Si no, prueba:
```bash
glxinfo | grep "OpenGL version"
```
(si `glxinfo` no existe: `sudo apt install mesa-utils`)

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