#ifndef DECODER_H
#define DECODER_H

#include "utils.h"

#include <stdbool.h>

typedef enum {
    IK_LB, IK_LH, IK_LW, IK_LD, IK_LBU, IK_LHU, IK_LWU,
    IK_FENCE, IK_FENCE_I,
    IK_ADDI, IK_SLLI, IK_SLTI, IK_SLTIU, IK_XORI, IK_SRLI, IK_SRAI, IK_ORI, IK_ANDI, IK_AUIPC, IK_ADDIW, IK_SLLIW, IK_SRLIW, IK_SRAIW,
    IK_SB, IK_SH, IK_SW, IK_SD,
    IK_ADD, IK_SLL, IK_SLT, IK_SLTU, IK_XOR, IK_SRL, IK_OR, IK_AND,
    IK_MUL, IK_MULH, IK_MULHSU, IK_MULHU, IK_DIV, IK_DIVU, IK_REM, IK_REMU,
    IK_SUB, IK_SRA, IK_LUI,
    IK_ADDW, IK_SLLW, IK_SRLW, IK_MULW, IK_DIVW, IK_DIVUW, IK_REMW, IK_REMUW, IK_SUBW, IK_SRAW,
    IK_BEQ, IK_BNE, IK_BLT, IK_BGE, IK_BLTU, IK_BGEU,
    IK_JALR, IK_JAL, IK_ECALL, IK_EBREAK,
    IK_CSRRC, IK_CSRRCI, IK_CSRRS, IK_CSRRSI, IK_CSRRW, IK_CSRRWI,
    IK_FLW, IK_FSW,
    IK_FMADD_S, IK_FMSUB_S, IK_FNMSUB_S, IK_FNMADD_S, IK_FADD_S, IK_FSUB_S, IK_FMUL_S, IK_FDIV_S, IK_FSQRT_S,
    IK_FSGNJ_S, IK_FSGNJN_S, IK_FSGNJX_S,
    IK_FMIN_S, IK_FMAX_S,
    IK_FCVT_W_S, IK_FCVT_WU_S, IK_FMV_X_W,
    IK_FEQ_S, IK_FLT_S, IK_FLE_S, IK_FCLASS_S,
    IK_FCVT_S_W, IK_FCVT_S_WU, IK_FMV_W_X, IK_FCVT_L_S, IK_FCVT_LU_S,
    IK_FCVT_S_L, IK_FCVT_S_LU,
    IK_FLD, IK_FSD,
    IK_FMADD_D, IK_FMSUB_D, IK_FNMSUB_D, IK_FNMADD_D,
    IK_FADD_D, IK_FSUB_D, IK_FMUL_D, IK_FDIV_D, IK_FSQRT_D,
    IK_FSGNJ_D, IK_FSGNJN_D, IK_FSGNJX_D,
    IK_FMIN_D, IK_FMAX_D,
    IK_FCVT_S_D, IK_FCVT_D_S,
    IK_FEQ_D, IK_FLT_D, IK_FLE_D, IK_FCLASS_D,
    IK_FCVT_W_D, IK_FCVT_WU_D, IK_FCVT_D_W, IK_FCVT_D_WU,
    IK_FCVT_L_D, IK_FCVT_LU_D,
    IK_FMV_X_D, IK_FCVT_D_L, IK_FCVT_D_LU, IK_FMV_D_X,
    IK_LR_W, IK_SC_W,
    IK_AMOSWAP_W, IK_AMOADD_W, IK_AMOAND_W, IK_AMOOR_W, IK_AMOXOR_W, IK_AMOMAX_W, IK_AMOMIN_W, IK_AMOMAXU_W, IK_AMOMINU_W,
    IK_LR_D, IK_SC_D,
    IK_AMOSWAP_D, IK_AMOADD_D, IK_AMOAND_D, IK_AMOOR_D, IK_AMOXOR_D, IK_AMOMAX_D, IK_AMOMIN_D, IK_AMOMAXU_D, IK_AMOMINU_D,
    num_insts,
} InstKind;

typedef struct {
    i8 rd;
    i8 rs1;
    i8 rs2;
    i8 rs3;
    i32 imm;
    InstKind kind;
    bool rvc;   // Is RISCV compression extension?
    bool brk;   // Break out of current block (control flow changed)
} Inst;

typedef struct InstDef InstDef;
struct InstDef {
    const char *name;
    InstKind kind;

    u16 opcode;      // [6:0]
    u16 funct2;      // [26:25]
    u16 funct3;      // [14:12]
    u16 funct5high;  // [31:27]
    u16 funct5low;   // [24:20]
    u16 funct7;      // [31:25]
    u16 funct12;     // [31:20]

    u16 copcode;     // [1:0]
    u16 cfunct1;     // [12]
    u16 cfunct2high; // [11:10]
    u16 cfunct2low;  // [6:5]
    u16 cfunct3;     // [15:13]
    u16 cfunct5high; // [11:7]
    u16 cfunct5low;  // [6:2]

    Inst (*decode)(const InstDef *, u32);
};

typedef enum {
    RI_ZERO, RI_RA, RI_SP, RI_GP, RI_TP,
    RI_T0, RI_T1, RI_T2,
    RI_S0, RI_S1,
    RI_A0, RI_A1, RI_A2, RI_A3, RI_A4, RI_A5, RI_A6, RI_A7,
    RI_S2, RI_S3, RI_S4, RI_S5, RI_S6, RI_S7, RI_S8, RI_S9, RI_S10, RI_S11,
    RI_T3, RI_T4, RI_T5, RI_T6,
    num_regs,
} RegIndex;

void inst_decode(Inst *instp, u32 data);
InstDef inst_lookup(u32 data);

#endif // DECODER_H
