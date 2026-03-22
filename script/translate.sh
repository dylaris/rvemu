#!/bin/sh

# Generate pure machine codes

SRC="$1"
BIN="${SRC%.s}.bin"

$HOME/opt/riscv/bin/riscv64-unknown-elf-gcc -c "$SRC" -o tmp.o
$HOME/opt/riscv/bin/riscv64-unknown-elf-objcopy -O binary tmp.o "$BIN"
rm -f tmp.o
