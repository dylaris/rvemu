.option norvc
.option nopic

# @ident: name=slli, opcode=0b0010011, funct3=0x1, funct7=0x00

# @expect: rd=1, rs1=2, imm=0
slli x1, x2, 0
# @expect: rd=2, rs1=3, imm=1
slli x2, x3, 1
# @expect: rd=3, rs1=4, imm=7
slli x3, x4, 7
# @expect: rd=4, rs1=5, imm=15
slli x4, x5, 15
# @expect: rd=5, rs1=6, imm=31
slli x5, x6, 31
