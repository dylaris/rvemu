.option norvc
.option nopic

# @ident: name=slliw, opcode=0x1b, funct3=0x1, funct7=0x00

# @expect: rd=1, rs1=2
slliw x1, x2, 1

# @expect: rd=3, rs1=4
slliw x3, x4, 15

# @expect: rd=5, rs1=6
slliw x5, x6, 31

