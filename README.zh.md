中文 | [English](./README.md)

# rvsim

一个 RISC-V 64 用户态模拟器（仅支持 RV64IMFDC 指令集）

- 仅支持 Linux
- 仅支持静态链接的 ELF64 程序
- 纯解释器实现
- 性能优化：computed-goto、基本块缓存、基本块链接

## 快速开始

### 编译

```console
$ cc -o nob nob.c
$ ./nob help
```

### 运行 Lua

```console
$ ./rvsim --elf lua -v
Lua 5.5.0  Copyright (C) 1994-2025 Lua.org, PUC-Rio

$ ./rvsim --elf lua test/lua/lzw.lua --compress src/one.c
u#include "memory.c"u#include "syscall.c"u#include "decode.c"u#include "interp.c"u#include "cache.c"u// #include "codegen.c"u#include "machine.c"u#include "rvsim.c"uu#define NOB_IMPLEMENTATIONu#include "nob.h"
```

## 参考资料

- [RISC-V 指令集手册，第一卷：非特权架构](https://docs.riscv.org/reference/isa/unpriv/unpriv-index.html)
- [ELF 规范 v1.2](https://refspecs.linuxfoundation.org/elf/elf.pdf)
- [rvemu：一个简单快速的 RISC-V JIT 模拟器](https://github.com/ksco/rvemu)
- [QEMU：一个快速可移植的动态翻译器](https://dl.acm.org/doi/10.5555/1247360.1247401)
- [面向虚拟原型的高速解释器指令集模拟](https://ieeexplore.ieee.org/document/10992929)
