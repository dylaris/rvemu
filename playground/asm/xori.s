.option norvc
.option nopic

# @ident: name=xori, opcode=0b0010011, funct3=0x4

# @expect: rd=1, rs1=2, imm=0
xori x1, x2, 0
# @expect: rd=2, rs1=3, imm=1
xori x2, x3, 1
# @expect: rd=3, rs1=4, imm=-1
xori x3, x4, -1
# @expect: rd=4, rs1=5, imm=2047
xori x4, x5, 2047
# @expect: rd=5, rs1=6, imm=-2048
xori x5, x6, -2048
# @expect: rd=6, rs1=7, imm=0x55
xori x6, x7, 0x55
# @expect: rd=7, rs1=8, imm=0xAA
xori x7, x8, 0xAA
