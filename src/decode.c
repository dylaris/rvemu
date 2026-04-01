#include "decode.h"
#include "cpu.h"

#include <string.h>

#define QUADRANT(raw)   (((raw) >>  0) & 0x3)

#define OPCODE(raw)     (((raw) >>  0) & 0x7f)
#define RD(raw)         (((raw) >>  7) & 0x1f)
#define RS1(raw)        (((raw) >> 15) & 0x1f)
#define RS2(raw)        (((raw) >> 20) & 0x1f)
#define RS3(raw)        (((raw) >> 27) & 0x1f)
#define FUNCT2(raw)     (((raw) >> 25) & 0x3 )
#define FUNCT3(raw)     (((raw) >> 12) & 0x7 )
#define FUNCT5HIGH(raw) (((raw) >> 27) & 0x1f)
#define FUNCT5LOW(raw)  (((raw) >> 20) & 0x1f)
#define FUNCT6(raw)     (((raw) >> 26) & 0x3f)
#define FUNCT7(raw)     (((raw) >> 25) & 0x7f)
#define FUNCT12(raw)    (((raw) >> 20) & 0xfff)

#define COPCODE(raw)     (((raw) >>  0)  & 0x3)
#define CRD(raw)         (((raw) >>  7)  & 0x1f)
#define CRS1(raw)        (((raw) >>  7)  & 0x1f)
#define CRS2(raw)        (((raw) >>  2)  & 0x1f)
#define CRP(raw,h,l)     (((raw) >> (l)) & 0x7)
#define CFUNCT1(raw)     (((raw) >> 12)  & 0x1)
#define CFUNCT2HIGH(raw) (((raw) >> 10)  & 0x3)
#define CFUNCT2LOW(raw)  (((raw) >>  5)  & 0x3)
#define CFUNCT3(raw)     (((raw) >> 13)  & 0x7)
#define CFUNCT5HIGH(raw) (((raw) >>  7)  & 0x1f)
#define CFUNCT5LOW(raw)  (((raw) >>  2)  & 0x1f)

static inline i32 decode__sign_extend(i32 x, u32 n)
{
    return (i32) ((x << (32 - n)) >> (32 - n));
}

static inline Instr decode__blank(const InstrInfo *info, u32 raw)
{
    (void) raw;
    return (Instr) {
        .kind = info->kind,
    };
}

static inline Instr decode__crtype(const InstrInfo *info, u32 raw)
{
    Instr instr = {
        .kind = info->kind,
        .rd   = CRD(raw),
        .rs1  = CRS1(raw),
        .rs2  = CRS2(raw),
        .rvc  = true,
    };
    if (strcmp(info->name, "c.jr") == 0)
        instr.rd = GPR_ZERO;
    else if (strcmp(info->name, "c.mv") == 0)
        instr.rs1 = GPR_ZERO;
    else if (strcmp(info->name, "c.jalr") == 0)
        instr.rd = GPR_RA;
    return instr;
}

static inline Instr decode__citype(const InstrInfo *info, u32 raw)
{
    u32 imm4_0 = (u32) ((raw >>  2) & 0x1f);
    u32 imm5   = (u32) ((raw >> 12) & 0x1);
    i32 imm    = (i32) ((imm5 << 5) | imm4_0);
    Instr instr = {
        .kind = info->kind,
        .rd   = CRD(raw),
        .rs1  = CRS1(raw),
        .imm  = decode__sign_extend(imm, 6),
        .rvc  = true,
    };
    if (strcmp(info->name, "c.li") == 0)
        instr.rs1 = GPR_ZERO;
    return instr;
}

static inline Instr decode__citype2(const InstrInfo *info, u32 raw)
{
    u32 imm8_6 = (u32) ((raw >>  2) & 0x7);
    u32 imm4_3 = (u32) ((raw >>  5) & 0x3);
    u32 imm5   = (u32) ((raw >> 12) & 0x1);
    i32 imm    = (i32) ((imm8_6 << 6) | (imm4_3 << 3) | (imm5 << 5));
    Instr instr = {
        .kind = info->kind,
        .rd   = CRD(raw),
        .rs1  = CRS1(raw),
        .imm  = imm,
        .rvc  = true,
    };
    if (strcmp(info->name, "c.fldsp") == 0 || strcmp(info->name, "c.ldsp") == 0)
        instr.rs1 = GPR_SP;
    return instr;
}

static inline Instr decode__citype3(const InstrInfo *info, u32 raw)
{
    u32 imm5   = (u32) ((raw >>  2) & 0x1);
    u32 imm8_7 = (u32) ((raw >>  3) & 0x3);
    u32 imm6   = (u32) ((raw >>  5) & 0x1);
    u32 imm4   = (u32) ((raw >>  6) & 0x1);
    u32 imm9   = (u32) ((raw >> 12) & 0x1);
    i32 imm    = (i32) ((imm5 << 5) | (imm8_7 << 7) | (imm6 << 6) | (imm4 << 4) | (imm9 << 9));
    return (Instr) {
        .kind = info->kind,
        .rd   = CRD(raw),
        .rs1  = CRS1(raw),
        .imm  = decode__sign_extend(imm, 10),
        .rvc  = true,
    };
}

static inline Instr decode__citype4(const InstrInfo *info, u32 raw)
{
    u32 imm5   = (u32) ((raw >> 12) & 0x1);
    u32 imm4_2 = (u32) ((raw >>  4) & 0x7);
    u32 imm7_6 = (u32) ((raw >>  2) & 0x3);
    i32 imm    = (i32) ((imm5 << 5) | (imm4_2 << 2) | (imm7_6 << 6));
    Instr instr = {
        .kind = info->kind,
        .rd   = CRD(raw),
        .rs1  = CRS1(raw),
        .imm  = imm,
        .rvc  = true,
    };
    if (strcmp(info->name, "c.lwsp") == 0)
        instr.rs1 = GPR_SP;
    return instr;
}

static inline Instr decode__citype5(const InstrInfo *info, u32 raw)
{
    u32 imm16_12 = (u32) ((raw >>  2) & 0x1f);
    u32 imm17    = (u32) ((raw >> 12) & 0x1);
    i32 imm      = (i32) ((imm16_12 << 12) | (imm17 << 17));
    return (Instr) {
        .kind = info->kind,
        .rd   = CRD(raw),
        .rs1  = CRS1(raw),
        .imm  = decode__sign_extend(imm, 18),
        .rvc  = true,
    };
}

