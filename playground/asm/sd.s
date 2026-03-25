.option norvc
.option nopic

# @ident: name=sd, opcode=0x23, funct3=0x3

# @expect: rd=0, rs1=1, rs2=2, imm=0
sd x2, 0(x1)

# @expect: rd=0, rs1=3, rs2=4, imm=8
sd x4, 8(x3)

