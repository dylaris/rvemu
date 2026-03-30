#include "interpreter.h"
#include "decoder.h"
#include "mmu.h"

#include <stdlib.h>
#include <math.h>

static inline u64 _mulhu(u64 a, u64 b)
{
    uint64_t t;
    uint32_t y1, y2, y3;
    uint64_t a0 = (uint32_t)a, a1 = a >> 32;
    uint64_t b0 = (uint32_t)b, b1 = b >> 32;
    t = a1*b0 + ((a0*b0) >> 32);
    y1 = t;
    y2 = t >> 32;
    t = a0*b1 + y1;
    y1 = t;
    t = a1*b1 + y2 + (t >> 32);
    y2 = t;
    y3 = t >> 32;
    return ((uint64_t)y3 << 32) | y2;
}

static inline i64 _mulh(i64 a, i64 b)
{
    int negate = (a < 0) != (b < 0);
    uint64_t res = _mulhu(a < 0 ? -a : a, b < 0 ? -b : b);
    return negate ? ~res + (a * b == 0) : res;
}

static inline i64 _mulhsu(i64 a, i64 b)
{
    int negate = a < 0;
    uint64_t res = _mulhu(a < 0 ? -a : a, b);
    return negate ? ~res + (a * b == 0) : res;
}

static inline i64 _div(i64 a, i64 b)
{
    if (b == 0)
        return -1;
    else if (a == INT64_MIN && b == -1)
        return INT64_MIN;
    else
        return a / b;
}

static inline i64 _rem(i64 a, i64 b)
{
    if (b == 0)
        return a;
    else if (a == INT64_MIN && b == -1)
        return 0;
    else
        return a % b;
}

#define F32_SIGN ((uint32_t)1 << 31)
#define F64_SIGN ((uint64_t)1 << 63)

static inline u32 _fsgnj32(u32 a, u32 b, bool n, bool x)
{
    u32 v = x ? a : n ? F32_SIGN : 0;
    return (a & ~F32_SIGN) | ((v ^ b) & F32_SIGN);
}

static inline u64 _fsgnj64(u64 a, u64 b, bool n, bool x)
{
    u64 v = x ? a : n ? F64_SIGN : 0;
    return (a & ~F64_SIGN) | ((v ^ b) & F64_SIGN);
}

static inline u64 _fsgnj(u64 a, u64 b, u64 size, bool neg, bool xor)
{
    if (size == 8)
        return _fsgnj64(a, b, neg, xor);
    else if (size == 4)
        return _fsgnj32(a, b, neg, xor);
    else
        fatal("size must be 4 or 8");
}

union u32_f32 { u32 ui; f32 f; };
#define signF32UI(a) ((bool) ((uint32_t) (a)>>31))
#define expF32UI(a) ((int_fast16_t) ((a)>>23) & 0xFF)
#define fracF32UI(a) ((a) & 0x007FFFFF)
#define isNaNF32UI(a) (((~(a) & 0x7F800000) == 0) && ((a) & 0x007FFFFF))
#define isSigNaNF32UI(uiA) ((((uiA) & 0x7FC00000) == 0x7F800000) && ((uiA) & 0x003FFFFF))

static inline u16 _f32_classify(f32 a)
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
#define signF64UI(a) ((bool) ((uint64_t) (a)>>63))
#define expF64UI(a) ((int_fast16_t) ((a)>>52) & 0x7FF)
#define fracF64UI(a) ((a) & UINT64_C(0x000FFFFFFFFFFFFF))
#define isNaNF64UI(a) (((~(a) & UINT64_C(0x7FF0000000000000)) == 0) && ((a) & UINT64_C(0x000FFFFFFFFFFFFF)))
#define isSigNaNF64UI(uiA) ((((uiA) & UINT64_C(0x7FF8000000000000)) == UINT64_C(0x7FF0000000000000)) && ((uiA) & UINT64_C(0x0007FFFFFFFFFFFF)))

static inline u16 _f64_classify(f64 a)
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

static inline u16 _f_classify(f64 a, u64 size)
{
    if (size == 8)
        return _f64_classify(a);
    else if (size == 4)
        return _f32_classify(a);
    else
        fatal("size must be 4 or 8");
}

#define GEN(name, tag, a1, a2, a3, a4) GEN_##tag(name, a1, a2, a3, a4)

