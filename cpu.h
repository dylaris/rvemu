#ifndef CPU_H
#define CPU_H

#include "utils.h"

typedef enum {
    NONE = 0,
    DIRECT_JUMP,
    INDIRECT_JUMP,
    ECALL
} BreakCode;

typedef struct {
    BreakCode brkcode;  // Reason for breaking out of the current block
    u64 gp_regs[32];    // General purpose registers
    u64 pc;
} CPU;

#endif // CPU_H
