#include "interp.h"
#include "decode.h"
#include "trap.h"

#include <math.h>

static inline u64 help_mulhu(u64 a, u64 b)
{
    u64 t;
    uint32_t y1, y2, y3;
    u64 a0 = (uint32_t)a, a1 = a >> 32;
    u64 b0 = (uint32_t)b, b1 = b >> 32;
    t = a1*b0 + ((a0*b0) >> 32);
    y1 = t;
    y2 = t >> 32;
    t = a0*b1 + y1;
    y1 = t;
    t = a1*b1 + y2 + (t >> 32);
    y2 = t;
    y3 = t >> 32;
    return ((u64)y3 << 32) | y2;
}

static inline i64 help_mulh(i64 a, i64 b)
{
    int negate = (a < 0) != (b < 0);
    u64 res = help_mulhu(a < 0 ? -a : a, b < 0 ? -b : b);
    return negate ? ~res + (a * b == 0) : res;
}

static inline i64 help_mulhsu(i64 a, i64 b)
{
    int negate = a < 0;
    u64 res = help_mulhu(a < 0 ? -a : a, b);
    return negate ? ~res + (a * b == 0) : res;
}

static inline i64 help_div(i64 a, i64 b)
{
    if (b == 0)
        return -1;
    else if (a == INT64_MIN && b == -1)
        return INT64_MIN;
    else
        return a / b;
}

static inline i64 help_rem(i64 a, i64 b)
{
    if (b == 0)
        return a;
    else if (a == INT64_MIN && b == -1)
        return 0;
    else
        return a % b;
}

#define F32_SIGN ((uint32_t)1 << 31)
#define F64_SIGN ((u64)1 << 63)

static inline u32 help_fsgnj32(u32 a, u32 b, bool n, bool x)
{
    u32 v = x ? a : n ? F32_SIGN : 0;
    return (a & ~F32_SIGN) | ((v ^ b) & F32_SIGN);
}

static inline u64 help_fsgnj64(u64 a, u64 b, bool n, bool x)
{
    u64 v = x ? a : n ? F64_SIGN : 0;
    return (a & ~F64_SIGN) | ((v ^ b) & F64_SIGN);
}

static inline u64 help_fsgnj(u64 a, u64 b, u64 size, bool neg, bool xor)
{
    if (size == 8)
        return help_fsgnj64(a, b, neg, xor);
    else if (size == 4)
        return help_fsgnj32(a, b, neg, xor);
    else
        fatal("size must be 4 or 8");
}

union u32_f32 { u32 ui; f32 f; };
#define signF32UI(a) ((bool) ((uint32_t) (a)>>31))
#define expF32UI(a) ((int_fast16_t) ((a)>>23) & 0xFF)
#define fracF32UI(a) ((a) & 0x007FFFFF)
#define isNaNF32UI(a) (((~(a) & 0x7F800000) == 0) && ((a) & 0x007FFFFF))
#define isSigNaNF32UI(uiA) ((((uiA) & 0x7FC00000) == 0x7F800000) && ((uiA) & 0x003FFFFF))

static inline u16 help_f32_classify(f32 a)
{
    union u32_f32 uA;
    u32 uiA;

    uA.f = a;
    uiA = uA.ui;

    u16 infOrNaN = expF32UI(uiA) == 0xFF;
    u16 subnormalOrZero = expF32UI(uiA) == 0;
    bool sign = signF32UI(uiA);
    bool fracZero = fracF32UI(uiA) == 0;
    bool isNaN = isNaNF32UI(uiA);
    bool isSNaN = isSigNaNF32UI(uiA);

    return
        (sign && infOrNaN && fracZero)          << 0 |
        (sign && !infOrNaN && !subnormalOrZero) << 1 |
        (sign && subnormalOrZero && !fracZero)  << 2 |
        (sign && subnormalOrZero && fracZero)   << 3 |
        (!sign && infOrNaN && fracZero)          << 7 |
        (!sign && !infOrNaN && !subnormalOrZero) << 6 |
        (!sign && subnormalOrZero && !fracZero)  << 5 |
        (!sign && subnormalOrZero && fracZero)   << 4 |
        (isNaN &&  isSNaN)                       << 8 |
        (isNaN && !isSNaN)                       << 9;
}

union u64_f64 { u64 ui; f64 f; };
#define signF64UI(a) ((bool) ((u64) (a)>>63))
#define expF64UI(a) ((int_fast16_t) ((a)>>52) & 0x7FF)
#define fracF64UI(a) ((a) & UINT64_C(0x000FFFFFFFFFFFFF))
#define isNaNF64UI(a) (((~(a) & UINT64_C(0x7FF0000000000000)) == 0) && ((a) & UINT64_C(0x000FFFFFFFFFFFFF)))
#define isSigNaNF64UI(uiA) ((((uiA) & UINT64_C(0x7FF8000000000000)) == UINT64_C(0x7FF0000000000000)) && ((uiA) & UINT64_C(0x0007FFFFFFFFFFFF)))

