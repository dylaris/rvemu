#ifndef DECODE_H
#define DECODE_H

#include "common.h"

#define INSTRUCTION_LIST(X) \
    X(lb,  LOAD, i8,  _, _, _) \
    X(lh,  LOAD, i16, _, _, _) \
    X(lw,  LOAD, i32, _, _, _) \
    X(ld,  LOAD, i64, _, _, _) \
    X(lbu, LOAD, u8,  _, _, _) \
    X(lhu, LOAD, u16, _, _, _) \
    X(lwu, LOAD, u32, _, _, _) \
    X(fence,   EMPTY, _, _, _, _) \
    X(fence_i, EMPTY, _, _, _, _) \
    X(ebreak,  EMPTY, _, _, _, _) \
    X(auipc,   AUIPC, _, _, _, _) \
    X(lui,     LUI,   _, _, _, _) \
    X(ecall,   ECALL, _, _, _, _) \
    X(addi,  OP_IMM, rs1 + imm,                            _, _, _) \
    X(slli,  OP_IMM, rs1 << (imm & 0x3f),                  _, _, _) \
    X(slti,  OP_IMM, (i64)rs1 < (i64)imm,                  _, _, _) \
    X(sltiu, OP_IMM, (u64)rs1 < (u64)imm,                  _, _, _) \
    X(xori,  OP_IMM, rs1 ^ imm,                            _, _, _) \
    X(srli,  OP_IMM, rs1 >> (imm & 0x3f),                  _, _, _) \
    X(srai,  OP_IMM, (i64)rs1 >> (imm & 0x3f),             _, _, _) \
    X(ori,   OP_IMM, rs1 | (u64)imm,                       _, _, _) \
    X(andi,  OP_IMM, rs1 & (u64)imm,                       _, _, _) \
    X(addiw, OP_IMM, (i64)(i32)(rs1 + imm),                _, _, _) \
    X(slliw, OP_IMM, (i64)(i32)(rs1 << (imm & 0x1f)),      _, _, _) \
    X(srliw, OP_IMM, (i64)(i32)((u32)rs1 >> (imm & 0x1f)), _, _, _) \
    X(sraiw, OP_IMM, (i64)((i32)rs1 >> (imm & 0x1f)),      _, _, _) \
    X(sb, STORE, u8,  _, _, _) \
    X(sh, STORE, u16, _, _, _) \
    X(sw, STORE, u32, _, _, _) \
    X(sd, STORE, u64, _, _, _) \
    X(add,    OP_REG, rs1 + rs2,                                                            _, _, _) \
    X(sll,    OP_REG, rs1 << (rs2 & 0x3f),                                                  _, _, _) \
    X(slt,    OP_REG, (i64)rs1 < (i64)rs2,                                                  _, _, _) \
    X(sltu,   OP_REG, (u64)rs1 < (u64)rs2,                                                  _, _, _) \
    X(xor,    OP_REG, rs1 ^ rs2,                                                            _, _, _) \
    X(srl,    OP_REG, rs1 >> (rs2 & 0x3f),                                                  _, _, _) \
    X(or,     OP_REG, rs1 | rs2,                                                            _, _, _) \
    X(and,    OP_REG, rs1 & rs2,                                                            _, _, _) \
    X(mul,    OP_REG, rs1 * rs2,                                                            _, _, _) \
    X(mulh,   OP_REG, help_mulh(rs1, rs2),                                                  _, _, _) \
    X(mulhsu, OP_REG, help_mulhsu(rs1, rs2),                                                _, _, _) \
    X(mulhu,  OP_REG, help_mulhu(rs1, rs2),                                                 _, _, _) \
    X(div,    OP_REG, help_div(rs1, rs2),                                                   _, _, _) \
    X(divu,   OP_REG, rs2 == 0 ? UINT64_MAX : rs1 / rs2,                                    _, _, _) \
    X(rem,    OP_REG, help_rem(rs1, rs2),                                                   _, _, _) \
    X(remu,   OP_REG, rs2 == 0 ? rs1 : rs1 % rs2,                                           _, _, _) \
    X(sub,    OP_REG, rs1 - rs2,                                                            _, _, _) \
    X(sra,    OP_REG, (i64)rs1 >> (rs2 & 0x3f),                                             _, _, _) \
    X(addw,   OP_REG, (i64)(i32)(rs1 + rs2),                                                _, _, _) \
    X(sllw,   OP_REG, (i64)(i32)(rs1 << (rs2 & 0x1f)),                                      _, _, _) \
    X(srlw,   OP_REG, (i64)(i32)((u32)rs1 >> (rs2 & 0x1f)),                                 _, _, _) \
    X(mulw,   OP_REG, (i64)(i32)(rs1 * rs2),                                                _, _, _) \
    X(divw,   OP_REG, rs2 == 0 ? UINT64_MAX : (i32)((i64)(i32)rs1 / (i64)(i32)rs2),         _, _, _) \
    X(divuw,  OP_REG, rs2 == 0 ? UINT64_MAX : (i32)((u32)rs1 / (u32)rs2),                   _, _, _) \
    X(remw,   OP_REG, rs2 == 0 ? (i64)(i32)rs1 : (i64)(i32)((i64)(i32)rs1 % (i64)(i32)rs2), _, _, _) \
    X(remuw,  OP_REG, rs2 == 0 ? (i64)(i32)(u32)rs1 : (i64)(i32)((u32)rs1 % (u32)rs2),      _, _, _) \
    X(subw,   OP_REG, (i64)(i32)(rs1 - rs2),                                                _, _, _) \
    X(sraw,   OP_REG, (i64)(i32)((i32)rs1 >> (rs2 & 0x1f)),                                 _, _, _) \
    X(beq,  BRANCH, (u64)rs1 == (u64)rs2, _, _, _) \
    X(bne,  BRANCH, (u64)rs1 != (u64)rs2, _, _, _) \
    X(blt,  BRANCH, (i64)rs1 <  (i64)rs2, _, _, _) \
    X(bge,  BRANCH, (i64)rs1 >= (i64)rs2, _, _, _) \
    X(bltu, BRANCH, (u64)rs1 <  (u64)rs2, _, _, _) \
    X(bgeu, BRANCH, (u64)rs1 >= (u64)rs2, _, _, _) \
    X(jalr, JUMP, (rs1 + (i64)imm) & ~(u64)1, _, _, _) \
    X(jal,  JUMP, pc + (i64)imm,              _, _, _) \
    X(csrrc,  CSR, _, _, _, _) \
    X(csrrci, CSR, _, _, _, _) \
    X(csrrs,  CSR, _, _, _, _) \
    X(csrrsi, CSR, _, _, _, _) \
    X(csrrw,  CSR, _, _, _, _) \
    X(csrrwi, CSR, _, _, _, _) \
    X(flw, FP_LOAD, u32, _, _, _) \
    X(fld, FP_LOAD, u64, _, _, _) \
    X(fsw, FP_STORE, u32, _, _, _) \
    X(fsd, FP_STORE, u64, _, _, _) \
    X(fmadd_s,   FP_FMA4, s, f32, rs1 * rs2 + rs3,    _) \
    X(fmsub_s,   FP_FMA4, s, f32, rs1 * rs2 - rs3,    _) \
    X(fnmsub_s,  FP_FMA4, s, f32, -(rs1 * rs2) + rs3, _) \
    X(fnmadd_s,  FP_FMA4, s, f32, -(rs1 * rs2) - rs3, _) \
    X(fmadd_d,   FP_FMA4, d, f64, rs1 * rs2 + rs3,    _) \
    X(fmsub_d,   FP_FMA4, d, f64, rs1 * rs2 - rs3,    _) \
    X(fnmsub_d,  FP_FMA4, d, f64, -(rs1 * rs2) + rs3, _) \
    X(fnmadd_d,  FP_FMA4, d, f64, -(rs1 * rs2) - rs3, _) \
    X(fadd_s,  FP_BINOP, s, f32, rs1 + rs2,             _) \
    X(fsub_s,  FP_BINOP, s, f32, rs1 - rs2,             _) \
    X(fmul_s,  FP_BINOP, s, f32, rs1 * rs2,             _) \
    X(fdiv_s,  FP_BINOP, s, f32, rs1 / rs2,             _) \
    X(fsqrt_s, FP_BINOP, s, f32, sqrtf(rs1),            _) \
    X(fmin_s,  FP_BINOP, s, f32, rs1 < rs2 ? rs1 : rs2, _) \
    X(fmax_s,  FP_BINOP, s, f32, rs1 > rs2 ? rs1 : rs2, _) \
    X(fadd_d,  FP_BINOP, d, f64, rs1 + rs2,             _) \
    X(fsub_d,  FP_BINOP, d, f64, rs1 - rs2,             _) \
    X(fmul_d,  FP_BINOP, d, f64, rs1 * rs2,             _) \
    X(fdiv_d,  FP_BINOP, d, f64, rs1 / rs2,             _) \
    X(fsqrt_d, FP_BINOP, d, f64, sqrt(rs1),             _) \
    X(fmin_d,  FP_BINOP, d, f64, rs1 < rs2 ? rs1 : rs2, _) \
    X(fmax_d,  FP_BINOP, d, f64, rs1 > rs2 ? rs1 : rs2, _) \
    X(fsgnj_s,  FP_SGNJ, w, u32, false, false) \
    X(fsgnjn_s, FP_SGNJ, w, u32, true,  false) \
    X(fsgnjx_s, FP_SGNJ, w, u32, false, true)  \
    X(fsgnj_d,  FP_SGNJ, q, u64, false, false) \
    X(fsgnjn_d, FP_SGNJ, q, u64, true,  false) \
    X(fsgnjx_d, FP_SGNJ, q, u64, false, true)  \
    X(fcvt_w_s,  FP_XFER_F2I, s, f32, (i64)(i32)llrintf(src),      _) \
    X(fcvt_wu_s, FP_XFER_F2I, s, f32, (i64)(i32)(u32)llrintf(src), _) \
    X(fmv_x_w,   FP_XFER_F2I, w, u32, (i64)(i32)src,               _) \
    X(fcvt_l_s,  FP_XFER_F2I, s, f32, (i64)llrintf(src),           _) \
    X(fcvt_lu_s, FP_XFER_F2I, s, f32, (u64)llrintf(src),           _) \
    X(fcvt_s_lu, FP_XFER_F2I, s, f32, (f32)(u64)src,               _) \
    X(fcvt_w_d,  FP_XFER_F2I, d, f64, (i64)(i32)llrint(src),       _) \
    X(fcvt_wu_d, FP_XFER_F2I, d, f64, (i64)(i32)(u32)llrint(src),  _) \
    X(fcvt_l_d,  FP_XFER_F2I, d, f64, (i64)llrint(src),            _) \
    X(fcvt_lu_d, FP_XFER_F2I, d, f64, (u64)llrint(src),            _) \
    X(fmv_x_d,   FP_XFER_F2I, q, u64, src,                         _) \
    X(fcvt_s_w,  FP_XFER_I2F, s, (f32)(i32)src, _, _) \
    X(fcvt_s_wu, FP_XFER_I2F, s, (f32)(u32)src, _, _) \
    X(fmv_w_x,   FP_XFER_I2F, w, (u32)src,      _, _) \
    X(fcvt_s_l,  FP_XFER_I2F, s, (f32)(i64)src, _, _) \
    X(fcvt_d_w,  FP_XFER_I2F, d, (f64)(i64)src, _, _) \
    X(fcvt_d_wu, FP_XFER_I2F, d, (f64)(u64)src, _, _) \
    X(fcvt_d_l,  FP_XFER_I2F, d, (f64)(i64)src, _, _) \
    X(fcvt_d_lu, FP_XFER_I2F, d, (f64)(u64)src, _, _) \
    X(fmv_d_x,   FP_XFER_I2F, q, src,           _, _) \
    X(fcvt_s_d, FP_XFER_F2F, d, s, f64, (f32)src) \
    X(fcvt_d_s, FP_XFER_F2F, s, d, f32, (f64)src) \
    X(feq_s, FP_CMP, s, f32, rs1 == rs2, _) \
    X(flt_s, FP_CMP, s, f32, rs1 <  rs2, _) \
    X(fle_s, FP_CMP, s, f32, rs1 <= rs2, _) \
    X(feq_d, FP_CMP, d, f64, rs1 == rs2, _) \
    X(flt_d, FP_CMP, d, f64, rs1 <  rs2, _) \
    X(fle_d, FP_CMP, d, f64, rs1 <= rs2, _) \
    X(fclass_s, FP_CLASS, s, f32, _, _) \
    X(fclass_d, FP_CLASS, d, f64, _, _)

    // X(lr_w, EMPTY, 0) \
    // X(sc_w, EMPTY, 0) \
    // X(amoswap_w, EMPTY, 0) \
    // X(amoadd_w, EMPTY, 0) \
    // X(amoand_w, EMPTY, 0) \
    // X(amoor_w, EMPTY, 0) \
    // X(amoxor_w, EMPTY, 0) \
    // X(amomax_w, EMPTY, 0) \
    // X(amomin_w, EMPTY, 0) \
    // X(amomaxu_w, EMPTY, 0) \
    // X(amominu_w, EMPTY, 0) \
    // X(lr_d, EMPTY, 0) \
    // X(sc_d, EMPTY, 0) \
    // X(amoswap_d, EMPTY, 0) \
    // X(amoadd_d, EMPTY, 0) \
    // X(amoand_d, EMPTY, 0) \
    // X(amoor_d, EMPTY, 0) \
    // X(amoxor_d, EMPTY, 0) \
    // X(amomax_d, EMPTY, 0) \
    // X(amomin_d, EMPTY, 0) \
    // X(amomaxu_d, EMPTY, 0) \
    // X(amominu_d, EMPTY,)

