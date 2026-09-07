#!/bin/bash
# uso:
#   ./nueva_practica.sh <nombre_carpeta> <ruta_al_zip>       -> descomprime el zip ahí
#   ./nueva_practica.sh <nombre_carpeta> <ruta_al_cpp>        -> copia solo ese .cpp
#   ./nueva_practica.sh <nombre_carpeta> <ruta_a_una_carpeta> -> copia el CONTENIDO de esa carpeta
#   ./nueva_practica.sh <nombre_carpeta>                      -> carpeta vacía, solo CMakeLists
#
# ejemplos:
#   ./nueva_practica.sh p4 ~/Downloads/practica4.zip
#   ./nueva_practica.sh mi_proyecto ~/algun_archivo.cpp
#   ./nueva_practica.sh mi_proyecto ~/Downloads/practica4_descomprimida/
#   ./nueva_practica.sh proyecto_final
set -e

DIR=$1
ORIGEN=$2

if [ -z "$DIR" ]; then
    echo "uso: ./nueva_practica.sh <nombre_carpeta> [ruta_al_zip_o_cpp]"
    exit 1
fi

mkdir -p "$DIR"

if [ -z "$ORIGEN" ]; then
    # Modo 3: sin origen -> carpeta vacía, solo con el CMakeLists
    echo "Carpeta '$DIR' creada con CMakeLists.txt genérico (sin .cpp todavía)."
    echo "Recuerda: si no vas a usar shaders, borra la línea 'file(COPY ... Shader ...)' de $DIR/CMakeLists.txt"

elif [[ "$ORIGEN" == *.zip ]]; then
    # Modo 1: viene un zip -> descomprimir (puede traer su propio Shader/, .cpp, etc.)
    unzip -o "$ORIGEN" -d "$DIR"
    echo "Zip descomprimido en '$DIR'."

elif [[ "$ORIGEN" == *.cpp ]]; then
    # Modo 2: viene un .cpp suelto -> copiarlo nada más
    cp "$ORIGEN" "$DIR/"
    echo "Archivo '$ORIGEN' copiado a '$DIR/'."
    echo "Recuerda: si no vas a usar shaders, borra la línea 'file(COPY ... Shader ...)' de $DIR/CMakeLists.txt"

elif [ -d "$ORIGEN" ]; then
    # Modo 4: viene una carpeta -> copiar su CONTENIDO (no la carpeta en sí) hacia $DIR
    # así evitamos terminar con $DIR/nombre_de_la_carpeta_original/archivo.cpp
    cp -r "$ORIGEN"/. "$DIR"/
    echo "Contenido de '$ORIGEN' copiado a '$DIR/'."
    echo "(cualquier CMakeLists.txt que haya traído esa carpeta se reemplaza por el genérico, ver abajo)"

else
    echo "No reconozco el origen '$ORIGEN' (debe ser un .zip, un .cpp, o una carpeta existente)."
    exit 1
fi

# Siempre imponemos el CMakeLists.txt genérico al final, aunque el zip/carpeta haya traído uno
# propio (típicamente viejo, con GLEW en vez de GLAD). Así evitamos el error de
# "Target links to GLEW::GLEW but the target was not found".
cp template/CMakeLists.txt "$DIR/CMakeLists.txt"

# NO compilamos acá a propósito: si el origen trae código con GLEW viejo,
# primero hay que correr fix_glew.py (paso 2 del pipeline). Compilar antes
# de eso produce errores de "undefined reference to __glewXxx".
echo ""
echo "Listo. Antes de compilar, revisa si el código trae GLEW viejo:"
echo "  python3 fix_glew.py $DIR"
echo "Luego compila:"
echo "  cd build && cmake .. && make -j\$(nproc)"