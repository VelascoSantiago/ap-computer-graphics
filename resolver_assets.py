#!/usr/bin/env python3
"""uso: python3 resolver_assets.py <carpeta>
Busca en el .cpp las rutas Models/ images/ Shader/ que no existan en <carpeta>
y las trae de la práctica anterior más reciente que las tenga."""
import re
import shutil
import sys
from pathlib import Path

RAIZ = Path(__file__).resolve().parent
IGNORAR = {"build", "template", "dependencias", "teo-models"}

if len(sys.argv) != 2:
    print("uso: python3 resolver_assets.py <carpeta>   (ej: python3 resolver_assets.py p8)")
    sys.exit(1)

destino = (RAIZ / sys.argv[1]).resolve()
if not destino.is_dir():
    print(f"No existe la carpeta {destino}")
    sys.exit(1)


def natural(p):
    return [int(t) if t.isdigit() else t for t in re.split(r"(\d+)", p.name)]


previas = sorted(
    (d for d in RAIZ.iterdir()
     if d.is_dir() and d != destino and d.name not in IGNORAR and not d.name.startswith(".")),
    key=natural, reverse=True,  # p7 antes que p6, etc.
)


def copiar_sin_pisar(origen: Path, rel: Path):
    dst = destino / rel
    if dst.exists():
        return
    dst.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(origen, dst)


def traer(rel: Path, base: Path):
    """Copia rel desde base/; si es un .obj arrastra .mtl y texturas;
    si está dentro de una subcarpeta (Models/Building/x.obj) copia la subcarpeta."""
    src = base / rel
    if len(rel.parts) > 2 and src.parent.is_dir():  # Models/Subcarpeta/archivo
        for f in src.parent.rglob("*"):
            if f.is_file():
                copiar_sin_pisar(f, f.relative_to(base))
        return
    copiar_sin_pisar(src, rel)
    if src.suffix.lower() == ".obj":
        texto = src.read_text(errors="ignore")
        for mtl in re.findall(r"^mtllib\s+(.+)$", texto, re.M):
            mtl_src = src.parent / mtl.strip()
            if mtl_src.exists():
                copiar_sin_pisar(mtl_src, mtl_src.relative_to(base))
                for mapa in re.findall(r"^\s*map_\w+\s+(?:-\S+\s+)*(.+)$",
                                       mtl_src.read_text(errors="ignore"), re.M):
                    tex = mtl_src.parent / mapa.strip()
                    if tex.exists():
                        copiar_sin_pisar(tex, tex.relative_to(base))


rutas = set()
for cpp in destino.glob("*.cpp"):
    texto = cpp.read_text(errors="ignore")
    texto = re.sub(r"/\*.*?\*/", "", texto, flags=re.S)  # comentarios de bloque
    texto = re.sub(r"//[^\n]*", "", texto)               # comentarios de línea
    rutas.update(re.findall(r'"((?:Models|images|Shader)/[^"]+)"', texto))

if not rutas:
    print("No encontré rutas Models/ images/ Shader/ en los .cpp.")

for r in sorted(rutas):
    rel = Path(r)
    if (destino / rel).exists():
        continue
    for prev in previas:
        if (prev / rel).exists():
            traer(rel, prev)
            print(f"copiado: {r}  (desde {prev.name})")
            break
    else:
        print(f"FALTA:   {r}  (no está en ninguna práctica anterior)")