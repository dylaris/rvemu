#!/bin/sh

# Compile C file (RISCV64)

SRC="$1"
EXE="${SRC%.c}"

$HOME/opt/riscv/bin/riscv64-unknown-elf-gcc -static -march=rv64gc -mabi=lp64d -o "$EXE" "$SRC"
