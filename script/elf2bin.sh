#!/bin/sh

cd riscv-tests/isa

RISCV_PREFIX=/home/aris/opt/riscv/bin/riscv64-unknown-elf-

for elf in rv64{ui,um,uf,ud,uc}-p-*; do
    if [[ "$elf" == *.bin ]]; then
        continue
    fi

    [ -f "$elf" ] || continue
    [ -x "$elf" ] || continue

    ${RISCV_PREFIX}objcopy \
      -O binary \
      --only-section=.text \
      --only-section=.data \
      --only-section=.rodata \
      "$elf" "$elf.bin"
done
