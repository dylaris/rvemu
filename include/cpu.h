#ifndef CPU_H
#define CPU_H

#include "reg.h"

typedef enum {
    BLOCK_NONE = 0,
    BLOCK_JUMP,
    BLOCK_ECALL,
    BLOCK_ERROR,
} BlockExitReason;

typedef struct {
    BlockExitReason exit_reason;
    u64 target_pc;
} Trace;

typedef struct {
    Trace trace;
    GPR gp_regs[NUM_GPRS];
    FPR fp_regs[NUM_FPRS];
    u64 pc;
} CPUState;

static inline void cpu_clean_trace(CPUState *state)
{
    state->trace = (Trace) {
        .exit_reason = BLOCK_NONE,
        .target_pc = 0,
    };
}

static inline void cpu_set_gpr(CPUState *state, GPRIndex reg, GPR val)
{
    assert(reg >= 0 && reg < NUM_GPRS);
    if (reg == GPR_ZERO)
        return;
    state->gp_regs[reg] = val;
}

static inline GPR cpu_get_gpr(const CPUState *state, GPRIndex reg)
{
    assert(reg >= 0 && reg < NUM_GPRS);
    if (reg == GPR_ZERO)
        return 0;
    return state->gp_regs[reg];
}

static inline void cpu_set_fpr(CPUState *state, FPRIndex reg, FPR val)
{
    assert(reg >= 0 && reg < NUM_FPRS);
    state->fp_regs[reg] = val;
}

static inline FPR cpu_get_fpr(const CPUState *state, FPRIndex reg)
{
    assert(reg >= 0 && reg < NUM_FPRS);
    return state->fp_regs[reg];
}

static inline void cpu_set_fpr_q(CPUState *state, FPRIndex reg, u64 val)
{
    assert(reg >= 0 && reg < NUM_FPRS);
    state->fp_regs[reg].q = val;
}

static inline u64 cpu_get_fpr_q(const CPUState *state, FPRIndex reg)
{
    assert(reg >= 0 && reg < NUM_FPRS);
    return state->fp_regs[reg].q;
}

static inline void cpu_set_fpr_w(CPUState *state, FPRIndex reg, u32 val)
{
    assert(reg >= 0 && reg < NUM_FPRS);
    state->fp_regs[reg].w = val;
}

static inline u32 cpu_get_fpr_w(const CPUState *state, FPRIndex reg)
{
    assert(reg >= 0 && reg < NUM_FPRS);
    return state->fp_regs[reg].w;
}

static inline void cpu_set_fpr_d(CPUState *state, FPRIndex reg, f64 val)
{
    assert(reg >= 0 && reg < NUM_FPRS);
    state->fp_regs[reg].d = val;
}

static inline f64 cpu_get_fpr_d(const CPUState *state, FPRIndex reg)
{
    assert(reg >= 0 && reg < NUM_FPRS);
    return state->fp_regs[reg].d;
}

static inline void cpu_set_fpr_s(CPUState *state, FPRIndex reg, f32 val)
{
    assert(reg >= 0 && reg < NUM_FPRS);
    state->fp_regs[reg].s = val;
}

static inline f32 cpu_get_fpr_s(const CPUState *state, FPRIndex reg)
{
    assert(reg >= 0 && reg < NUM_FPRS);
    return state->fp_regs[reg].s;
}

#endif // CPU_H
