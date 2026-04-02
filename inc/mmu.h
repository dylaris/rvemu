/*
 * Linear MMU Implementation for Emulator Performance Optimization
 *
 * This module implements a lightweight linear MMU to improve emulator performance.
 * We define two concepts:
 * 1. Host program: The emulator itself.
 * 2. Guest program: The program you want to run via the emulator.
 *
 * Typically, the host (emulator) is loaded into the **high region** of its own virtual
 * address space, such as 0x7ffxxxxxxxxx. This leaves the **low region** of virtual
 * memory unused and available.
 *
 * We load the guest program's memory into this **unused low region** (e.g., 0x088800000000).
 * This layout is sufficient for most programs.
 *
 * This design provides two key advantages:
 * 1. Simple address translation
 *    Host and guest share the same virtual memory space. Address translation only
 *    requires adding or subtracting a fixed offset. No complex page table logic
 *    or hardware-like page table walking is needed.
 *
 * 2. Native memory protection
 *    We use mmap() to map guest memory into the host process. Guest memory fully reuses
 *    the host OS's memory protection mechanism.
 *    When loading the guest, we set corresponding page permissions via mmap().
 *    If the guest attempts to write to a read-only page, the host OS will trigger
 *    an error automatically. The guest directly uses the host's memory permission checks.
 */

#ifndef MMU_H
#define MMU_H

#include "common.h"

#ifndef GUEST_MEMORY_OFFSET
#define GUEST_MEMORY_OFFSET 0x088800000000ULL
#endif

typedef u64 HostVAddr;
typedef u64 GuestVAddr;

static inline HostVAddr mmu_to_host(GuestVAddr addr)
{
    return addr + GUEST_MEMORY_OFFSET;
}

static inline GuestVAddr mmu_to_guest(HostVAddr addr)
{
    return addr - GUEST_MEMORY_OFFSET;
}

#endif // MMU_H