static inline Instr decode__csstype(const InstrInfo *info, u32 raw)
{
    u32 imm8_6 = (u32) ((raw >>  7) & 0x7);
    u32 imm5_3 = (u32) ((raw >> 10) & 0x7);
    i32 imm    = (i32) ((imm8_6 << 6) | (imm5_3 << 3));
    Instr instr = {
        .kind = info->kind,
        .rs2  = CRS2(raw),
        .imm  = imm,
        .rvc  = true,
    };
    if (strcmp(info->name, "c.fsdsp") == 0 || strcmp(info->name, "c.sdsp") == 0)
        instr.rs1 = GPR_SP;
    return instr;
}

static inline Instr decode__csstype2(const InstrInfo *info, u32 raw)
{
    u32 imm7_6 = (u32) ((raw >> 7) & 0x3);
    u32 imm5_2 = (u32) ((raw >> 9) & 0xf);
    i32 imm    = (i32) ((imm7_6 << 6) | (imm5_2 << 2));
    Instr instr = {
        .kind = info->kind,
        .rs2  = CRS2(raw),
        .imm  = imm,
        .rvc  = true,
    };
    if (strcmp(info->name, "c.swsp") == 0)
        instr.rs1 = GPR_SP;
    return instr;
}

static inline Instr decode__ciwtype(const InstrInfo *info, u32 raw)
{
    u32 imm3   = (u32) ((raw >>  5) & 0x1);
    u32 imm2   = (u32) ((raw >>  6) & 0x1);
    u32 imm9_6 = (u32) ((raw >>  7) & 0xf);
    u32 imm5_4 = (u32) ((raw >> 11) & 0x3);
    i32 imm    = (i32) ((imm3 << 3) | (imm2 << 2) | (imm9_6 << 6) | (imm5_4 << 4));
    Instr instr = {
        .kind = info->kind,
        .imm  = imm,
        .rd   = CRP(raw,4,2) + 8,
        .rvc  = true,
    };
    if (strcmp(info->name, "c.addi4spn") == 0)
        instr.rs1 = GPR_SP;
    return instr;
}

static inline Instr decode__cltype(const InstrInfo *info, u32 raw)
{
    u32 imm6   = (u32) ((raw >>  5) & 0x1);
    u32 imm2   = (u32) ((raw >>  6) & 0x1);
    u32 imm5_3 = (u32) ((raw >> 10) & 0x7);
    i32 imm    = (i32) ((imm6 << 6) | (imm2 << 2) | (imm5_3 << 3));
    return (Instr) {
        .kind = info->kind,
        .rd   = CRP(raw,4,2) + 8,
        .rs1  = CRP(raw,9,7) + 8,
        .imm  = imm,
        .rvc  = true,
    };
}

static inline Instr decode__cltype2(const InstrInfo *info, u32 raw)
{
    u32 imm7_6 = (u32) ((raw >>  5) & 0x3);
    u32 imm5_3 = (u32) ((raw >> 10) & 0x7);
    i32 imm    = (i32) ((imm7_6 << 6) | (imm5_3 << 3));
    return (Instr) {
        .kind = info->kind,
        .rd   = CRP(raw,4,2) + 8,
        .rs1  = CRP(raw,9,7) + 8,
        .imm  = imm,
        .rvc  = true,
    };
}

static inline Instr decode__cstype(const InstrInfo *info, u32 raw)
{
    u32 imm7_6 = (u32) ((raw >>  5) & 0x3);
    u32 imm5_3 = (u32) ((raw >> 10) & 0x7);
    i32 imm    = (i32) (((imm7_6 << 6) | (imm5_3 << 3)));
    return (Instr) {
        .kind = info->kind,
        .rs1  = CRP(raw,9,7) + 8,
        .rs2  = CRP(raw,4,2) + 8,
        .imm  = imm,
        .rvc  = true,
    };
}

static inline Instr decode__cstype2(const InstrInfo *info, u32 raw)
{
    u32 imm6   = (u32) ((raw >>  5) & 0x1);
    u32 imm2   = (u32) ((raw >>  6) & 0x1);
    u32 imm5_3 = (u32) ((raw >> 10) & 0x7);
    i32 imm    = (i32) (((imm6 << 6) | (imm2 << 2) | (imm5_3 << 3)));
    return (Instr) {
        .kind = info->kind,
        .rs1  = CRP(raw,9,7) + 8,
        .rs2  = CRP(raw,4,2) + 8,
        .imm  = imm,
        .rvc  = true,
    };
}

static inline Instr decode__catype(const InstrInfo *info, u32 raw)
{
    return (Instr) {
        .kind = info->kind,
        .rd   = CRP(raw,9,7) + 8,
        .rs1  = CRP(raw,9,7) + 8,
        .rs2  = CRP(raw,4,2) + 8,
        .rvc  = true,
    };
}

static inline Instr decode__cbtype(const InstrInfo *info, u32 raw)
{
    u32 imm5   = (u32) ((raw >>  2) & 0x1);
    u32 imm2_1 = (u32) ((raw >>  3) & 0x3);
    u32 imm7_6 = (u32) ((raw >>  5) & 0x3);
    u32 imm4_3 = (u32) ((raw >> 10) & 0x3);
    u32 imm8   = (u32) ((raw >> 12) & 0x1);
    i32 imm    = (i32) ((imm8 << 8) | (imm7_6 << 6) | (imm5 << 5) | (imm4_3 << 3) | (imm2_1 << 1));
    Instr instr = {
        .kind = info->kind,
        .rd   = CRP(raw,9,7) + 8,
        .rs1  = CRP(raw,9,7) + 8,
        .imm  = decode__sign_extend(imm, 9),
        .rvc  = true,
    };
    if (strcmp(info->name, "c.beqz") == 0 || strcmp(info->name, "c.bnez") == 0)
        instr.rs2 = GPR_ZERO;
    return instr;
}

static inline Instr decode__cbtype2(const InstrInfo *info, u32 raw)
{
    u32 imm4_0 = (u32) ((raw >>  2) & 0x1f);
    u32 imm5   = (u32) ((raw >> 12) & 0x1);
    i32 imm    = (i32) ((imm5 << 5) | imm4_0);
    return (Instr) {
        .kind = info->kind,
        .rd   = CRP(raw,9,7) + 8,
        .rs1  = CRP(raw,9,7) + 8,
        .imm  = decode__sign_extend(imm, 6),
        .rvc  = true,
    };
}