#define GEN_EMPTY(name, a1, a2, a3, a4) \
static void func_##name(CPU *cpup, Inst *instp) \
{ \
    (void) cpup; \
    (void) instp; \
}

#define GEN_LOAD(name, type, a2, a3, a4) \
static void func_##name(CPU *cpup, Inst *instp) \
{ \
    GuestVAddr addr = cpup->gp_regs[instp->rs1] + (i64) instp->imm; \
    cpup->gp_regs[instp->rd] = *(type *) TO_HOST(addr); \
}

#define GEN_OP_IMM(name, expr, a2, a3, a4) \
static void func_##name(CPU *cpup, Inst *instp) \
{ \
    u64 rs1 = cpup->gp_regs[instp->rs1]; \
    i64 imm = (i64) instp->imm; \
    cpup->gp_regs[instp->rd] = (expr); \
}

#define GEN_AUIPC(name, a1, a2, a3, a4) \
static void func_##name(CPU *cpup, Inst *instp) \
{ \
    u64 val = cpup->pc + (i64) instp->imm; \
    cpup->gp_regs[instp->rd] = val; \
}

#define GEN_STORE(name, type, a2, a3, a4) \
static void func_##name(CPU *cpup, Inst *instp) \
{ \
    u64 val  = cpup->gp_regs[instp->rs2]; \
    GuestVAddr addr = cpup->gp_regs[instp->rs1] + (i64) instp->imm; \
    *(type *) TO_HOST(addr) = (type) val; \
}
    // printf("\nkind: %d\nga: 0x%016lx\nha: 0x%016lx\nrs1:%d\nrs2:%d\n", instp->kind, addr, TO_HOST(addr), instp->rs1, instp->rs2);

#define GEN_OP_REG(name, expr, a2, a3, a4) \
static void func_##name(CPU *cpup, Inst *instp) \
{ \
    u64 rs1 = cpup->gp_regs[instp->rs1]; \
    u64 rs2 = cpup->gp_regs[instp->rs2]; \
    cpup->gp_regs[instp->rd] = (expr); \
}

#define GEN_LUI(name, a1, a2, a3, a4) \
static void func_##name(CPU *cpup, Inst *instp) \
{ \
    cpup->gp_regs[instp->rd] = (i64) instp->imm; \
}

#define GEN_BRANCH(name, expr, a2, a3, a4) \
static void func_##name(CPU *cpup, Inst *instp) \
{ \
    u64 rs1 = cpup->gp_regs[instp->rs1]; \
    u64 rs2 = cpup->gp_regs[instp->rs2]; \
    GuestVAddr target_addr = cpup->pc + (i64) instp->imm; \
    if (expr) { \
        cpup->reenter_pc = cpup->pc = target_addr; \
        cpup->brkcode = JUMP; \
        instp->brk = true; \
    } \
}

#define GEN_JUMP(name, addr, a2, a3, a4) \
static void func_##name(CPU *cpup, Inst *instp) \
{ \
    u64 rs1 = cpup->gp_regs[instp->rs1]; \
    i64 imm = (i64) instp->imm; \
    u64 pc = cpup->pc; \
    instp->brk = true; \
    cpup->gp_regs[instp->rd] = pc + (instp->rvc ? 2 : 4); \
    cpup->reenter_pc = (addr); \
    cpup->brkcode = JUMP; \
}

#define GEN_ECALL(name, a1, a2, a3, a4) \
static void func_##name(CPU *cpup, Inst *instp) \
{ \
    instp->brk = true; \
    cpup->reenter_pc = cpup->pc + 4; \
    cpup->brkcode = ECALL; \
}

#define GEN_CSR(name, a1, a2, a3, a4) \
static void func_##name(CPU *cpup, Inst *instp) \
{ \
    u16 csr = (u16) instp->imm; \
    switch (csr) { \
    case FFLAGS: \
    case FRM: \
    case FCSR: \
        break; \
    default: \
        fatal("unsupported csr"); \
    } \
    cpup->gp_regs[instp->rd] = 0; \
}

#define GEN_FP_LOAD(name, type, a2, a3, a4) \
static void func_##name(CPU *cpup, Inst *instp) \
{ \
    GuestVAddr addr = cpup->gp_regs[instp->rs1] + (i64) instp->imm; \
    u64 val = sizeof(type) == 8 ? \
        *(u64 *) TO_HOST(addr) : \
        *(u32 *) TO_HOST(addr) | (-1ULL << 32); \
    cpup->fp_regs[instp->rd].q = val; \
}

