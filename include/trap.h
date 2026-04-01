#ifndef TRAP_H
#define TRAP_H

#include "common.h"
#include <setjmp.h>

typedef enum {
    TRAP_NONE = 0,
    TRAP_ECALL_U = 8,
    TRAP_ILLEGAL_INSTR = 2,
    TRAP_LOAD_MISALIGN = 4,
    TRAP_STORE_MISALIGN = 6,
    TRAP_LOAD_FAULT = 5,
    TRAP_STORE_FAULT = 7,
    TRAP_CRASH = 99,
} TrapKind;

typedef struct {
    TrapKind kind;
    GuestVAddr fault_addr;
} Trap;

void trap_init(void);
void trap_enter(jmp_buf *buf);
void trap_leave(void);
Trap trap_get(void);

#endif // TRAP_H
