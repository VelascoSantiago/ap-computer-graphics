#!/bin/bash
# uso: ./nueva_practica.sh <numero> <ruta_al_zip>
set -e
NUM=$1
ZIP=$2
DIR="p$NUM"

mkdir -p "$DIR"
unzip -o "$ZIP" -d "$DIR"
cp template/CMakeLists.txt "$DIR/CMakeLists.txt"

cd build
cmake ..
make "$DIR" -j"$(nproc)"

echo ""
echo "Listo -> ejecuta: ./build/$DIR/$DIR"
