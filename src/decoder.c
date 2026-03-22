#include "decoder.h"

#define QUADRANT(data) (((data) >>  0) & 0x3 )
#define OPCODE(data)   (((data) >>  0) & 0x7f)
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

static inline Inst inst__decode_rtype(InstKind kind, u32 data)
{
    return (Inst) {
        .kind = kind,
        .rd   = RD(data),
        .rs1  = RS1(data),
        .rs2  = RS2(data),
    };
}

static inline Inst inst__decode_itype(InstKind kind, u32 data)
{
    i32 imm = (i32) ((data >> 20) & 0xfff);
    return (Inst) {
        .kind = kind,
        .rd   = RD(data),
        .rs1  = RS1(data),
        .imm  = inst__sign_extend(imm, 12)
    };
}

static inline Inst inst__decode_stype(InstKind kind, u32 data)
{
    u32 imm4_0  = (u32) ((data >>  7) & 0x1f);
    u32 imm11_5 = (u32) ((data >> 25) & 0x7f);
    i32 imm     = (i32) ((imm11_5 << 5) | imm4_0);
    return (Inst) {
        .kind = kind,
        .rs1  = RS1(data),
        .rs2  = RS2(data),
        .imm  = inst__sign_extend(imm, 12)
    };
}

static inline Inst inst__decode_btype(InstKind kind, u32 data)
{
    u32 imm11   = (u32) ((data >>  7) & 0x1);
    u32 imm4_1  = (u32) ((data >>  8) & 0xf);
    u32 imm10_5 = (u32) ((data >> 25) & 0x3f);
    u32 imm12   = (u32) ((data >> 31) & 0x1);
    i32 imm     = (i32) ((imm12 << 12) | (imm11 << 11) | (imm10_5 << 5) | (imm4_1 << 1));
    return (Inst) {
        .kind = kind,
        .rs1  = RS1(data),
        .rs2  = RS2(data),
        .imm  = inst__sign_extend(imm, 13)
    };
}

static inline Inst inst__decode_utype(InstKind kind, u32 data)
{
    return (Inst) {
        .kind = kind,
        .rd   = RD(data),
        .imm  = (i32) (data & 0xfffff000)
    };
}

static inline Inst inst__decode_jtype(InstKind kind, u32 data)
{
    u32 imm19_12 = (u32) ((data >> 12) & 0xff);
    u32 imm11    = (u32) ((data >> 20) & 0x1);
    u32 imm10_1  = (u32) ((data >> 21) & 0x3ff);
    u32 imm20    = (u32) ((data >> 31) & 0x1);
    i32 imm      = (i32) ((imm20 << 20) | (imm19_12 << 12) | (imm11 << 11) | (imm10_1 << 1));
    return (Inst) {
        .kind = kind,
        .rd   = RD(data),
        .imm  = inst__sign_extend(imm, 21)
    };
}

#define IGNORE 0xff
static InstDef inst_table[] = {
    { "add",   IK_ADD,   0x33, 0x0, 0x00,   inst__decode_rtype },
    { "sub",   IK_SUB,   0x33, 0x0, 0x20,   inst__decode_rtype },
    { "xor",   IK_XOR,   0x33, 0x4, 0x00,   inst__decode_rtype },
    { "or",    IK_OR,    0x33, 0x6, 0x00,   inst__decode_rtype },
    { "and",   IK_AND,   0x33, 0x7, 0x00,   inst__decode_rtype },
    { "sll",   IK_SLL,   0x33, 0x1, 0x00,   inst__decode_rtype },
    { "srl",   IK_SRL,   0x33, 0x5, 0x00,   inst__decode_rtype },
    { "sra",   IK_SRA,   0x33, 0x5, 0x20,   inst__decode_rtype },
    { "slt",   IK_SLT,   0x33, 0x2, 0x00,   inst__decode_rtype },
    { "sltu",  IK_SLTU,  0x33, 0x3, 0x00,   inst__decode_rtype },
    { "addi",  IK_ADDI,  0x13, 0x0, IGNORE, inst__decode_itype },
    { "xori",  IK_XORI,  0x13, 0x4, IGNORE, inst__decode_itype },
    { "ori",   IK_ORI,   0x13, 0x6, IGNORE, inst__decode_itype },
    { "andi",  IK_ANDI,  0x13, 0x7, IGNORE, inst__decode_itype },
    { "slli",  IK_SLLI,  0x13, 0x1, 0x00,   inst__decode_itype },
    { "srli",  IK_SRLI,  0x13, 0x5, 0x00,   inst__decode_itype },
    { "srai",  IK_SRAI,  0x13, 0x5, 0x20,   inst__decode_itype },
    { "slti",  IK_SLTI,  0x13, 0x2, IGNORE, inst__decode_itype },
    { "sltiu", IK_SLTIU, 0x13, 0x3, IGNORE, inst__decode_itype },
    { "lb",    IK_LB,    0x03, 0x0, IGNORE, inst__decode_itype },
    { "lh",    IK_LH,    0x03, 0x1, IGNORE, inst__decode_itype },
    { "lw",    IK_LW,    0x03, 0x2, IGNORE, inst__decode_itype },
    { "lbu",   IK_LBU,   0x03, 0x4, IGNORE, inst__decode_itype },
    { "lhu",   IK_LHU,   0x03, 0x5, IGNORE, inst__decode_itype },
    { "sb",    IK_SB,    0x23, 0x0, IGNORE, inst__decode_stype },
    { "sh",    IK_SH,    0x23, 0x1, IGNORE, inst__decode_stype },
    { "sw",    IK_SW,    0x23, 0x2, IGNORE, inst__decode_stype },
    { "beq",   IK_BEQ,   0x63, 0x0, IGNORE, inst__decode_btype },
    { "bne",   IK_BNE,   0x63, 0x1, IGNORE, inst__decode_btype },
    { "blt",   IK_BLT,   0x63, 0x4, IGNORE, inst__decode_btype },
    { "bge",   IK_BGE,   0x63, 0x5, IGNORE, inst__decode_btype },
    { "bltu",  IK_BLTU,  0x63, 0x6, IGNORE, inst__decode_btype },
    { "bgeu",  IK_BGEU,  0x63, 0x7, IGNORE, inst__decode_btype },
    { NULL, 0, 0, 0, 0, NULL },
};

void inst_decode(Inst *instp, u32 data)
{
    InstDef def = inst_lookup(data);
    *instp = def.decode(def.kind, data);
}

InstDef inst_lookup(u32 data)
{
    u8 opcode = OPCODE(data);
    u8 funct3 = FUNCT3(data);
    u8 funct7 = FUNCT7(data);

    for (InstDef *defp = inst_table; defp->name; defp++) {
        if (defp->opcode == opcode && defp->funct3 == funct3 &&
           (defp->funct7 == funct7 || defp->funct7 == IGNORE)) {
            return *defp;
        }
    }

    fatalf("failed to decode: %d(opcode), %d(funct3), %d(funct7)", opcode, funct3, funct7);
}
