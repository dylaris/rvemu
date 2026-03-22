.option norvc
.option nopic

# @ident: name=lb, opcode=0b0000011, funct3=0x0

# @expect: rd=1, rs1=2, imm=0
lb x1, 0(x2)
# @expect: rd=2, rs1=3, imm=1
lb x2, 1(x3)
# @expect: rd=3, rs1=4, imm=-1
lb x3, -1(x4)
# @expect: rd=4, rs1=5, imm=15
lb x4, 15(x5)
# @expect: rd=5, rs1=6, imm=-20
lb x5, -20(x6)
# @expect: rd=6, rs1=7, imm=2047
lb x6, 2047(x7)
# @expect: rd=7, rs1=8, imm=-2048
lb x7, -2048(x8)
