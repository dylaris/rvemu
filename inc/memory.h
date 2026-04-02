#ifndef MEMORY_H
#define MEMORY_H

#include "mmu.h"

#include <string.h>

// host_end = TO_HOST(heap_end)
// heap_end = TO_GUEST(host_end)
typedef struct {
    GuestVAddr entry;       // ELF entry point
    HostVAddr  host_end;    // Host mapped region end address
    GuestVAddr heap_base;   // Guest heap base address
    GuestVAddr heap_brk;    // Guest heap program break
    GuestVAddr heap_end;    // Guest heap end address
} Memory;

void mem_load_elf(Memory *mem, FILE *f);
void mem_load_bin(Memory *mem, FILE *f, GuestVAddr base);
GuestVAddr mem_alloc(Memory *mem, i64 size);

static inline void mem_write(GuestVAddr addr, const void *data, u64 len)
{
    memcpy((void *) mmu_to_host(addr), data, len);
}

static inline void mem_read(GuestVAddr addr, void *buf, u64 len)
{
    memcpy(buf, (void *) mmu_to_host(addr), len);
}

#define DECL_MEM_WRITE_X(type) \
static inline void mem_write_##type(GuestVAddr addr, type val) \
{ \
    *(type *) mmu_to_host(addr) = val; \
}

DECL_MEM_WRITE_X(u8)
DECL_MEM_WRITE_X(u16)
DECL_MEM_WRITE_X(u32)
DECL_MEM_WRITE_X(u64)

#define DECL_MEM_READ_X(type) \
static inline type mem_read_##type(GuestVAddr addr) \
{ \
    return *(type *) mmu_to_host(addr); \
}

DECL_MEM_READ_X(u8)
DECL_MEM_READ_X(u16)
DECL_MEM_READ_X(u32)
DECL_MEM_READ_X(u64)
DECL_MEM_READ_X(i8)
DECL_MEM_READ_X(i16)
DECL_MEM_READ_X(i32)
DECL_MEM_READ_X(i64)

#endif // MEMORY_H
