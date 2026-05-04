#!/bin/sh

TEST_DIR="riscv-tests/build/share/riscv-tests/isa"

for elf in "$TEST_DIR"/rv64{ui,um,uf,ud,uc}-p-*; do
    case "$elf" in
        *.bin|*.dump) continue ;;
    esac

    [ -f "$elf" ] || continue

    binfile="${elf}.bin"

    echo "Converting: $elf -> $binfile"

    riscv64-unknown-elf-objcopy \
      -O binary \
      --only-section=.text \
      --only-section=.data \
      --only-section=.rodata \
      "$elf" "$binfile"
done
