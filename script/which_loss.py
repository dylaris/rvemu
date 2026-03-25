import os

insts = [
    "lb", "lh", "lw", "ld", "lbu", "lhu", "lwu", "flw", "fld", "pause", "fence.tso", "fence", "fence.i", "addi", "slli",
    "slti", "sltiu", "xori", "srai", "srli", "ori", "andi", "auipc", "addiw", "slliw", "srliw", "sraiw", "sb", "sh", "sw",
    "sd", "fsw", "fsd", "amoadd.w", "amoswap.w", "lr.w", "sc.w", "amoxor.w", "amoor.w", "amoand.w", "amomin.w", "amomax.w",
    "amominu.w", "amomaxu.w", "amoadd.d", "amoswap.d", "lr.d", "sc.d", "amoxor.d", "amoor.d", "amoand.d", "amomin.d", "amomax.d",
    "amominu.d", "amomaxu.d", "add", "mul", "sub", "sll", "mulh", "slt", "mulhsu", "sltu", "mulhu", "xor", "div", "srl", "divu",
    "sra", "or", "rem", "and", "remu", "lui", "addw", "mulw", "subw", "sllw", "divw", "srlw", "divuw", "sraw", "remw", "remuw",
    "fmadd.s", "fmadd.d", "fmsub.s", "fmsub.d", "fnmsub.s", "fnmsub.d", "fnmadd.s", "fnmadd.d", "fcvt.d.wu", "fmv.x.w", "fmv.x.d",
    "fmv.w.x", "fmv.d.x", "fsgnj.s", "fsgnj.d", "fmin.s", "fmin.d", "fle.s", "fle.d", "fclass.s", "fclass.d", "fsgnjn.s", "fsgnjn.d",
    "fmax.s", "fmax.d", "flt.s", "flt.d", "fsgnjx.s", "fsgnjx.d", "feq.s", "feq.d", "fcvt.d.s", "fsqrt.s", "fsqrt.d", "fcvt.w.s", "fcvt.w.d",
    "fcvt.s.w", "fcvt.d.w", "fcvt.s.d", "fcvt.wu.s", "fcvt.wu.d", "fcvt.s.wu", "fcvt.l.s", "fcvt.l.d", "fcvt.s.l", "fcvt.d.l", "fcvt.lu.s",
    "fcvt.lu.d", "fcvt.s.lu", "fcvt.d.lu", "fadd.s", "fadd.d", "fsub.s", "fsub.d", "fmul.s", "fmul.d", "fdiv.s", "fdiv.d", "beq", "bne", "blt",
    "bge", "bltu", "bgeu", "jalr", "jal", "ebreak", "ecall", "csrrw", "csrrs", "csrrc", "csrrwi", "csrrsi", "csrrci", "c.addi4spn", "c.fld",
    "c.lw", "c.ld", "c.fsd", "c.sw", "c.sd", "c.sub", "c.xor", "c.or", "c.and", "c.subw", "c.addw", "c.addi16sp", "c.addi", "c.addiw", "c.li",
    "c.lui", "c.srli", "c.srai", "c.andi", "c.j", "c.beqz", "c.bnez", "c.ebreak", "c.jr", "c.jalr", "c.mv", "c.add", "c.slli", "c.fldsp",
    "c.lwsp", "c.ldsp", "c.fsdsp", "c.swsp", "c.sdsp",
]

asm_dir = "playground/asm"
existing_files = set()

for filename in os.listdir(asm_dir):
    if filename.endswith(".s"):
        inst_name = filename[:-2]
        existing_files.add(inst_name)

missing = []
for inst in insts:
    if inst not in existing_files:
        missing.append(inst)

print(f"total: {len(insts)}")
print(f"exist: {len(existing_files)}")
print(f"miss:  {len(missing)}")
for m in missing:
    print(f"       {m}.s")
