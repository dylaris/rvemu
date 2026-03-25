.option norvc
.option nopic

# @ident: name=lwu, opcode=0x03, funct3=0x6

# @expect: rd=1, rs1=2, imm=0
lwu x1, 0(x2)

# @expect: rd=3, rs1=4, imm=4
lwu x3, 4(x4)

# @expect: rd=5, rs1=6, imm=-8
lwu x5, -8(x6)

