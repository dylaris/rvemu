#!/bin/bash

# ==============================
# Configuration
# ==============================
PROJ_DIR="/home/aris/project/rvsim"
EMU="${PROJ_DIR}/rvsim"
LUA="${PROJ_DIR}/bin/lua"
LZW_SCRIPT="${PROJ_DIR}/test/lua/lzw.lua"
INPUT_DIR="${PROJ_DIR}/tmp"
OUTPUT_DIR="${PROJ_DIR}/tmp"

# ==============================
# Process each .txt file
# ==============================
for txt in "$INPUT_DIR"/*.txt; do
    [ -f "$txt" ] || continue

    name=$(basename "$txt")
    out="${OUTPUT_DIR}/${name%.txt}.txt.lzw"

    echo "Processing: $name"
    $EMU --elf "$LUA" "$LZW_SCRIPT" --compress "$txt" > "$out"

    if [ $? -eq 0 ]; then
        echo "  -> $out [OK]"
    else
        echo "  -> Failed"
    fi
done

echo "Done."