#define X(name, tag, a1, a2, a3, a4) INSTR_##name,
typedef enum { INSTRUCTION_LIST(X) NUM_INSTRS, } InstrKind;
#undef X

// Dynamic runtime instruction
typedef struct {
    i8 rd;
    i8 rs1;
    i8 rs2;
    i8 rs3;
    i32 imm;
    InstrKind kind;
    bool rvc; // Is RISCV compression extension?
    bool cfc; // Is control flow changed?
} Instr;

// Static instruction metadata
typedef struct InstrInfo InstrInfo;
struct InstrInfo {
    const char *name;
    InstrKind kind;

    u16 opcode;      // [6:0]
    u16 funct2;      // [26:25]
    u16 funct3;      // [14:12]
    u16 funct5high;  // [31:27]
    u16 funct5low;   // [24:20]
    u16 funct6;      // [31:26]
    u16 funct7;      // [31:25]
    u16 funct12;     // [31:20]

    u16 copcode;     // [1:0]
    u16 cfunct1;     // [12]
    u16 cfunct2high; // [11:10]
    u16 cfunct2low;  // [6:5]
    u16 cfunct3;     // [15:13]
    u16 cfunct5high; // [11:7]
    u16 cfunct5low;  // [6:2]

    Instr (*decode)(const InstrInfo *, u32);
};

typedef enum {
    CSR_FFLAGS = 0x001,
    CSR_FRM    = 0x002,
    CSR_FCSR   = 0x003,
} CSR;

bool decode_instr(u32 raw, Instr *out);
bool decode_instr_info(u32 raw, InstrInfo *out);
const char *instr_to_string(InstrKind kind);

#endif // DECODE_H