static inline Instr decode__cjtype(const InstrInfo *info, u32 raw)
{
    u32 imm5   = (u32) ((raw >>  2) & 0x1);
    u32 imm3_1 = (u32) ((raw >>  3) & 0x7);
    u32 imm7   = (u32) ((raw >>  6) & 0x1);
    u32 imm6   = (u32) ((raw >>  7) & 0x1);
    u32 imm10  = (u32) ((raw >>  8) & 0x1);
    u32 imm9_8 = (u32) ((raw >>  9) & 0x3);
    u32 imm4   = (u32) ((raw >> 11) & 0x1);
    u32 imm11  = (u32) ((raw >> 12) & 0x1);
    i32 imm    = (i32) (((imm5  << 5)  | (imm3_1 << 1) | (imm7 << 7) | (imm6 << 6) |
                         (imm10 << 10) | (imm9_8 << 8) | (imm4 << 4) | (imm11 << 11)));
    Instr instr = {
        .kind = info->kind,
        .imm  = decode__sign_extend(imm, 12),
        .rvc  = true,
    };
    if (strcmp(info->name, "c.j") == 0)
        instr.rd = GPR_ZERO;
    return instr;
}

static inline Instr decode__rtype(const InstrInfo *info, u32 raw)
{
    return (Instr) {
        .kind = info->kind,
        .rd   = RD(raw),
        .rs1  = RS1(raw),
        .rs2  = RS2(raw),
    };
}

static inline Instr decode__r4type(const InstrInfo *info, u32 raw)
{
    return (Instr) {
        .kind = info->kind,
        .rd   = RD(raw),
        .rs1  = RS1(raw),
        .rs2  = RS2(raw),
        .rs3  = RS3(raw),
    };
}

static inline Instr decode__itype(const InstrInfo *info, u32 raw)
{
    i32 imm = (i32) ((raw >> 20) & 0xfff);
    return (Instr) {
        .kind = info->kind,
        .rd   = RD(raw),
        .rs1  = RS1(raw),
        .imm  = decode__sign_extend(imm, 12)
    };
}

static inline Instr decode__stype(const InstrInfo *info, u32 raw)
{
    u32 imm4_0  = (u32) ((raw >>  7) & 0x1f);
    u32 imm11_5 = (u32) ((raw >> 25) & 0x7f);
    i32 imm     = (i32) ((imm11_5 << 5) | imm4_0);
    return (Instr) {
        .kind = info->kind,
        .rs1  = RS1(raw),
        .rs2  = RS2(raw),
        .imm  = decode__sign_extend(imm, 12)
    };
}

static inline Instr decode__btype(const InstrInfo *info, u32 raw)
{
    u32 imm11   = (u32) ((raw >>  7) & 0x1);
    u32 imm4_1  = (u32) ((raw >>  8) & 0xf);
    u32 imm10_5 = (u32) ((raw >> 25) & 0x3f);
    u32 imm12   = (u32) ((raw >> 31) & 0x1);
    i32 imm     = (i32) ((imm12 << 12) | (imm11 << 11) | (imm10_5 << 5) | (imm4_1 << 1));
    return (Instr) {
        .kind = info->kind,
        .rs1  = RS1(raw),
        .rs2  = RS2(raw),
        .imm  = decode__sign_extend(imm, 13)
    };
}

static inline Instr decode__utype(const InstrInfo *info, u32 raw)
{
    return (Instr) {
        .kind = info->kind,
        .rd   = RD(raw),
        .imm  = (i32) raw & 0xfffff000
    };
}

static inline Instr decode__jtype(const InstrInfo *info, u32 raw)
{
    u32 imm19_12 = (u32) ((raw >> 12) & 0xff);
    u32 imm11    = (u32) ((raw >> 20) & 0x1);
    u32 imm10_1  = (u32) ((raw >> 21) & 0x3ff);
    u32 imm20    = (u32) ((raw >> 31) & 0x1);
    i32 imm      = (i32) ((imm20 << 20) | (imm19_12 << 12) | (imm11 << 11) | (imm10_1 << 1));
    return (Instr) {
        .kind = info->kind,
        .rd   = RD(raw),
        .imm  = decode__sign_extend(imm, 21)
    };
}

#define IGNORE 0xffff
#define IGNORE_RVC  IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE
#define IGNORE_NRVC IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE

