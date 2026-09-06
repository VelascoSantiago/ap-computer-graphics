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
echo "Listo -> revisa los ejecutables generados en build/$DIR/"
ls "$DIR" 2>/dev/null | grep -v CMakeFiles