static inline u16 help_f64_classify(f64 a)
{
    union u64_f64 uA;
    u64 uiA;

    uA.f = a;
    uiA = uA.ui;

    u16 infOrNaN = expF64UI(uiA) == 0x7FF;
    u16 subnormalOrZero = expF64UI(uiA) == 0;
    bool sign = signF64UI(uiA);
    bool fracZero = fracF64UI(uiA) == 0;
    bool isNaN = isNaNF64UI(uiA);
    bool isSNaN = isSigNaNF64UI(uiA);

    return
        (sign && infOrNaN && fracZero)          << 0 |
        (sign && !infOrNaN && !subnormalOrZero) << 1 |
        (sign && subnormalOrZero && !fracZero)  << 2 |
        (sign && subnormalOrZero && fracZero)   << 3 |
        (!sign && infOrNaN && fracZero)          << 7 |
        (!sign && !infOrNaN && !subnormalOrZero) << 6 |
        (!sign && subnormalOrZero && !fracZero)  << 5 |
        (!sign && subnormalOrZero && fracZero)   << 4 |
        (isNaN &&  isSNaN)                       << 8 |
        (isNaN && !isSNaN)                       << 9;
}

static inline u16 help_f_classify(f64 a, u64 size)
{
    if (size == 8)
        return help_f64_classify(a);
    else if (size == 4)
        return help_f32_classify(a);
    else
        fatal("size must be 4 or 8");
}

#define GEN(name, tag, a1, a2, a3, a4) GEN_##tag(name, a1, a2, a3, a4)

#define GEN_EMPTY(name, a1, a2, a3, a4) \
static void interp__##name(CPUState *state, Instr *instr) \
{ \
    (void) state; \
    (void) instr; \
}