static InstrInfo nrvc_instr_table[] = {
    /* name        kind             opc   f2      f3      f5h     f5l     f6      f7      f12     ...         decode       */
    { "lb",        INSTR_lb,        0x03, IGNORE, 0x0,    IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__itype },
    { "lh",        INSTR_lh,        0x03, IGNORE, 0x1,    IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__itype },
    { "lw",        INSTR_lw,        0x03, IGNORE, 0x2,    IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__itype },
    { "ld",        INSTR_ld,        0x03, IGNORE, 0x3,    IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__itype },
    { "lbu",       INSTR_lbu,       0x03, IGNORE, 0x4,    IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__itype },
    { "lhu",       INSTR_lhu,       0x03, IGNORE, 0x5,    IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__itype },
    { "lwu",       INSTR_lwu,       0x03, IGNORE, 0x6,    IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__itype },
    { "flw",       INSTR_flw,       0x07, IGNORE, 0x2,    IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__itype },
    { "fld",       INSTR_fld,       0x07, IGNORE, 0x3,    IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__itype },
    { "fence",     INSTR_fence,     0x0f, IGNORE, 0x0,    IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__blank },
    { "fence.i",   INSTR_fence_i,   0x0f, IGNORE, 0x1,    IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__blank },
    { "addi",      INSTR_addi,      0x13, IGNORE, 0x0,    IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__itype },
    { "slli",      INSTR_slli,      0x13, IGNORE, 0x1,    IGNORE, IGNORE, 0x00,   IGNORE, IGNORE, IGNORE_RVC, decode__itype },
    { "slti",      INSTR_slti,      0x13, IGNORE, 0x2,    IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__itype },
    { "sltiu",     INSTR_sltiu,     0x13, IGNORE, 0x3,    IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__itype },
    { "xori",      INSTR_xori,      0x13, IGNORE, 0x4,    IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__itype },
    { "srli",      INSTR_srli,      0x13, IGNORE, 0x5,    IGNORE, IGNORE, 0x00,   IGNORE, IGNORE, IGNORE_RVC, decode__itype },
    { "srai",      INSTR_srai,      0x13, IGNORE, 0x5,    IGNORE, IGNORE, 0x10,   IGNORE, IGNORE, IGNORE_RVC, decode__itype },
    { "ori",       INSTR_ori,       0x13, IGNORE, 0x6,    IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__itype },
    { "andi",      INSTR_andi,      0x13, IGNORE, 0x7,    IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__itype },
    { "auipc",     INSTR_auipc,     0x17, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__utype },
    { "addiw",     INSTR_addiw,     0x1b, IGNORE, 0x0,    IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__itype },
    { "slliw",     INSTR_slliw,     0x1b, IGNORE, 0x1,    IGNORE, IGNORE, IGNORE, 0x00,   IGNORE, IGNORE_RVC, decode__itype },
    { "srliw",     INSTR_srliw,     0x1b, IGNORE, 0x5,    IGNORE, IGNORE, IGNORE, 0x00,   IGNORE, IGNORE_RVC, decode__itype },
    { "sraiw",     INSTR_sraiw,     0x1b, IGNORE, 0x5,    IGNORE, IGNORE, IGNORE, 0x20,   IGNORE, IGNORE_RVC, decode__itype },
    { "sb",        INSTR_sb,        0x23, IGNORE, 0x0,    IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__stype },
    { "sh",        INSTR_sh,        0x23, IGNORE, 0x1,    IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__stype },
    { "sw",        INSTR_sw,        0x23, IGNORE, 0x2,    IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__stype },
    { "sd",        INSTR_sd,        0x23, IGNORE, 0x3,    IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__stype },
    { "fsw",       INSTR_fsw,       0x27, IGNORE, 0x2,    IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__stype },
    { "fsd",       INSTR_fsd,       0x27, IGNORE, 0x3,    IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__stype },
    // { "amoadd.w",  INSTR_amoadd_w,  0x2f, IGNORE, 0x2,    0x00,   IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__rtype },
    // { "amoswap.w", INSTR_amoswap_W, 0x2f, IGNORE, 0x2,    0x01,   IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__rtype },
    // { "lr.w",      INSTR_lr_w,      0x2f, IGNORE, 0x2,    0x02,   IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__rtype },
    // { "sc.w",      INSTR_sc_w,      0x2f, IGNORE, 0x2,    0x03,   IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__rtype },
    // { "amoxor.w",  INSTR_amoxor_w,  0x2f, IGNORE, 0x2,    0x04,   IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__rtype },
    // { "amoor.w",   INSTR_amoor_w,   0x2f, IGNORE, 0x2,    0x08,   IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__rtype },
    // { "amoand.w",  INSTR_amoand_w,  0x2f, IGNORE, 0x2,    0x0c,   IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__rtype },
    // { "amomin.w",  INSTR_amomin_w,  0x2f, IGNORE, 0x2,    0x10,   IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__rtype },
    // { "amomax.w",  INSTR_amomax_w,  0x2f, IGNORE, 0x2,    0x14,   IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__rtype },
    // { "amominu.w", INSTR_amominu_W, 0x2f, IGNORE, 0x2,    0x18,   IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__rtype },
    // { "amomaxu.w", INSTR_amomaxu_W, 0x2f, IGNORE, 0x2,    0x1c,   IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__rtype },
    // { "amoadd.d",  INSTR_amoadd_d,  0x2f, IGNORE, 0x3,    0x00,   IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__rtype },
    // { "amoswap.d", INSTR_amoswap_D, 0x2f, IGNORE, 0x3,    0x01,   IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__rtype },
    // { "lr.d",      INSTR_lr_d,      0x2f, IGNORE, 0x3,    0x02,   0x00,   IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__rtype },
    // { "sc.d",      INSTR_sc_d,      0x2f, IGNORE, 0x3,    0x03,   IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__rtype },
    // { "amoxor.d",  INSTR_amoxor_d,  0x2f, IGNORE, 0x3,    0x04,   IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__rtype },
    // { "amoor.d",   INSTR_amoor_d,   0x2f, IGNORE, 0x3,    0x08,   IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__rtype },
    // { "amoand.d",  INSTR_amoand_d,  0x2f, IGNORE, 0x3,    0x0c,   IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__rtype },
    // { "amomin.d",  INSTR_amomin_d,  0x2f, IGNORE, 0x3,    0x10,   IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__rtype },
    // { "amomax.d",  INSTR_amomax_d,  0x2f, IGNORE, 0x3,    0x14,   IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__rtype },
    // { "amominu.d", INSTR_amominu_D, 0x2f, IGNORE, 0x3,    0x18,   IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__rtype },
    // { "amomaxu.d", INSTR_amomaxu_D, 0x2f, IGNORE, 0x3,    0x1c,   IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__rtype },
    { "add",       INSTR_add,       0x33, IGNORE, 0x0,    IGNORE, IGNORE, IGNORE, 0x00,   IGNORE, IGNORE_RVC, decode__rtype },
    { "mul",       INSTR_mul,       0x33, IGNORE, 0x0,    IGNORE, IGNORE, IGNORE, 0x01,   IGNORE, IGNORE_RVC, decode__rtype },
    { "sub",       INSTR_sub,       0x33, IGNORE, 0x0,    IGNORE, IGNORE, IGNORE, 0x20,   IGNORE, IGNORE_RVC, decode__rtype },
    { "sll",       INSTR_sll,       0x33, IGNORE, 0x1,    IGNORE, IGNORE, IGNORE, 0x00,   IGNORE, IGNORE_RVC, decode__rtype },
    { "mulh",      INSTR_mulh,      0x33, IGNORE, 0x1,    IGNORE, IGNORE, IGNORE, 0x01,   IGNORE, IGNORE_RVC, decode__rtype },
    { "slt",       INSTR_slt,       0x33, IGNORE, 0x2,    IGNORE, IGNORE, IGNORE, 0x00,   IGNORE, IGNORE_RVC, decode__rtype },
    { "mulhsu",    INSTR_mulhsu,    0x33, IGNORE, 0x2,    IGNORE, IGNORE, IGNORE, 0x01,   IGNORE, IGNORE_RVC, decode__rtype },
    { "sltu",      INSTR_sltu,      0x33, IGNORE, 0x3,    IGNORE, IGNORE, IGNORE, 0x00,   IGNORE, IGNORE_RVC, decode__rtype },
    { "mulhu",     INSTR_mulhu,     0x33, IGNORE, 0x3,    IGNORE, IGNORE, IGNORE, 0x01,   IGNORE, IGNORE_RVC, decode__rtype },
    { "xor",       INSTR_xor,       0x33, IGNORE, 0x4,    IGNORE, IGNORE, IGNORE, 0x00,   IGNORE, IGNORE_RVC, decode__rtype },
    { "div",       INSTR_div,       0x33, IGNORE, 0x4,    IGNORE, IGNORE, IGNORE, 0x01,   IGNORE, IGNORE_RVC, decode__rtype },
    { "srl",       INSTR_srl,       0x33, IGNORE, 0x5,    IGNORE, IGNORE, IGNORE, 0x00,   IGNORE, IGNORE_RVC, decode__rtype },
    { "divu",      INSTR_divu,      0x33, IGNORE, 0x5,    IGNORE, IGNORE, IGNORE, 0x01,   IGNORE, IGNORE_RVC, decode__rtype },
    { "sra",       INSTR_sra,       0x33, IGNORE, 0x5,    IGNORE, IGNORE, IGNORE, 0x20,   IGNORE, IGNORE_RVC, decode__rtype },
    { "or",        INSTR_or,        0x33, IGNORE, 0x6,    IGNORE, IGNORE, IGNORE, 0x00,   IGNORE, IGNORE_RVC, decode__rtype },
    { "rem",       INSTR_rem,       0x33, IGNORE, 0x6,    IGNORE, IGNORE, IGNORE, 0x01,   IGNORE, IGNORE_RVC, decode__rtype },
    { "and",       INSTR_and,       0x33, IGNORE, 0x7,    IGNORE, IGNORE, IGNORE, 0x00,   IGNORE, IGNORE_RVC, decode__rtype },
    { "remu",      INSTR_remu,      0x33, IGNORE, 0x7,    IGNORE, IGNORE, IGNORE, 0x01,   IGNORE, IGNORE_RVC, decode__rtype },
    { "lui",       INSTR_lui,       0x37, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__utype },
    { "addw",      INSTR_addw,      0x3b, IGNORE, 0x0,    IGNORE, IGNORE, IGNORE, 0x00,   IGNORE, IGNORE_RVC, decode__rtype },
    { "mulw",      INSTR_mulw,      0x3b, IGNORE, 0x0,    IGNORE, IGNORE, IGNORE, 0x01,   IGNORE, IGNORE_RVC, decode__rtype },
    { "subw",      INSTR_subw,      0x3b, IGNORE, 0x0,    IGNORE, IGNORE, IGNORE, 0x20,   IGNORE, IGNORE_RVC, decode__rtype },
    { "sllw",      INSTR_sllw,      0x3b, IGNORE, 0x1,    IGNORE, IGNORE, IGNORE, 0x00,   IGNORE, IGNORE_RVC, decode__rtype },
    { "divw",      INSTR_divw,      0x3b, IGNORE, 0x4,    IGNORE, IGNORE, IGNORE, 0x01,   IGNORE, IGNORE_RVC, decode__rtype },
    { "srlw",      INSTR_srlw,      0x3b, IGNORE, 0x5,    IGNORE, IGNORE, IGNORE, 0x00,   IGNORE, IGNORE_RVC, decode__rtype },
    { "divuw",     INSTR_divuw,     0x3b, IGNORE, 0x5,    IGNORE, IGNORE, IGNORE, 0x01,   IGNORE, IGNORE_RVC, decode__rtype },
    { "sraw",      INSTR_sraw,      0x3b, IGNORE, 0x5,    IGNORE, IGNORE, IGNORE, 0x20,   IGNORE, IGNORE_RVC, decode__rtype },
    { "remw",      INSTR_remw,      0x3b, IGNORE, 0x6,    IGNORE, IGNORE, IGNORE, 0x01,   IGNORE, IGNORE_RVC, decode__rtype },
    { "remuw",     INSTR_remuw,     0x3b, IGNORE, 0x7,    IGNORE, IGNORE, IGNORE, 0x01,   IGNORE, IGNORE_RVC, decode__rtype },
    { "fmadd.s",   INSTR_fmadd_s,   0x43, 0x0,    IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__r4type },
    { "fmadd.d",   INSTR_fmadd_d,   0x43, 0x1,    IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__r4type },
    { "fmsub.s",   INSTR_fmsub_s,   0x47, 0x0,    IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__r4type },
    { "fmsub.d",   INSTR_fmsub_d,   0x47, 0x1,    IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__r4type },
    { "fnmsub.s",  INSTR_fnmsub_s,  0x4b, 0x0,    IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__r4type },
    { "fnmsub.d",  INSTR_fnmsub_d,  0x4b, 0x1,    IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__r4type },
    { "fnmadd.s",  INSTR_fnmadd_s,  0x4f, 0x0,    IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__r4type },
    { "fnmadd.d",  INSTR_fnmadd_d,  0x4f, 0x1,    IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__r4type },
    { "fcvt.d.wu", INSTR_fcvt_d_wu, 0x53, IGNORE, 0x0,    IGNORE, 0x01,   IGNORE, 0x69,   IGNORE, IGNORE_RVC, decode__rtype },
    { "fmv.x.w",   INSTR_fmv_x_w,   0x53, IGNORE, 0x0,    IGNORE, 0x00,   IGNORE, 0x70,   IGNORE, IGNORE_RVC, decode__rtype },
    { "fmv.x.d",   INSTR_fmv_x_d,   0x53, IGNORE, 0x0,    IGNORE, 0x00,   IGNORE, 0x71,   IGNORE, IGNORE_RVC, decode__rtype },
    { "fmv.w.x",   INSTR_fmv_w_x,   0x53, IGNORE, 0x0,    IGNORE, 0x00,   IGNORE, 0x78,   IGNORE, IGNORE_RVC, decode__rtype },
    { "fmv.d.x",   INSTR_fmv_d_x,   0x53, IGNORE, 0x0,    IGNORE, 0x00,   IGNORE, 0x79,   IGNORE, IGNORE_RVC, decode__rtype },
    { "fsgnj.s",   INSTR_fsgnj_s,   0x53, IGNORE, 0x0,    IGNORE, IGNORE, IGNORE, 0x10,   IGNORE, IGNORE_RVC, decode__rtype },
    { "fsgnj.d",   INSTR_fsgnj_d,   0x53, IGNORE, 0x0,    IGNORE, IGNORE, IGNORE, 0x11,   IGNORE, IGNORE_RVC, decode__rtype },
    { "fmin.s",    INSTR_fmin_s,    0x53, IGNORE, 0x0,    IGNORE, IGNORE, IGNORE, 0x14,   IGNORE, IGNORE_RVC, decode__rtype },
    { "fmin.d",    INSTR_fmin_d,    0x53, IGNORE, 0x0,    IGNORE, IGNORE, IGNORE, 0x15,   IGNORE, IGNORE_RVC, decode__rtype },
    { "fle.s",     INSTR_fle_s,     0x53, IGNORE, 0x0,    IGNORE, IGNORE, IGNORE, 0x50,   IGNORE, IGNORE_RVC, decode__rtype },
    { "fle.d",     INSTR_fle_d,     0x53, IGNORE, 0x0,    IGNORE, IGNORE, IGNORE, 0x51,   IGNORE, IGNORE_RVC, decode__rtype },
    { "fclass.s",  INSTR_fclass_s,  0x53, IGNORE, 0x1,    IGNORE, 0x00,   IGNORE, 0x70,   IGNORE, IGNORE_RVC, decode__rtype },
    { "fclass.d",  INSTR_fclass_d,  0x53, IGNORE, 0x1,    IGNORE, 0x00,   IGNORE, 0x71,   IGNORE, IGNORE_RVC, decode__rtype },
    { "fsgnjn.s",  INSTR_fsgnjn_s,  0x53, IGNORE, 0x1,    IGNORE, IGNORE, IGNORE, 0x10,   IGNORE, IGNORE_RVC, decode__rtype },
    { "fsgnjn.d",  INSTR_fsgnjn_d,  0x53, IGNORE, 0x1,    IGNORE, IGNORE, IGNORE, 0x11,   IGNORE, IGNORE_RVC, decode__rtype },
    { "fmax.s",    INSTR_fmax_s,    0x53, IGNORE, 0x1,    IGNORE, IGNORE, IGNORE, 0x14,   IGNORE, IGNORE_RVC, decode__rtype },
    { "fmax.d",    INSTR_fmax_d,    0x53, IGNORE, 0x1,    IGNORE, IGNORE, IGNORE, 0x15,   IGNORE, IGNORE_RVC, decode__rtype },
    { "flt.s",     INSTR_flt_s,     0x53, IGNORE, 0x1,    IGNORE, IGNORE, IGNORE, 0x50,   IGNORE, IGNORE_RVC, decode__rtype },
    { "flt.d",     INSTR_flt_d,     0x53, IGNORE, 0x1,    IGNORE, IGNORE, IGNORE, 0x51,   IGNORE, IGNORE_RVC, decode__rtype },
    { "fsgnjx.s",  INSTR_fsgnjx_s,  0x53, IGNORE, 0x2,    IGNORE, IGNORE, IGNORE, 0x10,   IGNORE, IGNORE_RVC, decode__rtype },
    { "fsgnjx.d",  INSTR_fsgnjx_d,  0x53, IGNORE, 0x2,    IGNORE, IGNORE, IGNORE, 0x11,   IGNORE, IGNORE_RVC, decode__rtype },
    { "feq.s",     INSTR_feq_s,     0x53, IGNORE, 0x2,    IGNORE, IGNORE, IGNORE, 0x50,   IGNORE, IGNORE_RVC, decode__rtype },
    { "feq.d",     INSTR_feq_d,     0x53, IGNORE, 0x2,    IGNORE, IGNORE, IGNORE, 0x51,   IGNORE, IGNORE_RVC, decode__rtype },
    { "fcvt.d.s",  INSTR_fcvt_d_s,  0x53, IGNORE, IGNORE, IGNORE, 0x00,   IGNORE, 0x21,   IGNORE, IGNORE_RVC, decode__rtype },
    { "fsqrt.s",   INSTR_fsqrt_s,   0x53, IGNORE, IGNORE, IGNORE, 0x00,   IGNORE, 0x2c,   IGNORE, IGNORE_RVC, decode__rtype },
    { "fsqrt.d",   INSTR_fsqrt_d,   0x53, IGNORE, IGNORE, IGNORE, 0x00,   IGNORE, 0x2d,   IGNORE, IGNORE_RVC, decode__rtype },
    { "fcvt.w.s",  INSTR_fcvt_w_s,  0x53, IGNORE, IGNORE, IGNORE, 0x00,   IGNORE, 0x60,   IGNORE, IGNORE_RVC, decode__rtype },
    { "fcvt.w.d",  INSTR_fcvt_w_d,  0x53, IGNORE, IGNORE, IGNORE, 0x00,   IGNORE, 0x61,   IGNORE, IGNORE_RVC, decode__rtype },
    { "fcvt.s.w",  INSTR_fcvt_s_w,  0x53, IGNORE, IGNORE, IGNORE, 0x00,   IGNORE, 0x68,   IGNORE, IGNORE_RVC, decode__rtype },
    { "fcvt.d.w",  INSTR_fcvt_d_w,  0x53, IGNORE, IGNORE, IGNORE, 0x00,   IGNORE, 0x69,   IGNORE, IGNORE_RVC, decode__rtype },
    { "fcvt.s.d",  INSTR_fcvt_s_d,  0x53, IGNORE, IGNORE, IGNORE, 0x01,   IGNORE, 0x20,   IGNORE, IGNORE_RVC, decode__rtype },
    { "fcvt.wu.s", INSTR_fcvt_wu_s, 0x53, IGNORE, IGNORE, IGNORE, 0x01,   IGNORE, 0x60,   IGNORE, IGNORE_RVC, decode__rtype },
    { "fcvt.wu.d", INSTR_fcvt_wu_d, 0x53, IGNORE, IGNORE, IGNORE, 0x01,   IGNORE, 0x61,   IGNORE, IGNORE_RVC, decode__rtype },
    { "fcvt.s.wu", INSTR_fcvt_s_wu, 0x53, IGNORE, IGNORE, IGNORE, 0x01,   IGNORE, 0x68,   IGNORE, IGNORE_RVC, decode__rtype },
    { "fcvt.l.s",  INSTR_fcvt_l_s,  0x53, IGNORE, IGNORE, IGNORE, 0x02,   IGNORE, 0x60,   IGNORE, IGNORE_RVC, decode__rtype },
    { "fcvt.l.d",  INSTR_fcvt_l_d,  0x53, IGNORE, IGNORE, IGNORE, 0x02,   IGNORE, 0x61,   IGNORE, IGNORE_RVC, decode__rtype },
    { "fcvt.s.l",  INSTR_fcvt_s_l,  0x53, IGNORE, IGNORE, IGNORE, 0x02,   IGNORE, 0x68,   IGNORE, IGNORE_RVC, decode__rtype },
    { "fcvt.d.l",  INSTR_fcvt_d_l,  0x53, IGNORE, IGNORE, IGNORE, 0x02,   IGNORE, 0x69,   IGNORE, IGNORE_RVC, decode__rtype },
    { "fcvt.lu.s", INSTR_fcvt_lu_s, 0x53, IGNORE, IGNORE, IGNORE, 0x03,   IGNORE, 0x60,   IGNORE, IGNORE_RVC, decode__rtype },
    { "fcvt.lu.d", INSTR_fcvt_lu_d, 0x53, IGNORE, IGNORE, IGNORE, 0x03,   IGNORE, 0x61,   IGNORE, IGNORE_RVC, decode__rtype },
    { "fcvt.s.lu", INSTR_fcvt_s_lu, 0x53, IGNORE, IGNORE, IGNORE, 0x03,   IGNORE, 0x68,   IGNORE, IGNORE_RVC, decode__rtype },
    { "fcvt.d.lu", INSTR_fcvt_d_lu, 0x53, IGNORE, IGNORE, IGNORE, 0x03,   IGNORE, 0x69,   IGNORE, IGNORE_RVC, decode__rtype },
    { "fadd.s",    INSTR_fadd_s,    0x53, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, 0x00,   IGNORE, IGNORE_RVC, decode__rtype },
    { "fadd.d",    INSTR_fadd_d,    0x53, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, 0x01,   IGNORE, IGNORE_RVC, decode__rtype },
    { "fsub.s",    INSTR_fsub_s,    0x53, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, 0x04,   IGNORE, IGNORE_RVC, decode__rtype },
    { "fsub.d",    INSTR_fsub_d,    0x53, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, 0x05,   IGNORE, IGNORE_RVC, decode__rtype },
    { "fmul.s",    INSTR_fmul_s,    0x53, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, 0x08,   IGNORE, IGNORE_RVC, decode__rtype },
    { "fmul.d",    INSTR_fmul_d,    0x53, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, 0x09,   IGNORE, IGNORE_RVC, decode__rtype },
    { "fdiv.s",    INSTR_fdiv_s,    0x53, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, 0x0c,   IGNORE, IGNORE_RVC, decode__rtype },
    { "fdiv.d",    INSTR_fdiv_d,    0x53, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, 0x0d,   IGNORE, IGNORE_RVC, decode__rtype },
    { "beq",       INSTR_beq,       0x63, IGNORE, 0x0,    IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__btype },
    { "bne",       INSTR_bne,       0x63, IGNORE, 0x1,    IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__btype },
    { "blt",       INSTR_blt,       0x63, IGNORE, 0x4,    IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__btype },
    { "bge",       INSTR_bge,       0x63, IGNORE, 0x5,    IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__btype },
    { "bltu",      INSTR_bltu,      0x63, IGNORE, 0x6,    IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__btype },
    { "bgeu",      INSTR_bgeu,      0x63, IGNORE, 0x7,    IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__btype },
    { "jalr",      INSTR_jalr,      0x67, IGNORE, 0x0,    IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__itype },
    { "jal",       INSTR_jal,       0x6f, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__jtype },
    { "ebreak",    INSTR_ebreak,    0x73, IGNORE, 0x0,    IGNORE, IGNORE, IGNORE, IGNORE, 0x001,  IGNORE_RVC, decode__itype },
    { "ecall",     INSTR_ecall,     0x73, IGNORE, 0x0,    IGNORE, IGNORE, IGNORE, IGNORE, 0x000,  IGNORE_RVC, decode__itype },
    { "csrrw",     INSTR_csrrw,     0x73, IGNORE, 0x1,    IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__itype },
    { "csrrs",     INSTR_csrrs,     0x73, IGNORE, 0x2,    IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__itype },
    { "csrrc",     INSTR_csrrc,     0x73, IGNORE, 0x3,    IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__itype },
    { "csrrwi",    INSTR_csrrwi,    0x73, IGNORE, 0x5,    IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__itype },
    { "csrrsi",    INSTR_csrrsi,    0x73, IGNORE, 0x6,    IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__itype },
    { "csrrci",    INSTR_csrrci,    0x73, IGNORE, 0x7,    IGNORE, IGNORE, IGNORE, IGNORE, IGNORE, IGNORE_RVC, decode__itype },
};

