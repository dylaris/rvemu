.option norvc
.option nopic

# @ident: name=srai, opcode=0b0010011, funct3=0x5, funct7=0x20

# funct7 is imm[5:11], so imm[5:11] = 0x20
# which means the imm is like 0b0010_000x_xxxx
# so the actual imm field need to plus 1024

# @expect: rd=1, rs1=2, imm=1024
srai x1, x2, 0
# @expect: rd=2, rs1=3, imm=1025
srai x2, x3, 1
# @expect: rd=3, rs1=4, imm=1031
srai x3, x4, 7
# @expect: rd=4, rs1=5, imm=1039
srai x4, x5, 15
# @expect: rd=5, rs1=6, imm=1055
srai x5, x6, 31
