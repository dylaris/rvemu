#ifndef REG_H
#define REG_H

#include "common.h"

typedef enum {
    GPR_ZERO = 0, GPR_RA, GPR_SP, GPR_GP, GPR_TP,
    GPR_T0, GPR_T1, GPR_T2,
    GPR_S0, GPR_S1,
    GPR_A0, GPR_A1, GPR_A2, GPR_A3, GPR_A4, GPR_A5, GPR_A6, GPR_A7,
    GPR_S2, GPR_S3, GPR_S4, GPR_S5, GPR_S6, GPR_S7, GPR_S8, GPR_S9, GPR_S10, GPR_S11,
    GPR_T3, GPR_T4, GPR_T5, GPR_T6,
    NUM_GPRS,
} GPRIndex;

typedef enum {
    FPR_FT0 = 0, FPR_FT1, FPR_FT2, FPR_FT3, FPR_FT4, FPR_FT5, FPR_FT6, FPR_FT7,
    FPR_FS0, FPR_FS1,
    FPR_FA0, FPR_FA1, FPR_FA2, FPR_FA3, FPR_FA4, FPR_FA5, FPR_FA6, FPR_FA7,
    FPR_FS2, FPR_FS3, FPR_FS4, FPR_FS5, FPR_FS6, FPR_FS7, FPR_FS8, FPR_FS9, FPR_FS10, FPR_FS11,
    FPR_FT8, FPR_FT9, FPR_FT10, FPR_FT11,
    NUM_FPRS,
} FPRIndex;

typedef u64 GPR;
typedef union {
    u64 q;
    f64 d;
    u32 w;
    f32 s;
} FPR;

#endif // REG_H