static InstrInfo rvc_instr_table[] = {
    /* name         kind          ...          copc  cf1     cf2h    cf2l    cf3  cf5h,   cf5l    decode         */
    { "c.addi4spn", INSTR_addi,   IGNORE_NRVC, 0x0,  IGNORE, IGNORE, IGNORE, 0x0, IGNORE, IGNORE, decode__ciwtype },
    { "c.fld",      INSTR_fld,    IGNORE_NRVC, 0x0,  IGNORE, IGNORE, IGNORE, 0x1, IGNORE, IGNORE, decode__cltype2 },
    { "c.lw",       INSTR_lw,     IGNORE_NRVC, 0x0,  IGNORE, IGNORE, IGNORE, 0x2, IGNORE, IGNORE, decode__cltype },
    { "c.ld",       INSTR_ld,     IGNORE_NRVC, 0x0,  IGNORE, IGNORE, IGNORE, 0x3, IGNORE, IGNORE, decode__cltype2 },
    { "c.fsd",      INSTR_fsd,    IGNORE_NRVC, 0x0,  IGNORE, IGNORE, IGNORE, 0x5, IGNORE, IGNORE, decode__cstype },
    { "c.sw",       INSTR_sw,     IGNORE_NRVC, 0x0,  IGNORE, IGNORE, IGNORE, 0x6, IGNORE, IGNORE, decode__cstype2 },
    { "c.sd",       INSTR_sd,     IGNORE_NRVC, 0x0,  IGNORE, IGNORE, IGNORE, 0x7, IGNORE, IGNORE, decode__cstype },
    { "c.sub",      INSTR_sub,    IGNORE_NRVC, 0x1,  0x0,    0x3,    0x0,    0x4, IGNORE, IGNORE, decode__catype },
    { "c.xor",      INSTR_xor,    IGNORE_NRVC, 0x1,  0x0,    0x3,    0x1,    0x4, IGNORE, IGNORE, decode__catype },
    { "c.or",       INSTR_or,     IGNORE_NRVC, 0x1,  0x0,    0x3,    0x2,    0x4, IGNORE, IGNORE, decode__catype },
    { "c.and",      INSTR_and,    IGNORE_NRVC, 0x1,  0x0,    0x3,    0x3,    0x4, IGNORE, IGNORE, decode__catype },
    { "c.subw",     INSTR_subw,   IGNORE_NRVC, 0x1,  0x1,    0x3,    0x0,    0x4, IGNORE, IGNORE, decode__catype },
    { "c.addw",     INSTR_addw,   IGNORE_NRVC, 0x1,  0x1,    0x3,    0x1,    0x4, IGNORE, IGNORE, decode__catype },
    { "c.addi16sp", INSTR_addi,   IGNORE_NRVC, 0x1,  IGNORE, IGNORE, IGNORE, 0x3, 0x02,   IGNORE, decode__citype3 },
    { "c.addi",     INSTR_addi,   IGNORE_NRVC, 0x1,  IGNORE, IGNORE, IGNORE, 0x0, IGNORE, IGNORE, decode__citype },
    { "c.addiw",    INSTR_addiw,  IGNORE_NRVC, 0x1,  IGNORE, IGNORE, IGNORE, 0x1, IGNORE, IGNORE, decode__citype },
    { "c.li",       INSTR_addi,   IGNORE_NRVC, 0x1,  IGNORE, IGNORE, IGNORE, 0x2, IGNORE, IGNORE, decode__citype },
    { "c.lui",      INSTR_lui,    IGNORE_NRVC, 0x1,  IGNORE, IGNORE, IGNORE, 0x3, IGNORE, IGNORE, decode__citype5 },
    { "c.srli",     INSTR_srli,   IGNORE_NRVC, 0x1,  IGNORE, 0x0,    IGNORE, 0x4, IGNORE, IGNORE, decode__cbtype2 },
    { "c.srai",     INSTR_srai,   IGNORE_NRVC, 0x1,  IGNORE, 0x1,    IGNORE, 0x4, IGNORE, IGNORE, decode__cbtype2 },
    { "c.andi",     INSTR_andi,   IGNORE_NRVC, 0x1,  IGNORE, 0x2,    IGNORE, 0x4, IGNORE, IGNORE, decode__cbtype2 },
    { "c.j",        INSTR_jal,    IGNORE_NRVC, 0x1,  IGNORE, IGNORE, IGNORE, 0x5, IGNORE, IGNORE, decode__cjtype },
    { "c.beqz",     INSTR_beq,    IGNORE_NRVC, 0x1,  IGNORE, IGNORE, IGNORE, 0x6, IGNORE, IGNORE, decode__cbtype },
    { "c.bnez",     INSTR_bne,    IGNORE_NRVC, 0x1,  IGNORE, IGNORE, IGNORE, 0x7, IGNORE, IGNORE, decode__cbtype },
    { "c.jr",       INSTR_jalr,   IGNORE_NRVC, 0x2,  0x0,    IGNORE, IGNORE, 0x4, IGNORE, 0x00,   decode__crtype },
    { "c.mv",       INSTR_add,    IGNORE_NRVC, 0x2,  0x0,    IGNORE, IGNORE, 0x4, IGNORE, IGNORE, decode__crtype },
    { "c.ebreak",   INSTR_ebreak, IGNORE_NRVC, 0x2,  0x1,    IGNORE, IGNORE, 0x4, 0x00,   0x00,   decode__crtype },
    { "c.jalr",     INSTR_jalr,   IGNORE_NRVC, 0x2,  0x1,    IGNORE, IGNORE, 0x4, IGNORE, 0x00,   decode__crtype },
    { "c.add",      INSTR_add,    IGNORE_NRVC, 0x2,  0x1,    IGNORE, IGNORE, 0x4, IGNORE, IGNORE, decode__crtype },
    { "c.slli",     INSTR_slli,   IGNORE_NRVC, 0x2,  IGNORE, IGNORE, IGNORE, 0x0, IGNORE, IGNORE, decode__citype },
    { "c.fldsp",    INSTR_fld,    IGNORE_NRVC, 0x2,  IGNORE, IGNORE, IGNORE, 0x1, IGNORE, IGNORE, decode__citype2 },
    { "c.lwsp",     INSTR_lw,     IGNORE_NRVC, 0x2,  IGNORE, IGNORE, IGNORE, 0x2, IGNORE, IGNORE, decode__citype4 },
    { "c.ldsp",     INSTR_ld,     IGNORE_NRVC, 0x2,  IGNORE, IGNORE, IGNORE, 0x3, IGNORE, IGNORE, decode__citype2 },
    { "c.fsdsp",    INSTR_fsd,    IGNORE_NRVC, 0x2,  IGNORE, IGNORE, IGNORE, 0x5, IGNORE, IGNORE, decode__csstype },
    { "c.swsp",     INSTR_sw,     IGNORE_NRVC, 0x2,  IGNORE, IGNORE, IGNORE, 0x6, IGNORE, IGNORE, decode__csstype2 },
    { "c.sdsp",     INSTR_sd,     IGNORE_NRVC, 0x2,  IGNORE, IGNORE, IGNORE, 0x7, IGNORE, IGNORE, decode__csstype },
};