#define GEN_LOAD(name, type, a2, a3, a4) \
static void interp__##name(CPUState *state, Instr *instr) \
{ \
    GuestVAddr addr = cpu_get_gpr(state, instr->rs1) + (i64) instr->imm; \
    cpu_set_gpr(state, instr->rd, mem_read_##type(addr)); \
}

#define GEN_OP_IMM(name, expr, a2, a3, a4) \
static void interp__##name(CPUState *state, Instr *instr) \
{ \
    u64 rs1 = cpu_get_gpr(state, instr->rs1); \
    i64 imm = (i64) instr->imm; \
    cpu_set_gpr(state, instr->rd, (expr)); \
}

#define GEN_AUIPC(name, a1, a2, a3, a4) \
static void interp__##name(CPUState *state, Instr *instr) \
{ \
    u64 val = state->pc + (i64) instr->imm; \
    cpu_set_gpr(state, instr->rd, val); \
}

#define GEN_STORE(name, type, a2, a3, a4) \
static void interp__##name(CPUState *state, Instr *instr) \
{ \
    u64 val  = cpu_get_gpr(state, instr->rs2); \
    GuestVAddr addr = cpu_get_gpr(state, instr->rs1) + (i64) instr->imm; \
    mem_write_##type(addr, (type) val); \
}

#define GEN_OP_REG(name, expr, a2, a3, a4) \
static void interp__##name(CPUState *state, Instr *instr) \
{ \
    u64 rs1 = cpu_get_gpr(state, instr->rs1); \
    u64 rs2 = cpu_get_gpr(state, instr->rs2); \
    cpu_set_gpr(state, instr->rd, (expr)); \
}

#define GEN_LUI(name, a1, a2, a3, a4) \
static void interp__##name(CPUState *state, Instr *instr) \
{ \
    cpu_set_gpr(state, instr->rd, (i64) instr->imm); \
}

#define GEN_BRANCH(name, expr, a2, a3, a4) \
static void interp__##name(CPUState *state, Instr *instr) \
{ \
    u64 rs1 = cpu_get_gpr(state, instr->rs1); \
    u64 rs2 = cpu_get_gpr(state, instr->rs2); \
    GuestVAddr addr = state->pc + (i64) instr->imm; \
    if (expr) { \
        instr->exit_block = true; \
        state->trace.exit_reason = BLOCK_JUMP; \
        state->trace.target_pc = addr; \
    } \
}

#define GEN_JUMP(name, addr, a2, a3, a4) \
static void interp__##name(CPUState *state, Instr *instr) \
{ \
    u64 rs1 = cpu_get_gpr(state, instr->rs1); \
    i64 imm = (i64) instr->imm; \
    u64 pc = state->pc; \
    cpu_set_gpr(state, instr->rd, pc + (instr->rvc ? 2 : 4)); \
    instr->exit_block = true; \
    state->trace.exit_reason = BLOCK_JUMP; \
    state->trace.target_pc = (addr); \
    (void) rs1; \
}

#define GEN_ECALL(name, a1, a2, a3, a4) \
static void interp__##name(CPUState *state, Instr *instr) \
{ \
    instr->exit_block = true; \
    state->trace.exit_reason = BLOCK_ECALL; \
    state->trace.target_pc = state->pc + 4; \
    trap_throw(TRAP_ECALL_U, state->pc); \
}

#define GEN_CSR(name, a1, a2, a3, a4) \
static void interp__##name(CPUState *state, Instr *instr) \
{ \
    u16 csr = (u16) instr->imm; \
    switch (csr) { \
    case CSR_FFLAGS: \
    case CSR_FRM: \
    case CSR_FCSR: \
        break; \
    default: \
        fatal("unsupported csr"); \
    } \
    cpu_set_gpr(state, instr->rd, 0); \
}

#define GEN_FP_LOAD(name, type, a2, a3, a4) \
static void interp__##name(CPUState *state, Instr *instr) \
{ \
    GuestVAddr addr = cpu_get_gpr(state, instr->rs1) + (i64) instr->imm; \
    u64 val = sizeof(type) == 8 ? \
        mem_read_u64(addr) : \
        mem_read_u32(addr) | (-1ULL << 32); \
    cpu_set_fpr_q(state, instr->rd, val); \
}

#define GEN_FP_STORE(name, type, a2, a3, a4) \
static void interp__##name(CPUState *state, Instr *instr) \
{ \
    u64 src = cpu_get_fpr_q(state, instr->rs2); \
    GuestVAddr addr = cpu_get_gpr(state, instr->rs1) + (i64) instr->imm; \
    mem_write_##type(addr, (type) src); \
}

#define GEN_FP_FMA4(name, view, type, expr, a4) \
static void interp__##name(CPUState *state, Instr *instr) \
{ \
    type rs1 = cpu_get_fpr_##view(state, instr->rs1); \
    type rs2 = cpu_get_fpr_##view(state, instr->rs2); \
    type rs3 = cpu_get_fpr_##view(state, instr->rs3); \
    cpu_set_fpr_##view(state, instr->rd, (type) (expr)); \
}

#define GEN_FP_BINOP(name, view, type, expr, a4) \
static void interp__##name(CPUState *state, Instr *instr) \
{ \
    type rs1 = cpu_get_fpr_##view(state, instr->rs1); \
    type rs2 = cpu_get_fpr_##view(state, instr->rs2); \
    state->fp_regs[instr->rd].view = (type) (expr); \
    cpu_set_fpr_##view(state, instr->rd, (type) (expr)); \
    (void) rs2; \
}

#define GEN_FP_SGNJ(name, view, type, neg, xor) \
static void interp__##name(CPUState *state, Instr *instr) \
{ \
    type rs1 = cpu_get_fpr_##view(state, instr->rs1); \
    type rs2 = cpu_get_fpr_##view(state, instr->rs2); \
    cpu_set_fpr_q(state, instr->rd, help_fsgnj(rs1, rs2, sizeof(type), neg, xor)); \
}

#define GEN_FP_XFER_F2I(name, view, type, expr, a4) \
static void interp__##name(CPUState *state, Instr *instr) \
{ \
    type src = cpu_get_fpr_##view(state, instr->rs1); \
    cpu_set_gpr(state, instr->rd, (expr)); \
}

#define GEN_FP_XFER_I2F(name, view, expr, a3, a4) \
static void interp__##name(CPUState *state, Instr *instr) \
{ \
    u64 src = cpu_get_gpr(state, instr->rs1); \
    cpu_set_fpr_##view(state, instr->rd, (expr)); \
}

#define GEN_FP_XFER_F2F(name, sview, dview, type, expr) \
static void interp__##name(CPUState *state, Instr *instr) \
{ \
    type src = cpu_get_fpr_##sview(state, instr->rs1); \
    cpu_set_fpr_##dview(state, instr->rd, (expr)); \
}

#define GEN_FP_CMP(name, view, type, expr, a4) \
static void interp__##name(CPUState *state, Instr *instr) \
{ \
    type rs1 = cpu_get_fpr_##view(state, instr->rs1); \
    type rs2 = cpu_get_fpr_##view(state, instr->rs2); \
    cpu_set_gpr(state, instr->rd, (expr)); \
}

#define GEN_FP_CLASS(name, view, type, a3, a4) \
static void interp__##name(CPUState *state, Instr *instr) \
{ \
    cpu_set_gpr(state, instr->rd, help_f_classify(state->fp_regs[instr->rs1].view, sizeof(type))); \
}

INSTRUCTION_LIST(GEN)

#undef GEN // Generate instruction handler

#define X(name, tag, a1, a2, a3, a4) [INSTR_##name] = interp__##name,
typedef void (*InterpFunc)(CPUState *, Instr *);
static InterpFunc funcs[] = { INSTRUCTION_LIST(X) };
#undef X // Generate dispatch table

void interp_block(CPUState *state)
{
    Instr instr = {0};

    while (1) {
        u32 raw = mem_read_u32(state->pc);

        if (!decode_instr(raw, &instr))
            trap_throw(TRAP_ILLEGAL_INSTR, state->pc);

        funcs[instr.kind](state, &instr);

        if (instr.exit_block)
            break;
        else
            state->pc += instr.rvc ? 2 : 4;
    }
}
