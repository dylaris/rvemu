[中文](./README.zh.md) | English

# rvsim

A user-space simulator for RISC-V 64 (RV64IMFDC only).

- Linux only
- Static-linked ELF64 programs only
- Pure interpreter
- Performance optimizations: computed-goto, basic block cache, and basic block linking

## Quick Start

### Build

```console
$ cc -o nob nob.c
$ ./nob help
```

### Run Lua

```console
$ ./rvsim --elf lua -v
Lua 5.5.0  Copyright (C) 1994-2025 Lua.org, PUC-Rio

$ ./rvsim --elf lua test/lua/lzw.lua --compress src/one.c
u#include "memory.c"u#include "syscall.c"u#include "decode.c"u#include "interp.c"u#include "cache.c"u// #include "codegen.c"u#include "machine.c"u#include "rvsim.c"uu#define NOB_IMPLEMENTATIONu#include "nob.h"
```

## References

- [The RISC-V Instruction Set Manual, Volume I: Unprivileged Architecture](https://docs.riscv.org/reference/isa/unpriv/unpriv-index.html)
- [ELF Specification v1.2](https://refspecs.linuxfoundation.org/elf/elf.pdf)
- [rvemu: A simple and fast RISC-V JIT emulator](https://github.com/ksco/rvemu)
- [QEMU, a fast and portable dynamic translator](https://dl.acm.org/doi/10.5555/1247360.1247401)
- [Fast Interpreter-Based Instruction Set Simulation for Virtual Prototypes](https://ieeexplore.ieee.org/document/10992929)
