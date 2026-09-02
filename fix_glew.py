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