#define GEN_FP_STORE(name, type, a2, a3, a4) \
static void func_##name(CPU *cpup, Inst *instp) \
{ \
    GuestVAddr src  = cpup->fp_regs[instp->rs2].q; \
    GuestVAddr dest = cpup->gp_regs[instp->rs1] + (i64) instp->imm; \
    *(type *) TO_HOST(dest) = (type) src; \
}

#define GEN_FP_FMA4(name, view, type, expr, a4) \
static void func_##name(CPU *cpup, Inst *instp) \
{ \
    type rs1 = cpup->fp_regs[instp->rs1].view; \
    type rs2 = cpup->fp_regs[instp->rs2].view; \
    type rs3 = cpup->fp_regs[instp->rs3].view; \
    cpup->fp_regs[instp->rd].view = (type) (expr); \
}

#define GEN_FP_BINOP(name, view, type, expr, a4) \
static void func_##name(CPU *cpup, Inst *instp) \
{ \
    type rs1 = cpup->fp_regs[instp->rs1].view; \
    type rs2 = cpup->fp_regs[instp->rs2].view; \
    cpup->fp_regs[instp->rd].view = (type) (expr); \
    (void) rs2; \
}

#define GEN_FP_SGNJ(name, view, type, neg, xor) \
static void func_##name(CPU *cpup, Inst *instp) \
{ \
    type rs1 = cpup->fp_regs[instp->rs1].view; \
    type rs2 = cpup->fp_regs[instp->rs2].view; \
    cpup->fp_regs[instp->rd].q = _fsgnj(rs1, rs2, sizeof(type), neg, xor); \
}

#define GEN_FP_XFER_F2I(name, view, type, expr, a4) \
static void func_##name(CPU *cpup, Inst *instp) \
{ \
    type src = cpup->fp_regs[instp->rs1].view; \
    cpup->gp_regs[instp->rd] = (expr); \
}

#define GEN_FP_XFER_I2F(name, view, expr, a3, a4) \
static void func_##name(CPU *cpup, Inst *instp) { \
    u64 src = cpup->gp_regs[instp->rs1]; \
    cpup->fp_regs[instp->rd].view = (expr); \
}

#define GEN_FP_XFER_F2F(name, sview, dview, type, expr) \
static void func_##name(CPU *cpup, Inst *instp) { \
    type src = cpup->fp_regs[instp->rs1].sview; \
    cpup->fp_regs[instp->rd].dview = (expr); \
}

#define GEN_FP_CMP(name, view, type, expr, a4) \
static void func_##name(CPU *cpup, Inst *instp) \
{ \
    type rs1 = cpup->fp_regs[instp->rs1].view; \
    type rs2 = cpup->fp_regs[instp->rs2].view; \
    cpup->gp_regs[instp->rd] = (expr); \
}

#define GEN_FP_CLASS(name, view, type, a3, a4) \
static void func_##name(CPU *cpup, Inst *instp) \
{ \
    cpup->gp_regs[instp->rd] = _f_classify(cpup->fp_regs[instp->rs1].view, sizeof(type)); \
}

INSTRUCTIONS(GEN)

#undef GEN // Generate instruction handler

#define GEN(name, tag, a1, a2, a3, a4) [IK_##name] = func_##name,
typedef void (Func)(CPU *, Inst *);
static Func *funcs[] = { INSTRUCTIONS(GEN) };
#undef GEN // Generate dispatch table

static const char *kind_to_string(InstKind kind)
{
#define TO_STRING(name, a0, a1, a2, a3, a4) case IK_##name: return #name;
    switch (kind) {
    INSTRUCTIONS(TO_STRING)
    default:
        unreachable();
    }
#undef TO_STRING
}

void exec_block_interp(CPU *cpup)
{
    Inst inst = {0};
    while (1) {
        // Fetch
        u32 data = *(u32 *) TO_HOST(cpup->pc);

        // Decode
        inst_decode(&inst, data);
        // printf("%s\n", kind_to_string(inst.kind));

        // Execute
        funcs[inst.kind](cpup, &inst);
        cpup->gp_regs[RI_ZERO] = 0; // Writes to x0 are ignored; x0 is hardwired to 0

        // Step
        if (inst.brk)
            break;
        else
            cpup->pc += inst.rvc ? 2 : 4;
    }
}
