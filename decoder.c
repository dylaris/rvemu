#include "decoder.h"

#define QUADRANT(data) (((data) >>  0) & 0x3 )
#define OPCODE(data)   (((data) >>  2) & 0x1f)
#define RD(data)       (((data) >>  7) & 0x1f)
#define RS1(data)      (((data) >> 15) & 0x1f)
#define RS2(data)      (((data) >> 20) & 0x1f)
#define RS3(data)      (((data) >> 27) & 0x1f)
#define FUNCT2(data)   (((data) >> 25) & 0x3 )
#define FUNCT3(data)   (((data) >> 12) & 0x7 )
#define FUNCT7(data)   (((data) >> 25) & 0x7f)

static inline i32 inst__sign_extend(i32 x, u32 n)
{
    return (i32) ((x << (32 - n)) >> (32 - n));
}

static inline Instruction inst__read_rtype(u32 data)
{
    return (Instruction) {
        .rd  = RD(data),
        .rs1 = RS1(data),
        .rs2 = RS1(data),
    };
}

static inline Instruction inst__read_itype(u32 data)
{
    return (Instruction) {
        .rd  = RD(data),
        .rs1 = RS1(data),
        .imm = (i32) ((data >> 20) & 0xfff)
    };
}

static inline Instruction inst__read_stype(u32 data)
{
    u32 imm4_0  = (u32) ((data >>  7) & 0x1f);
    u32 imm11_5 = (u32) ((data >> 25) & 0x7f);
    i32 imm     = (i32) ((imm11_5 << 5) | imm4_0);
    return (Instruction) {
        .rd  = RD(data),
        .rs1 = RS1(data),
        .imm = inst__sign_extend(imm, 12)
    };
}

static inline Instruction inst__read_btype(u32 data)
{
    u32 imm11   = (u32) ((data >>  7) & 0x1);
    u32 imm4_1  = (u32) ((data >>  8) & 0xf);
    u32 imm10_5 = (u32) ((data >> 25) & 0x3f);
    u32 imm12   = (u32) ((data >> 31) & 0x1);
    i32 imm     = (i32) ((imm12 << 12) | (imm11 << 11) | (imm10_5 << 5) | (imm4_1 << 1));
    return (Instruction) {
        .rs1 = RS1(data),
        .rs2 = RS2(data),
        .imm = inst__sign_extend(imm, 13)
    };
}

static inline Instruction inst__read_utype(u32 data)
{
    return (Instruction) {
        .rd  = RD(data),
        .imm = (i32) (data & 0xfffff000)
    };
}

static inline Instruction inst__read_jtype(u32 data)
{
    u32 imm19_12 = (u32) ((data >> 12) & 0xff);
    u32 imm11    = (u32) ((data >> 20) & 0x1);
    u32 imm10_1  = (u32) ((data >> 21) & 0x3ff);
    u32 imm20    = (u32) ((data >> 31) & 0x1);
    i32 imm      = (i32) ((imm20 << 20) | (imm19_12 << 12) | (imm11 << 11) | (imm10_1 << 1));
    return (Instruction) {
        .rd  = RD(data),
        .imm = inst__sign_extend(imm, 21)
    };
}

void inst_decode(Instruction *instp, u32 data)
{
    (void) instp;
    u32 quadrant = QUADRANT(data);
    switch (quadrant) {
    case 0x0: fatal("unimplemented");
    case 0x1: fatal("unimplemented");
    case 0x2: fatal("unimplemented");
    case 0x3: fatal("unimplemented");
    default: unreachable();
    }
}