bool decode_instr(u32 raw, Instr *out)
{
    InstrInfo info = {0};
    if (!decode_instr_info(raw, &info))
        return false;
    *out = info.decode(&info, raw);
    return true;
}

bool decode_instr_info(u32 raw, InstrInfo *out)
{
    u16 quadrant = QUADRANT(raw);
    if (quadrant == 0x3) {
        u16 opcode     = OPCODE(raw);
        u16 funct2     = FUNCT2(raw);
        u16 funct3     = FUNCT3(raw);
        u16 funct5high = FUNCT5HIGH(raw);
        u16 funct5low  = FUNCT5LOW(raw);
        u16 funct6     = FUNCT6(raw);
        u16 funct7     = FUNCT7(raw);
        u16 funct12    = FUNCT12(raw);

        for (u32 i = 0; i < ARRAY_SIZE(nrvc_instr_table); i++) {
            InstrInfo info = nrvc_instr_table[i];
            if (info.opcode     == opcode     &&
               (info.funct2     == funct2     || info.funct2     == IGNORE) &&
               (info.funct3     == funct3     || info.funct3     == IGNORE) &&
               (info.funct5high == funct5high || info.funct5high == IGNORE) &&
               (info.funct5low  == funct5low  || info.funct5low  == IGNORE) &&
               (info.funct6     == funct6     || info.funct6     == IGNORE) &&
               (info.funct7     == funct7     || info.funct7     == IGNORE) &&
               (info.funct12    == funct12    || info.funct12    == IGNORE)) {
                *out = info;
                return true;
            }
        }
    } else {
        u16 copcode     = COPCODE(raw);
        u16 cfunct1     = CFUNCT1(raw);
        u16 cfunct2high = CFUNCT2HIGH(raw);
        u16 cfunct2low  = CFUNCT2LOW(raw);
        u16 cfunct3     = CFUNCT3(raw);
        u16 cfunct5high = CFUNCT5HIGH(raw);
        u16 cfunct5low  = CFUNCT5LOW(raw);

        for (u32 i = 0; i < ARRAY_SIZE(rvc_instr_table); i++) {
            InstrInfo info = rvc_instr_table[i];
            if (info.copcode     == copcode     &&
               (info.cfunct1     == cfunct1     || info.cfunct1     == IGNORE) &&
               (info.cfunct2high == cfunct2high || info.cfunct2high == IGNORE) &&
               (info.cfunct2low  == cfunct2low  || info.cfunct2low  == IGNORE) &&
               (info.cfunct3     == cfunct3     || info.cfunct3     == IGNORE) &&
               (info.cfunct5high == cfunct5high || info.cfunct5high == IGNORE) &&
               (info.cfunct5low  == cfunct5low  || info.cfunct5low  == IGNORE)) {
                *out = info;
                return true;
            }
        }
    }
    return false;
}

const char *instr_to_string(InstrKind kind)
{
#define TO_STRING(name, a0, a1, a2, a3, a4) case INSTR_##name: return #name;
    switch (kind) {
    INSTRUCTION_LIST(TO_STRING)
    default:
        unreachable();
    }
#undef TO_STRING
}
